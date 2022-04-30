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

/**
 * Gets the magnitude of the specified vector
 */
float magnitude(const sf::Vector2f& v)
{
	float magnitude = sqrt(v.x * v.x + v.y * v.y);
	return magnitude;
}

/**
 * Normalizes the provided vector so that
 * the vector has a magnitude of 1.
 */
sf::Vector2f normalize(const sf::Vector2f& v)
{
	sf::Vector2f ret = v;
	float mag = magnitude(v);
	if (mag > 0.0f)
	{
		ret /= mag;
	}
	return ret;
}

bool isGrounded(RectangleShape a, RectangleShape b) {
	return false;
}

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
	for (int i = 0; i < propertyList.size(); i++) {
		cout << propertyList[i] << endl;
	}
	float H_ACCEL = propertyList[0];
	float H_COEFF = propertyList[1];
	float H_OPPOSITE = propertyList[2];
	float H_AIR = propertyList[3];
	float MIN_H_VEL = propertyList[4];;
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
	RectangleShape playerCharacter(Vector2f(24, 32));
	playerCharacter.setOrigin(playerCharacter.getSize().x / 2, playerCharacter.getSize().y / 2);
	playerCharacter.setFillColor(Color::Cyan);
	playerCharacter.setPosition(playerX, playerY);

	Vector2f playerVel(0, 0), playerAccel(0, 0);

	float shapeCount;
	file >> shapeCount;

	vector<RectangleShape> walls;
	sf::Color lGrey(211, 211, 211);
	// Setup clock for calculating delta time
	sf::Clock deltaTimeClock;
	float accumulator = 0.0f;

	for (int i = 0; i < shapeCount; i++) {
		float  Wx, Wy, Wwidth, Wheight;
		file >> Wx >> Wy >> Wwidth >> Wheight;
		sf::RectangleShape tempShape(Vector2f(Wwidth, Wheight));
		tempShape.setOrigin(tempShape.getSize().x / 2, tempShape.getSize().y / 2);
		tempShape.setPosition(Wx, Wy);
		tempShape.setFillColor(lGrey);
		walls.push_back(tempShape);
	}
	RenderWindow window(sf::VideoMode(800, 800), "Platforming 1");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);
	float TIMESTEP = 1.0f / 60;
	Vector2f forceDir(0, 0);

	float frameCounter = 0, jumpCounter = 0;

	bool keyRight = false, keyLeft = false, keyJump = false, keyStillPressed = false, jumpRel = false;
	bool goingRight = false; bool goingLeft = false;
	while (window.isOpen())
	{
		// Obtain the delta time
		sf::Time deltaTime = deltaTimeClock.restart();
		// Add the delta time to our accumulator
		accumulator += deltaTime.asSeconds();

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Key::W) {
					forceDir.y = V_ACCEL;
					keyJump = true;

				}
				if (event.key.code == Keyboard::Key::A) {
					keyLeft = true;
					keyStillPressed = true;
					forceDir.x = -15;

				}
				else if (event.key.code == Keyboard::Key::D) {
					keyRight = true;
					keyStillPressed = true;
					forceDir.x = 15;

				}
			}
			else if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Key::A) {
					keyLeft = false;
					if (!keyRight && !keyLeft) {
						forceDir.x = 0;
						keyStillPressed = false;
					}
				}
				else if (event.key.code == Keyboard::Key::D) {
					keyRight = false;
					if (!keyLeft && !keyRight) {
						forceDir.x = 0;
						keyStillPressed = false;
					}
				}
				else if (event.key.code == Keyboard::Key::W) {
					forceDir.y = 0;
					keyJump = false;
					jumpRel = true;
					landed = true;
				}
			}
		}


		while (accumulator >= TIMESTEP) {
			accumulator -= TIMESTEP;

			Vector2f currentPosition = playerCharacter.getPosition();
			Vector2f currentVelocity = playerVel;
			Vector2f currentAcceleration = playerAccel;
			if (keyJump) {
				currentAcceleration.x = forceDir.x * H_ACCEL * TIMESTEP * H_AIR;
			}
			else {
				currentAcceleration.x = forceDir.x * H_ACCEL * TIMESTEP * H_AIR;
			}
			if (currentVelocity.x > 0) {
				goingRight = true;
				goingLeft = false;
			}
			else if(currentVelocity.x < 0){
				goingRight = false;
				goingLeft = true;
			}
			if (goingLeft && currentAcceleration.x > 0) {
				currentAcceleration.x = currentAcceleration.x * H_OPPOSITE;
			}
			else if (goingRight && currentAcceleration.x < 0) {
				currentAcceleration.x = currentAcceleration.x * H_OPPOSITE;
			}

			Vector2f playerNextPos = currentPosition + currentVelocity * TIMESTEP;
			for (int i = 0; i < walls.size(); i++) {
				//collission checks
				FloatRect area;
				if (playerCharacter.getGlobalBounds().intersects(walls[i].getGlobalBounds(), area)) {
					frameCounter = 0;
					jumpCounter = 0;
					//vertical checks
					if (area.width > area.height) {
						if (area.contains(area.left, playerCharacter.getPosition().y - playerCharacter.getSize().y/2)) {
							playerNextPos.y = playerCharacter.getPosition().y + area.height + GAP;
							currentVelocity.y = 0;
							currentAcceleration.y = GRAVITY;
						}
						else {
							playerNextPos.y = playerCharacter.getPosition().y - area.height - GAP;
							currentVelocity.y = 0;
							currentAcceleration.y = 0;
						}
					}
					else if (area.width < area.height) {
						if (area.contains( playerCharacter.getPosition().x + playerCharacter.getSize().x/2 - .1f, area.top + 1.0f ))
						{
							//Right side crash
							playerNextPos.x = playerCharacter.getPosition().x - area.width - GAP;
							currentVelocity.x = 0;
							currentAcceleration.x = 0;
						}
						else
						{
							//Left side crash
							playerNextPos.x = playerCharacter.getPosition().x + area.width + GAP;
							currentVelocity.x = 0;
							currentAcceleration.x = 0;
						}
					}
				}
			}
			if (keyJump) {
				if (frameCounter < V_SAFE) {
					if (jumpCounter < V_HOLD) {
						currentAcceleration.y += V_ACCEL;
					}
					else {
						keyJump = false;
					}
				}
				jumpCounter += 1;
			}
			else {
				currentAcceleration.y += GRAVITY;
				if (jumpRel && !landed) {
					jumpRel = false;
					currentVelocity.y += CUT_V_VEL;
				}
			}
			playerVel = currentAcceleration + currentVelocity;
			if (playerVel.x > MAX_H_VEL) {
				playerVel.x = 200;
			}
			else if (playerVel.x < -MAX_H_VEL) {
				playerVel.x = -200;
			}
			if (playerVel.y > MAX_V_VEL) {
				playerVel.y = 400;
			}
			if (!keyStillPressed) {
				playerVel.x *= H_COEFF * TIMESTEP;
				if (playerVel.x < MIN_H_VEL) {
					playerVel.x = 0;
				}

			}
			playerCharacter.setPosition(playerNextPos);
			frameCounter += 1;

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







