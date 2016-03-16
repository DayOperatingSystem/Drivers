#ifndef ANSITERMINAL_H
#define ANSITERMINAL_H

#include <CharDevice.h>
#include <map>
#include <vector>
#include "Framebuffer.h"

class AnsiTerminal : public IO::CharDevice
{
	unsigned int m_id;
	std::map<int, int>& m_keymap;
	Framebuffer m_framebuffer;
	
	bool modifiers[3];
public:
	AnsiTerminal(unsigned int id, std::map<int, int>& kmap) : m_id(id), m_keymap(kmap) { initialize(); }
	bool initialize();
	const char* getName() { return "ANSI Compatible Terminal"; }
	
	size_t write(void* what, size_t size);
	
	void enable() { m_framebuffer.restore(); }
	void disable() { m_framebuffer.save(); }
	
	bool handle(message_t& msg);
	void clear() { m_framebuffer.clear(); }
};

#endif // ANSITERMINAL_H
