#include <stdio.h>
#include <syscall.h>
#include <cstring>
#include <memory>
#include <dayos.h>
#include <memory.h>
#include <driver.h>
#include <arch.h>
#include <iostream>
#include <cstdlib>
#include "Framebuffer.h"
#include <HIDDevice.h>
#include <algorithm>

char* framebuffer;
//vbe_mode_info_structure currentMode;

void setup_vga()
{
	int y;


	// draw horizontal line from 100,80 to 100,240 in multiple colors
	//for (y = 0; y < 200; y++)
	//	memset ((char*) 0xA0000 + (y * 320 + 80), y, 160);

	// wait for key
	//	syscall2(16, 0x16, 0x0);

	// switch to 80x25x16 text mode
	//syscall2(16, 0x10, 0x0003);
}

/*void put_pixel(int x, int y, int color)
{
	framebuffer[y*currentMode.width*currentMode.reserved1[0]+x * currentMode.reserved1[0]] = color;
}

void slow_fill(int x, int y, int w, int h, int color)
{
	for(int i = x; i < x + w; i++)
		for(int j = y; j < y + h; j++)
			put_pixel(i, j, color);
}

void fast_fill(int x, int y, int w, int h, int color)
{
	char* where = framebuffer + (y*currentMode.width * currentMode.reserved1[0] + x * currentMode.reserved1[0]);
	for(int i = 0; i < w; i++)
		{
			for(int j = 0; j < h; j++)
			{
				where[j * currentMode.reserved1[0]] = color;
				//put_pixel(i, j, color);
			}

			where += (currentMode.width * currentMode.reserved1[0]);
		}
}
*/
int main(int argc, char* argv[])
{
	VesaFramebuffer fb;
	if(argc >= 3)
	{
		uint32_t w = 1024, h = 768;
		w = atoi(argv[1]);
		h = atoi(argv[2]);
		fb.setVideoMode(w, h, 32);
	}
	else
		fb.videoModeMenu();

	/*for(int y = 1; y < fb.getHeight(); y++)
		for(int x = 1; x < fb.getWidth(); x++)
			fb.setPixel(x, y, Color(255.0 * ((float) 100.0 / (float) x), 255.0 * (float) 100.0/ (float) y, 0));*/

	IO::HIDDevice::registerObserver("mouse");

	int mx = 0, my = 0;
	bool lb = false;

	message_t msg;
	IO::HIDEvent* event = (IO::HIDEvent*) &msg.message;
	while(1)
	{
		while(receive_message(&msg, MESSAGE_ANY)) sleep(2);
		switch(msg.signal)
		{
			case IO::HID_EVENT:

				if(event->type == IO::HID_AXIS_MOVE)
				{
					if(lb == false)
						fb.slowFill(mx, my, 30, 30, Color(255, 255, 255));

					mx = std::max(1, (int) std::min((int) fb.getWidth()-31, mx + (int) event->data));
					my = std::max(1, (int) std::min((int) fb.getHeight()-31, my - (int) event->data1));

					//debug_printf("mx %d my %d\n", mx, my);

					fb.slowFill(mx, my, 30, 30, Color(150, 0, 150));

				}
				else if(event->type == IO::HID_KEY_DOWN && event->data == 0)
					lb = true;
				else if(event->type == IO::HID_KEY_UP && event->data == 0)
					lb = false;

					break;
		}

		//fb.swapBuffer();
	}
	while(1);
	return 0;
}
