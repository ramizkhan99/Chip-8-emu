#pragma once

#include <stdint.h>
#include "ConsoleColor.h"

/* Memory Map:
 * 0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
 * 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
 * 0x200 - 0xFFF - Program ROM and work RAM
 */

class Chip8
{
public:
	Chip8();
	~Chip8();

	/* Chip - 8 Display of 2048px (64 x 32) */
	uint8_t gfx[64 * 32];

	/* It has a HEX based keypad (0x0 - 0xF) */
	uint8_t key[16];

	bool drawFlag;

	void EmulateCycle();
	bool LoadROM(const char* path);
	inline void SetKeys(uint8_t keyCode) { std::cout << (int)keyCode << " " << (int)key[keyCode] << '\n';  key[keyCode] = 1; }
	inline void UnsetKeys(uint8_t keyCode) { key[keyCode] = 0; }
	inline uint8_t GetPixel(int x, int y) { return gfx[x + (y * 64)]; }

private:
	/* 35 opcodes, 2 bytes long */
	uint16_t opcode;

	/* 4K of memory */
	uint8_t memory[4096];

	/* 15 8-bit general purpose registers (16th one is carry flag) */
	uint8_t V[16];

	/* Index Register */
	uint16_t I;

	/* Program counter */
	uint16_t pc;

	/* Timers that count at 60Hz (Count back to zero if they are set to a value greater then 0) */
	uint8_t delayTimer;
	uint8_t soundTimer;		// Beeps when it reaches 0

	/* The Stack and Stack Pointer */
	uint16_t stack[16];
	uint16_t sp;
};

