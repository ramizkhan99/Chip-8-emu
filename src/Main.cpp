#include <iostream>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include "Chip8.h"

// TODO: Track keypresses and enable audio

int main()
{
	// FIXME: Fix the display issues
	Chip8 chip8{};
	if ( !chip8.LoadROM("./games/PONG") )
	{
		std::cerr << Color(0x0C) << "Failed mate" << "\n";
		std::cin.get();
		exit(EXIT_FAILURE);
	}

	std::cout << Color(0x0A) << "ROM Loaded successfully\n";

	uint8_t pixelSize = 16;
	sf::RectangleShape graphics[64 * 32];

	for ( uint8_t y = 0; y < 32; y++ )
	{
		for ( uint8_t x = 0; x < 64; x++ )
		{
			uint16_t index = x + (y * 64);
			graphics[index] = sf::RectangleShape(sf::Vector2f(pixelSize, pixelSize));
			graphics[index].setPosition(1 + (x * pixelSize), 1 + (y * pixelSize));
		}
	}

	sf::RenderWindow window(sf::VideoMode(pixelSize * 64, pixelSize * 32), "Chip 8");
	//window.setFramerateLimit(60);

	while ( window.isOpen() )
	{
		sf::Event event;
		while ( window.pollEvent(event) )
		{
			if ( event.type == sf::Event::Closed )
			{
				window.close();
			}

			if ( event.type == sf::Event::KeyPressed )
			{
				if ( event.key.code == sf::Keyboard::Num1 )
				{
					chip8.SetKeys(1);
				}

				if ( event.key.code == sf::Keyboard::Num2 )
				{
					chip8.SetKeys(2);
				}

				if ( event.key.code == sf::Keyboard::Num3 )
				{
					chip8.SetKeys(3);
				}

				if ( event.key.code == sf::Keyboard::Num4 )
				{
					chip8.SetKeys(0xC);
				}

				if ( event.key.code == sf::Keyboard::Q )
				{
					chip8.SetKeys(4);
				}

				if ( event.key.code == sf::Keyboard::W )
				{
					chip8.SetKeys(5);
				}

				if ( event.key.code == sf::Keyboard::E )
				{
					chip8.SetKeys(6);
				}

				if ( event.key.code == sf::Keyboard::R )
				{
					chip8.SetKeys(0xD);
				}

				if ( event.key.code == sf::Keyboard::A )
				{
					chip8.SetKeys(7);
				}

				if ( event.key.code == sf::Keyboard::S )
				{
					chip8.SetKeys(8);
				}

				if ( event.key.code == sf::Keyboard::D )
				{
					chip8.SetKeys(9);
				}

				if ( event.key.code == sf::Keyboard::F )
				{
					chip8.SetKeys(0xE);
				}
				if ( event.key.code == sf::Keyboard::Z )
				{
					chip8.SetKeys(0xA);
				}

				if ( event.key.code == sf::Keyboard::X )
				{
					chip8.SetKeys(0);
				}

				if ( event.key.code == sf::Keyboard::C )
				{
					chip8.SetKeys(0xB);
				}

				if ( event.key.code == sf::Keyboard::V )
				{
					chip8.SetKeys(0xF);
				}
			}

			if ( event.type == sf::Event::KeyReleased )
			{
				if ( event.key.code == sf::Keyboard::Num1 )
				{
					chip8.UnsetKeys(1);
				}

				if ( event.key.code == sf::Keyboard::Num2 )
				{
					chip8.UnsetKeys(2);
				}

				if ( event.key.code == sf::Keyboard::Num3 )
				{
					chip8.UnsetKeys(3);
				}

				if ( event.key.code == sf::Keyboard::Num4 )
				{
					chip8.UnsetKeys(0xC);
				}

				if ( event.key.code == sf::Keyboard::Q )
				{
					chip8.UnsetKeys(4);
				}

				if ( event.key.code == sf::Keyboard::W )
				{
					chip8.UnsetKeys(5);
				}

				if ( event.key.code == sf::Keyboard::E )
				{
					chip8.UnsetKeys(6);
				}

				if ( event.key.code == sf::Keyboard::R )
				{
					chip8.UnsetKeys(0xD);
				}

				if ( event.key.code == sf::Keyboard::A )
				{
					chip8.UnsetKeys(7);
				}

				if ( event.key.code == sf::Keyboard::S )
				{
					chip8.UnsetKeys(8);
				}

				if ( event.key.code == sf::Keyboard::D )
				{
					chip8.UnsetKeys(9);
				}

				if ( event.key.code == sf::Keyboard::F )
				{
					chip8.UnsetKeys(0xE);
				}
				if ( event.key.code == sf::Keyboard::Z )
				{
					chip8.UnsetKeys(0xA);
				}

				if ( event.key.code == sf::Keyboard::X )
				{
					chip8.UnsetKeys(0);
				}

				if ( event.key.code == sf::Keyboard::C )
				{
					chip8.UnsetKeys(0xB);
				}

				if ( event.key.code == sf::Keyboard::V )
				{
					chip8.UnsetKeys(0xF);
				}
			}
		}

		chip8.EmulateCycle();

		if ( chip8.drawFlag )
		{
			for ( int y = 0; y < 32; y++ )
			{
				for ( int x = 0; x < 64; x++ )
				{
					int index = x + (y * 64);
					if ( chip8.GetPixel(x, y) == 1 )
					{
						graphics[index].setFillColor(sf::Color(255, 255, 255, 255));
					}
					else
					{
						graphics[index].setFillColor(sf::Color(0, 0, 0, 255));
					}
				}
			}

			chip8.drawFlag = false;
		}

		window.clear();
		for ( uint16_t i = 0; i < 32 * 64; i++ )
		{
			window.draw(graphics[i]);
		}
		window.display();
	}

	return 0;
}