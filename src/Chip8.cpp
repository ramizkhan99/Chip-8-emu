#include <iostream>
#include <fstream>
#include <time.h>

#include "Chip8.h"

uint8_t chip8Fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

Chip8::Chip8() : gfx{}, memory{}, stack{}, V{}, key(), drawFlag(false)
{
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;

	delayTimer = 0;
	soundTimer = 0;

	for ( uint8_t i = 0; i < 80; i++ )
	{
		memory[i] = chip8Fontset[i];
	}

	srand(time(NULL));
}

Chip8::~Chip8()
{}

bool Chip8::LoadROM(const char* path)
{
	std::ifstream rom(path, std::ios::in | std::ios::binary);

	if ( !rom )
	{
		std::cerr << "Unable to open rom" << std::endl;
		return false;
	}

	rom.seekg(0, std::ios::end);
	uint16_t romSize = rom.tellg();
	rom.seekg(0);

	char* buffer = (char*)malloc(sizeof(char) * romSize);
	rom.read(buffer, romSize);

	if ( rom.fail() )
	{
		std::cerr << "Failed to read contents of ROM to buffer" << std::endl;
		perror("rom.fail(): ");
		return false;
	}

	for ( uint16_t i = 0; i < romSize; ++i )
		memory[i + 512] = (uint8_t)buffer[i];

	free(buffer);

	return true;
}

void Chip8::EmulateCycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];
	/*std::cout << "PC: " << std::hex << pc << std::endl;
	std::cout << "Opcode: " << std::hex << opcode << std::endl;*/

	switch ( opcode & 0xF000 )
	{
		case 0x0000:
			switch ( opcode & 0x000F )
			{
				case 0x0000:
					// Clear display
					memset(gfx, 0, sizeof(gfx));
					drawFlag = true;
					pc += 2;
					break;

				case 0x000E:
					// Return from a subroutine
					pc = stack[--sp];
					pc += 2;
					break;

				default:
					std::cerr << "Unknown Opcode: " << opcode << std::endl;
					exit(EXIT_FAILURE);
			}
			break;

		case 0x1000:
			// Jump to location nnn
			pc = opcode & 0x0FFF;
			break;

		case 0x2000:
			// Call subroutine at nnn
			stack[sp++] = pc;
			pc = opcode & 0x0FFF;
			break;

		case 0x3000:
			// Skip next instruction if Vx = kk
			if ( (V[(opcode & 0x0F00) >> 8]) == (opcode & 0x00FF) )
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
			break;

		case 0x4000:
			// Skip next instruction if Vx != kk
			if ( V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF) )
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
			break;

		case 0x5000:
			// 5xy0: Skip next instruction if V[x] == V[y]
			if ( (V[opcode & 0x0F00] >> 8) == (V[opcode & 0x00F0] >> 4) )
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
			break;

		case 0x6000:
			// 6xkk: Set V[x] = kk
			V[(opcode & 0x0F00) >> 8] = opcode & 0x0FF;
			pc += 2;
			break;

		case 0x7000:
			// 7xkk: Set V[x] = V[x] + kk
			V[(opcode & 0x0F00) >> 8] += 0x00FF;
			pc += 2;
			break;

		case 0x8000:
			switch ( opcode & 0x000F )
			{
				case 0x0000:
					// 8xy0: Set V[x] = V[y]
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;

				case 0x0001:
					// 8xy1: V[x] = V[x] OR V[y]
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;

				case 0x0002:
					// 8xy2: V[x] = V[x] AND V[y]
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;

				case 0x0003:
					// 8xy3: V[x] = V[x] XOR V[y]
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;

				case 0x0004:
					// 8xy4: V[x] = V[x] + V[y] and V[0xF] = carry if sum > 255 (0xFF)
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					if ( V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]) )
						V[0xF] = 1; //carry
					else
						V[0xF] = 0;
					pc += 2;
					break;

				case 0x0005:
					// 8xy5: V[x] = V[x] - V[y]
					if ( V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8] )
						V[0xF] = 0; // there is a borrow
					else
						V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;

				case 0x0006:
					// 8xy6: Set V[F] as last digit of V[x] and then right shift by 1
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
					V[(opcode & 0x0F00) >> 8] >>= 1;
					pc += 2;
					break;

				case 0x0007:
					// 8xy7: 8xy5: V[x] = V[y] - V[x]
					if ( V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4] )
						V[0xF] = 0; // there is a borrow
					else
						V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;

				case 0x000E:
					// 9xy0: Set V[F] as last digit of V[x] and then right shift by 1
					V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
					V[(opcode & 0x0F00) >> 8] <<= 1;
					pc += 2;
					break;

				default:
					std::cerr << Color(0x0C) << "Unknown Opcode: " << opcode << std::endl;
					exit(EXIT_FAILURE);
					break;
			}
			break;

		case 0xA000:
			// Annn: Set I = nnn
			I = opcode & 0x0FFF;
			pc += 2;
			break;

		case 0xB000:
			// Bnnn: Jump to location nnn + V[0]
			pc = (opcode & 0x0FFF) + V[0];
			break;

		case 0xC000:
			// Cxkk: V[x] = (random byte) AND kk
			V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
			pc += 2;
			break;

		case 0xD000:
		{
			uint8_t x = V[(opcode & 0x0F00) >> 8u];
			uint8_t y = V[(opcode & 0x00F0) >> 4u];
			uint8_t height = opcode & 0x000F;
			uint8_t pixel;

			V[0xF] = 0;
			for ( uint8_t yline = 0; yline < height; yline++ )
			{
				//std::cout << (I + yline) << std::endl;
				pixel = memory[I + yline];
				for ( int xline = 0; xline < 8; xline++ )
				{
					if ( (pixel & (0x80 >> xline)) != 0 )
					{
						if ( gfx[(x + xline + ((y + yline) * 64)) % (64 * 32)] == 1 )
							V[0xF] = 1;
						gfx[(x + xline + ((y + yline) * 64)) % (64 * 32)] ^= 1;
					}
				}
			}

			drawFlag = true;
			pc += 2;
		}
		break;

		case 0xE000:
			switch ( opcode & 0x00FF )
			{
				case 0x009E:
					// 0xEx9E: if key number V[x] is pressed, skip next instruction
					if ( key[V[(opcode & 0x0F00) >> 8]] != 0 )
						pc += 4;
					else
						pc += 2;
					break;

				case 0x00A1:
					// 0xEx9E: if key number V[x] is not pressed, skip next instruction
					if ( key[V[(opcode & 0x0F00) >> 8]] == 0 )
						pc += 4;
					else
						pc += 2;
					break;

				default:
					std::cerr << Color(0x0C) << "Unknown Opcode: " << opcode << std::endl;
					exit(EXIT_FAILURE);
					break;
			}
			break;

		case 0xF000:
			switch ( opcode & 0x00FF )
			{
				case 0x0007:
					V[(opcode & 0x0F00) >> 8] = delayTimer;
					pc += 2;
					break;

				case 0x000A:
				{
					bool keyPressed = false;
					while ( !keyPressed )
					{
						for ( uint8_t i = 0; i < 16; ++i )
						{
							if ( key[i] != 0 )
							{
								V[(opcode & 0x0F00) >> 8] = i;
								keyPressed = true;
							}
						}
					}
					pc += 2;
				}
				break;

				case 0x0015:
					delayTimer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;

				case 0x0018:
					soundTimer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;

				case 0x001E:
					I += V[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;

				case 0x0029:
					I = V[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
					break;

				case 0x0033:
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 100;
					pc += 2;
					break;

				case 0x0055:
					for ( uint8_t i = 0; i <= ((opcode & 0x0F00) >> 8); ++i )
					{
						memory[I + i] = V[i];
					}
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
					break;

				case 0x0065:
					for ( uint8_t i = 0; i < ((opcode & 0x0F00) >> 8); ++i )
					{
						V[i] = memory[I + i];
					}
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
					break;

				default:
					std::cerr << Color(0x0C) << "Unknown Opcode: " << opcode << std::endl;
					exit(EXIT_FAILURE);
			}
			break;

		default:
			std::cerr << Color(0x0C) << "Unknown Opcode: " << opcode << std::endl;
			exit(EXIT_FAILURE);
	}

	if ( delayTimer > 0 )
		--delayTimer;

	if ( soundTimer > 0 )
		--soundTimer;
}
