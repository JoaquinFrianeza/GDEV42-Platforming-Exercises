// Joaquin Carlos L. Frianeza, 182045
// James Matthew Cuartero, 
// April 30, 2022

// I/we certify that this submission complies with the DISCS Academic Integrity
// Policy.

// If I/we have discussed my/our C++ language code with anyone other than
// my/our instructor(s), my/our groupmate(s), the teaching assistant(s),
// the extent of each discussion has been clearly noted along with a proper
// citation in the comments of my/our program.

// If any C++ language code or documentation used in my/our program
// was obtained from another source, either modified or unmodified, such as a
// textbook, website, or another individual, the extent of its use has been
// clearly noted along with a proper citation in the comments of my/our program.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Audio/SoundStream.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <list>

using namespace std;
using namespace sf;


int main()
{
		
		RenderWindow window(sf::VideoMode(800, 800), "Audio 2");
		window.setFramerateLimit(60);
		window.setKeyRepeatEnabled(false);

		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}

	
			window.clear();
			window.display();

		
		}
		return 0;
	}








