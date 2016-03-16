#include "Framebuffer.h"
#include <driver.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <dayos.h>

#define CLAMP(x, min, max) ((x < min) ? min : ((x > max) ? max : x))

void outb(unsigned short port, unsigned char value)
{
	asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

unsigned char inb(unsigned short port)
{
	unsigned char ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

Framebuffer::Framebuffer() :
	videoMem((uint8_t*) 0xb8000),
	x(0),
	y(0),
	isHex(false),
	current_color(Color(LIGHT_GREY, BLACK))
{
	setColors(current_color);
	framebuffer = new uint8_t[80*25*2];
}

void Framebuffer::putch(char s)
{
	if(!s) return;
	
	switch(s)
	{
	case '\n':
	{
		x = 0;
		y++;
		scroll();
		updateCursor();
		return;
	}
	break;

	case '\b':
	{
		x--;
		
		if(x <= 0 && y > 0)
		{
			x = 79;
			y--;
		}
		else if(x <= 0 && y <= 0)
			x = 0;

		videoMem[2 * (y * 80 + x)] = 0;
		updateCursor();
		
		return;
	}
	
	case '\t':
		*this << "    ";
	return;
	}


	videoMem[2 * (y * 80 + x)] = s;
	videoMem[2 * (y * 80 + x) + 1] = attribute;

	x++;

	if(x >= 80)
	{
		x = 0;
		y++;
	}

	scroll();
	updateCursor();
}

void Framebuffer::scroll()
{
	if(y > 24)
	{
		int i;

		for(i = 0; i < 2 * 27 * 80; i++)
		{
			videoMem[i] = videoMem[i + 160];
		}

		for(; i < 2 * 25 * 80; i++)
		{
			videoMem[i] = 0;
		}

		y--;
	}
}

void Framebuffer::clear()
{
	int i;

	for(i = 0; i < 2 * 25 * 80; i = i + 2)
	{
		videoMem[i] = 0;
		videoMem[i + 1] = attribute;
	}

	x = y = 0;
	updateCursor();
}

Framebuffer& Framebuffer::operator << (const char* s)
{
	while(*s)
	{
		s = parseEscape(s);
		putch(*s);
		s++;
	}

	return *this;
}

const char* Framebuffer::parseGraphicsSequence(const char* str)
{
	// Handle graphics sequence
	while(*str != 0 && (isdigit(*str) || *str == ';'))
	{
		if(*str == '3')
		{
			str++;

			switch(*str)
			{
			case '0':
				setColors(Color(BLACK, current_color.getBackground()));
				break;

			case '1':
				setColors(Color(LIGHT_RED, current_color.getBackground()));
				break;

			case '2':
				setColors(Color(LIGHT_GREEN, current_color.getBackground()));
				break;

			case '3':
				setColors(Color(BROWN, current_color.getBackground()));
				break;

			case '4':
				setColors(Color(LIGHT_BLUE, current_color.getBackground()));
				break;

			case '5':
				setColors(Color(MAGENTA, current_color.getBackground()));
				break;

			case '6':
				setColors(Color(CYAN, current_color.getBackground()));
				break;

			case '7':
				setColors(Color(WHITE, current_color.getBackground()));
				break;

			case '9':
				setColors(Color(LIGHT_GREY, current_color.getBackground()));
				break;
			}
		}
		else if(*str == '4')
		{
			str++;

			switch(*str)
			{
			case '0':
				setColors(Color(current_color.getForeground(), BLACK));
				break;

			case '1':
				setColors(Color(current_color.getForeground(), RED));
				break;

			case '2':
				setColors(Color(current_color.getForeground(), GREEN));
				break;

			case '3':
				setColors(Color(current_color.getForeground(), BROWN));
				break;

			case '4':
				setColors(Color(current_color.getForeground(), BLUE));
				break;

			case '5':
				setColors(Color(current_color.getForeground(), MAGENTA));
				break;

			case '6':
				setColors(Color(current_color.getForeground(), CYAN));
				break;

			case '7':
				setColors(Color(current_color.getForeground(), WHITE));
				break;

			case '9':
				setColors(Color(current_color.getForeground(), LIGHT_GREY));
				break;
			}
		}

		str++;
	}

	return str + 1;
}

// http://www.isthe.com/chongo/tech/comp/ansi_escapes.html
const char* Framebuffer::parseEscape(const char* str)
{
	const char* start = str;
	int state = 0;
	
	while(*str)
	{
		switch(state)
		{
		case 0:
			if(*str == '\x1B')
				state = 1;
			else
				return start;

			break;

		case 1:
			if(*str == '[')
				state = 2;
			else
				return start;

			break;

		case 2:
			{
				// Find right type (last char of sequence)
				const char* type = str;
				while(*type && (isdigit(*type) || *type == ';'))
					type++;
				
				switch(*type)
				{
					case 'm': parseGraphicsSequence(str); break;
					case 'j': clear(); break;					
					case 'f':
					case 'h': 
						
						x = CLAMP(atoi(str), 0, 79);
						str = strchr(str, ';');
						
						if(!str)
							return type + 1;
						
						str++;
						y = CLAMP(atoi(str), 0, 24);
						updateCursor();
						break;
						
					case 'a':
						y -= atoi(str);
						y = CLAMP(y, 0, 24);
						updateCursor();
						break;

					case 'b':
						y += atoi(str);
						y = CLAMP(y, 0, 24);
						updateCursor();
						break;
						
					case 'c':
						x += atoi(str);
						x = CLAMP(x, 0, 79);
						updateCursor();
						break;
						
					case 'd':
						x -= atoi(str);
						x = CLAMP(x, 0, 79);
						updateCursor();
						break;
				}

				// Handle further sequences that are directly following
				start = type + 1;
				str = type;
				state = 0;
			}
			break;
		}

		str++;
	}

	return str;
}

void Framebuffer::putNumber(int x, int base)
{
	char buf[65];
	const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char* p;

	if(base > 36)
	{
		return;
	}

	p = buf + 64;
	*p = '\0';

	do
	{
		*--p = digits[x % base];
		x /= base;
	}
	while(x);

	while(*p)
	{
		putch(*p++);
	}

}

Framebuffer& Framebuffer::operator << (int z)
{
	if(isHex)
	{
		putNumber(z, 16);
	}
	else
	{
		putNumber(z, 10);
	}

	return *this;
}

Framebuffer& Framebuffer::operator << (uint32_t z)
{
	if(isHex)
	{
		putNumber((int) z, 16);
	}
	else
	{
		putNumber((int) z, 10);
	}

	return *this;
}

void Framebuffer::setHex(bool neu)
{
	isHex = neu;
}

void Framebuffer::setColors(Color farbe)
{
	attribute = (farbe.getBackground() << 4) | (farbe.getForeground() & 0x0F);
	current_color = farbe;
}

Framebuffer& Framebuffer::operator << (Color f)
{
	setColors(f);
	return *this;
}

void Framebuffer::updateCursor()
{
	unsigned temp;

	temp = y * 80 + x;

	outb(0x3D4, 14);
	outb(0x3D5, temp >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, temp);
}

void Framebuffer::setCursor(int _x, int _y)
{
	x = _x;
	y = _y;

	updateCursor();
}

void Framebuffer::setCursor(Vector2 n)
{
	x = n.X;
	y = n.Y;

	updateCursor();
}

Vector2 Framebuffer::getCursor()
{
	Vector2 tmp = {(int)x, (int)y};
	return tmp;
}
