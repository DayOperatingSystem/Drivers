#include <dayos.h>
#include <sys/ioctl.h>
#include <vector>
#include <string>
#include <HIDDevice.h>
#include "Keymap.h"
#include "AnsiTerminal.h"

#define NUM_TERMINALS 4

// Keymap for scancode set1 of the IBM AT keyboard
// according to here: http://www.win.tue.nl/~aeb/linux/kbd/scancodes-10.html#scancodesets
/*void setupDefaultKeymap(std::map<int, int>& map)
{
	
	map.map(0x10, 'Q');
	map.map(0x11, 'W');
	map.map(0x12, 'E');
	map.map(0x13, 'R');
	map.map(0x14, 'T');
	map.map(0x15, 'Y');
	map.map(0x16, 'I');
	map.map(0x17, 'O');
	map.map(0x18, 'P');
	map.map(0x19, '[');
	map.map(0x1A, ']');
	map.map(0x1B, '\\');
	
	
	map.map(0x1E, 'A');
	map.map(0x1F, 'S');
	map.map(0x20, 'D');
	map.map(0x21, 'F');
	map.map(0x22, 'G');
	map.map(0x23, 'H');
	map.map(0x24, 'J');
	map.map(0x25, 'K');
	map.map(0x26, 'L');
	map.map(0x27, ';');
	map.map(0x28, '\'');
	map.map(0x1C, '\n'); // Enter
	map.map(0x2C, 'Z');
	map.map(0x2D, 'X');
	map.map(0x2E, 'C');
	map.map(0x2F, 'V');
	map.map(0x30, 'B');
	map.map(0x31, 'N');
	map.map(0x32, 'M');
	map.map(0x33, ',');
	map.map(0x34, '.');
	map.map(0x35, '/');
}*/

void setupDefaultKeymap(std::map<int, int>& map)
{
	map[0x10] = 'q';
	map[0x11] = 'w';
	map[0x12] = 'e';
	map[0x13] = 'r';
	map[0x14] = 't';
	map[0x15] = 'y';
	map[0x16] = 'u';
	map[0x17] = 'i';
	map[0x18] = 'o';
	map[0x19] = 'p';
	map[0x1A] = '[';
	map[0x1B] = ']';
	map[0x1E] = 'a';
	map[0x1F] = 's';
	map[0x20] = 'd';
	map[0x21] = 'f';
	map[0x22] = 'g';
	map[0x23] = 'h';
	map[0x24] = 'j';
	map[0x25] = 'k';
	map[0x26] = 'l';
	map[0x27] = ';';
	map[0x28] = '\'';
	map[0x1C] = '\n'; // Enter
	map[0x2C] = 'z';
	map[0x2D] = 'x';
	map[0x2E] = 'c';
	map[0x2F] = 'v';
	map[0x30] = 'b';
	map[0x31] = 'n';
	map[0x32] = 'm';
	map[0x33] = ',';
	map[0x34] = '.';
	map[0x35] = '/';

	map[0x39] = ' ';
	
	map[0x29] = '`';
	map[0x02] = '1';
	map[0x03] = '2';
	map[0x04] = '3';
	map[0x05] = '4';
	map[0x06] = '5';
	map[0x07] = '6';
	map[0x08] = '7';
	map[0x09] = '8';
	map[0x0A] = '9';
	map[0x0B] = '0';
	map[0x0C] = '-';
	map[0x0D] = '=';
	map[0x0E] = '\b';
	map[0x0F] = '\t';

	
	map[0x110] = 'Q';
	map[0x111] = 'W';
	map[0x112] = 'E';
	map[0x113] = 'R';
	map[0x114] = 'T';
	map[0x115] = 'Y';
	map[0x116] = 'U';
	map[0x117] = 'I';
	map[0x118] = 'O';
	map[0x119] = 'P';
	map[0x11A] = '{';
	map[0x11B] = '}';
	map[0x11E] = 'A';
	map[0x11F] = 'S';
	map[0x120] = 'D';
	map[0x121] = 'F';
	map[0x122] = 'G';
	map[0x123] = 'H';
	map[0x124] = 'J';
	map[0x125] = 'K';
	map[0x126] = 'L';
	map[0x127] = ':';
	map[0x128] = '\"';
	map[0x11C] = '\n'; // Enter
	map[0x12C] = 'Z';
	map[0x12D] = 'X';
	map[0x12E] = 'C';
	map[0x12F] = 'V';
	map[0x130] = 'B';
	map[0x131] = 'N';
	map[0x132] = 'M';
	map[0x133] = '<';
	map[0x134] = '>';
	map[0x135] = '?';
		
	map[0x139] = ' ';
	
	map[0x129] = '~';
	map[0x102] = '!';
	map[0x103] = '@';
	map[0x104] = '#';
	map[0x105] = '$';
	map[0x106] = '%';
	map[0x107] = '^';
	map[0x108] = '&';
	map[0x109] = '*';
	map[0x10A] = '(';
	map[0x10B] = ')';
	map[0x10C] = '_';
	map[0x10D] = '+';
	map[0x10E] = '\b';
	map[0x10F] = '\t';
	
	map[0x239] = ' ';
	
	map[0x210] = 'q';
	map[0x211] = 'w';
	map[0x212] = 'e';
	map[0x213] = 'r';
	map[0x214] = 't';
	map[0x215] = 'y';
	map[0x216] = 'u';
	map[0x217] = 'i';
	map[0x218] = 'o';
	map[0x219] = 'p';
	map[0x21A] = '[';
	map[0x21B] = ']';
	map[0x21E] = 'a';
	map[0x21F] = 's';
	map[0x220] = 'd';
	map[0x221] = 'f';
	map[0x222] = 'g';
	map[0x223] = 'h';
	map[0x224] = 'j';
	map[0x225] = 'l';
	map[0x226] = 'l';
	map[0x227] = ';';
	map[0x228] = '\'';
	map[0x21C] = '\n'; // Enter
	map[0x22C] = 'z';
	map[0x22D] = 'x';
	map[0x22E] = 'c';
	map[0x22F] = 'v';
	map[0x230] = 'b';
	map[0x231] = 'n';
	map[0x232] = 'm';
	map[0x233] = ',';
	map[0x234] = '.';
	map[0x235] = '/';
}

void setupDeDeKeymap(std::map<int, int>& map)
{
	map[0x10] = 'q';
	map[0x11] = 'w';
	map[0x12] = 'e';
	map[0x13] = 'r';
	map[0x14] = 't';
	map[0x15] = 'z';
	map[0x16] = 'u';
	map[0x17] = 'i';
	map[0x18] = 'o';
	map[0x19] = 'p';
	map[0x1A] = 'ü';
	map[0x1B] = '+';
	map[0x1E] = 'a';
	map[0x1F] = 's';
	map[0x20] = 'd';
	map[0x21] = 'f';
	map[0x22] = 'g';
	map[0x23] = 'h';
	map[0x24] = 'j';
	map[0x25] = 'k';
	map[0x26] = 'l';
	map[0x27] = 'ö';
	map[0x28] = 'ä';
	map[0x1C] = '\n'; // Enter
	map[0x2C] = 'y';
	map[0x2D] = 'x';
	map[0x2E] = 'c';
	map[0x2F] = 'v';
	map[0x30] = 'b';
	map[0x31] = 'n';
	map[0x32] = 'm';
	map[0x33] = ',';
	map[0x34] = '.';
	map[0x35] = '-';

	map[0x39] = ' ';
	
	map[0x29] = '^';
	map[0x02] = '1';
	map[0x03] = '2';
	map[0x04] = '3';
	map[0x05] = '4';
	map[0x06] = '5';
	map[0x07] = '6';
	map[0x08] = '7';
	map[0x09] = '8';
	map[0x0A] = '9';
	map[0x0B] = '0';
	map[0x0C] = 'ß';
	map[0x0D] = '´';
	map[0x0E] = '\b';
	map[0x0F] = '\t';

	
	map[0x110] = 'Q';
	map[0x111] = 'W';
	map[0x112] = 'E';
	map[0x113] = 'R';
	map[0x114] = 'T';
	map[0x115] = 'Z';
	map[0x116] = 'U';
	map[0x117] = 'I';
	map[0x118] = 'O';
	map[0x119] = 'P';
	map[0x11A] = 'Ü';
	map[0x11B] = '*';
	map[0x11E] = 'A';
	map[0x11F] = 'S';
	map[0x120] = 'D';
	map[0x121] = 'F';
	map[0x122] = 'G';
	map[0x123] = 'H';
	map[0x124] = 'J';
	map[0x125] = 'K';
	map[0x126] = 'L';
	map[0x127] = 'Ö';
	map[0x128] = 'Ä';
	map[0x11C] = '\n'; // Enter
	map[0x12C] = 'Y';
	map[0x12D] = 'X';
	map[0x12E] = 'C';
	map[0x12F] = 'V';
	map[0x130] = 'B';
	map[0x131] = 'N';
	map[0x132] = 'M';
	map[0x133] = ';';
	map[0x134] = ':';
	map[0x135] = '_';
		
	map[0x139] = ' ';
	
	map[0x129] = '°';
	map[0x102] = '!';
	map[0x103] = '\"';
	map[0x104] = '§';
	map[0x105] = '$';
	map[0x106] = '%';
	map[0x107] = '&';
	map[0x108] = '/';
	map[0x109] = '(';
	map[0x10A] = ')';
	map[0x10B] = '=';
	map[0x10C] = '?';
	map[0x10D] = '`';
	map[0x10E] = '\b';
	map[0x10F] = '\t';
	
	map[0x239] = ' ';
	
	map[0x210] = 'q';
	map[0x211] = 'w';
	map[0x212] = 'e';
	map[0x213] = 'r';
	map[0x214] = 't';
	map[0x215] = 'z';
	map[0x216] = 'u';
	map[0x217] = 'i';
	map[0x218] = 'o';
	map[0x219] = 'p';
	map[0x21A] = '¨';
	map[0x21B] = '~';
	map[0x21E] = 'a';
	map[0x21F] = 's';
	map[0x220] = 'd';
	map[0x221] = 'f';
	map[0x222] = 'g';
	map[0x223] = 'h';
	map[0x224] = 'j';
	map[0x225] = 'k';
	map[0x226] = 'l';
	map[0x227] = '˝';
	map[0x228] = '^';
	map[0x21C] = '\n'; // Enter
	map[0x22C] = 'y';
	map[0x22D] = 'x';
	map[0x22E] = 'c';
	map[0x22F] = 'v';
	map[0x230] = 'b';
	map[0x231] = 'n';
	map[0x232] = 'm';
	map[0x233] = '·';
	map[0x234] = '…';
	map[0x235] = '–';
}

int main(int argc, char** argv)
{
	std::map<int, int> keymap;
	setupDeDeKeymap(keymap);

	// Load other keymap from file
	//setupKeymapFromFile(keymap, "/drives/roramdisk/keymaps/de_de.ini"); // FIXME: Read from file!
	
	if(!IO::HIDDevice::registerObserver("keyboard"))
	{
		debug_printf("[ TTY ] Could not access the keyboard!\n");
		return 1;
	}
		
	AnsiTerminal currentTTY(0, keymap);
	currentTTY.clear();
	
	message_t msg;
	while(true)
	{
		while(receive_message(&msg, MESSAGE_ANY) != MESSAGE_RECEIVED) sleep(10);
		currentTTY.handle(msg);
	}

	return 0;
}
