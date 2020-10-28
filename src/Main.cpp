#include <iostream>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include "Chip8.h"
#include "ConsoleColor.h"

// TODO: Track keypresses and enable audio

int main()
{
	// FIXME: Fix the display issues
	Chip8 chip8{};
	if ( !chip8.LoadROM("./games/BLITZ") )
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
	window.setFramerateLimit(60);

	while ( window.isOpen() )
	{
		sf::Event event;
		while ( window.pollEvent(event) )
		{
			if ( event.type == sf::Event::Closed )
			{
				window.close();
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

	std::cin.get();
	return 0;
}