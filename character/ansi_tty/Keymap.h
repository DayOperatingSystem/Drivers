#ifndef KEYMAP_H
#define KEYMAP_H

#include <map>

class Keymap
{
	std::map<int, int> m_keys;
public:
	void map(int scancode, int character) { m_keys.insert(std::pair<int, int>(scancode, character)); }
	int translate(int scancode) { return m_keys[scancode]; }
};

#endif // KEYMAP_H
