#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <sys/types.h>
#include <vector>

struct Color
{
	uint8_t r,g,b;
	Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

class VesaFramebuffer
{
	struct VbeInfoBlock
	{
		char vbe_signature[4];             // == "VESA"
		uint16_t vbe_version;                 // == 0x0300 for VBE 3.0
		uint16_t oem_string_ptr[2];            // isa vbeFarPtr
		uint8_t capabilities[4];
		uint16_t video_mode_ptr[2];         // isa vbeFarPtr
		uint16_t total_memory;             // as # of 64KB blocks
	} __attribute__((packed));

	struct VbeModeInfo
	{
		uint16_t attributes;      // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
		uint8_t window_a;         // deprecated
		uint8_t window_b;         // deprecated
		uint16_t granularity;      // deprecated; used while calculating bank numbers
		uint16_t window_size;
		uint16_t segment_a;
		uint16_t segment_b;
		uint32_t win_func_ptr;      // deprecated; used to switch banks from protected mode without returning to real mode
		uint16_t pitch;         // number of bytes per horizontal line
		uint16_t width;         // width in pixels
		uint16_t height;         // height in pixels
		uint8_t w_char;         // unused...
		uint8_t y_char;         // ...
		uint8_t planes;
		uint8_t bpp;         // bits per pixel in this mode
		uint8_t banks;         // deprecated; total number of banks in this mode
		uint8_t memory_model;
		uint8_t bank_size;      // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
		uint8_t image_pages;
		uint8_t reserved0;

		uint8_t red_mask;
		uint8_t red_position;
		uint8_t green_mask;
		uint8_t green_position;
		uint8_t blue_mask;
		uint8_t blue_position;
		uint8_t reserved_mask;
		uint8_t reserved_position;
		uint8_t direct_color_attributes;

		uint32_t framebuffer;      // physical address of the linear frame buffer; write here to draw to the screen
		uint32_t off_screen_mem_off;
		uint16_t off_screen_mem_size;   // size of memory in the framebuffer but not being displayed on the screen
		uint8_t reserved1[206];
	} __attribute__ ((packed));

	struct VesaMode
	{
		uint16_t id;
		VbeModeInfo info;

		VesaMode(uint16_t id, VbeModeInfo& info) : id(id), info(info) {}
		VesaMode() {}
	};
	std::vector<VesaMode> m_modes;

	uint16_t findVesaMode(uint32_t w, uint32_t h, uint8_t d);
	void buildModeList();
	void setVideoMode(VesaMode& mode);

	uint32_t width, height, bitsPerPixel, bytesPerPixel, bytesPerLine, bytesPerColumn;
	char* framebuffer;
	char* backbuffer;

	char* currentBuffer;

public:
	bool setVideoMode(uint32_t w, uint32_t h, uint8_t d);
	void videoModeMenu();

	void setPixel(uint32_t x, uint32_t y, Color c);
	void slowFill(int x, int y, int w, int h,  Color c);

	void swapBuffer();

	uint32_t getWidth() const
	{
		return width;
	}

	uint32_t getHeight() const
	{
		return height;
	}
};

#endif // FRAMEBUFFER_H
