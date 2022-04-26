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
#include <fstream>
#include <list>
#include<string>
#include<vector>
using namespace std;
using namespace sf;
vector<float> propertyList;

int main()
{

	string filename("properties.txt");
	float number;

	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return EXIT_FAILURE;
	}
	
	while (input_file >> number) {
		propertyList.push_back(number);
	}
	float H_ACCEL = propertyList[0];
	float H_COEFF = propertyList[1];
	float H_OPPOSITE= propertyList[2];
	float H_AIR = propertyList[3];
	float MIN_H_VEL=propertyList[4];;
	float MAX_H_VEL = propertyList[5];
	float GRAVITY = propertyList[6];
	float V_ACCEL = propertyList[7];
	float V_HOLD = propertyList[8];
	float V_SAFE = propertyList[9];
	float CUT_V_VEL = propertyList[10];
	float MAX_V_VEL = propertyList[11];
	float GAP = propertyList[12];

	ifstream file;
	file.open("stage.txt");

	float playerX = 0;
	float playerY = 0;
	file >> playerX;
	file >> playerY;
	RectangleShape playerCharacter(Vector2f(24,32));
	playerCharacter.setFillColor(Color::Cyan);
	playerCharacter.setPosition(playerX, playerY);

	float shapeCount;
	file >> shapeCount;
	
	vector<RectangleShape> walls;
	
	for (int i = 0; i < shapeCount; i++) {
		float  Wx, Wy, Wwidth, Wheight;
		file >> Wx >> Wy >> Wwidth >> Wheight;
		sf::RectangleShape tempShape(Vector2f(Wwidth,Wheight));
		tempShape.setPosition(Wx, Wy);
		tempShape.setFillColor(Color::White);
		walls.push_back(tempShape);
	}
		RenderWindow window(sf::VideoMode(800, 800), "Platforming 1");
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
			window.draw(playerCharacter);
			for (int i = 0; i < walls.size(); i++) {
				window.draw(walls[i]);
			}
			window.display();

		
		}
		input_file.close();
		return EXIT_SUCCESS;
		return 0;
	}







