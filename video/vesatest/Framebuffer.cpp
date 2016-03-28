#include "Framebuffer.h"
#include <arch.h>
#include <syscall.h>
#include <dayos.h>
#include <cstring>
#include <memory.h>
#include <cstdio>
#include <cstdlib>

#define DIFF(x, y) ((x) - (y))

void VesaFramebuffer::buildModeList()
{
	regs16_t regs;
	struct VbeInfoBlock* ctrl = (VbeInfoBlock*) 0x2000;
	struct VbeModeInfo* inf = (VbeModeInfo*) 0x3000;

	strncpy(ctrl->vbe_signature, "VBE2", 4);

	regs.ax = 0x4F00;
	regs.es = 0;
	regs.di = (uintptr_t) ctrl;
	syscall2(SYSCALL_BIOS_INT, 0x10, (uintptr_t) &regs);

	uint16_t* modes = (uint16_t*) ctrl->video_mode_ptr;
	for(int i = 0; modes[i] != 0xFFFF; i++)
	{
		regs.ax = 0x4F01;
		regs.cx = modes[i];
		regs.di = (uintptr_t) inf;
		syscall2(SYSCALL_BIOS_INT, 0x10, (uintptr_t) &regs);

		if ((uint16_t) regs.ax != 0x004F) continue;

		// Check if this is a graphics mode with linear frame buffer support
		if ((inf->attributes & 0x90) != 0x90) continue;

		// Check if this is a packed pixel or direct color mode
		if (inf->memory_model != 4 && inf->memory_model != 6) continue;

		// Only allow 32bit modes
		if (inf->bpp != 32) continue;
		
		//debug_printf("Found mode: %x %dx%dx%d\n", modes[i], inf->width, inf->height, inf->bpp);
		m_modes.push_back(VesaMode(modes[i], *inf));
	}

}

uint16_t VesaFramebuffer::findVesaMode(uint32_t w, uint32_t h, uint8_t d)
{
	regs16_t regs;
	int i = 0;
	int best = 0;

	int pixdiff, bestpixdiff = DIFF(320 * 200, w * h);
	int depthdiff, bestdepthdiff = 8 >= d ? 8 - d : (d - 8) * 2;

	if(m_modes.size() == 0)
		buildModeList();

	for(VesaFramebuffer::VesaMode& vm : m_modes)
	{
		VbeModeInfo& inf = vm.info;
		// Check if this is exactly the mode we're looking for
		if (w == inf.width && h == inf.height &&
			d == inf.bpp)
			return i;

		// Otherwise, compare to the closest match so far, remember if best
		pixdiff = DIFF(inf.width * inf.height, w * h);
		depthdiff = (inf.bpp >= d) ? inf.bpp - d : (d - inf.bpp) * 2;
		if (bestpixdiff > pixdiff ||
			(bestpixdiff == pixdiff && bestdepthdiff > depthdiff))
		{
			best = i;
			bestpixdiff = pixdiff;
			bestdepthdiff = depthdiff;
		}
		i++;
	}
	if (w == 640 && h == 480 && d == 1) return 0;
	return best;
}

bool VesaFramebuffer::setVideoMode(uint32_t w, uint32_t h, uint8_t d)
{
	VesaMode mode = m_modes[findVesaMode(w, h, d)];
	setVideoMode(mode);
}

void VesaFramebuffer::setVideoMode(VesaMode& mode)
{
	regs16_t regs;

	width = mode.info.width;
	height = mode.info.height;
	bitsPerPixel = mode.info.bpp;
	bytesPerPixel = mode.info.bpp / 8;
	bytesPerLine = width * bytesPerPixel;
	bytesPerColumn = height * bytesPerPixel;

	request_mem_region("VESA", mode.info.framebuffer, width * height * bytesPerPixel * 2);
	framebuffer = (char*) mode.info.framebuffer;

	regs.ax = 0x4F02;
	regs.bx = mode.id | 0x4000;
	syscall2(SYSCALL_BIOS_INT, 0x10, (uintptr_t) &regs); //0x18f);

	backbuffer = new char[width * height * bytesPerPixel];//framebuffer + width * width * height * bytesPerLine; //new char[width * height * bytesPerPixel];
	memset (backbuffer, 0xFF, (width * height * bytesPerPixel));

	currentBuffer = framebuffer;
}

void VesaFramebuffer::videoModeMenu()
{
	if(m_modes.size() == 0)
		buildModeList();

	int idx = 0;
	for(VesaMode& mode : m_modes)
	{
		printf("%d\t%dx%dx%d 0x%x\n", idx, mode.info.width, mode.info.height, mode.info.bpp, mode.id);
		idx++;
	}

	char buffer[20];

repeat_selection:

	printf("Selection: ");
	fflush(stdout);

	fgets(buffer, 20, stdin);
	idx = atoi(buffer);

	if(idx < 0 || idx >= m_modes.size())
	{
		goto repeat_selection;
	}

	setVideoMode(m_modes[idx]);
}

void VesaFramebuffer::setPixel(uint32_t x, uint32_t y, Color c)
{
	if(x > width || y > height)
		return;

	char* start = &currentBuffer[y * bytesPerLine + x * bytesPerPixel];
	*start++ = c.b;
	*start++ = c.g;
	*start++ = c.r;
}

void VesaFramebuffer::slowFill(int x, int y, int w, int h,  Color c)
{
	for(int i = x; i < x + w; i++)
		for(int j = y; j < y + h; j++)
			setPixel(i, j, c);
}

void VesaFramebuffer::swapBuffer()
{
	uint32_t startx = 0, starty = 0;
	if(currentBuffer == framebuffer)
	{
		startx = width;
		starty = height;
		currentBuffer = backbuffer;
	}
	else
		currentBuffer = framebuffer;

	regs16_t regs;
	regs.ax = 0x4F07;
	regs.bx = 0x02; // Set display start
	regs.cx = (uintptr_t) backbuffer;
	regs.dx = (uintptr_t) backbuffer >> 16;
	syscall2(SYSCALL_BIOS_INT, 0x10, (uintptr_t) &regs);

	//memcpy(framebuffer, backbuffer, width * height * bytesPerPixel);
}
