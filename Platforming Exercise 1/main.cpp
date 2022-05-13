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

#define KEY_LEFT	sf::Keyboard::A
#define KEY_RIGHT	sf::Keyboard::D
#define KEY_JUMP	sf::Keyboard::W
#define SCREEN_W	800
#define SCREEN_H	800
#define FPS			60.0f
#define PLAYER_W	24
#define PLAYER_H	32

using namespace std;
using namespace sf;
vector<float> propertyList;

float H_ACCEL; // affected by FPS
float H_COEFF;
float H_OPPOSITE;
float H_AIR;
float MIN_H_VEL;
float MAX_H_VEL; // affected by FPS
float GRAVITY; // affected by FPS
float V_ACCEL; // affected by FPS
float V_HOLD;
float V_SAFE;
float CUT_V_VEL; // affected by FPS
float MAX_V_VEL; // affected by FPS
float GAP;

float GLOBALh_accel;
float GLOBALmax_h_vel;
float GLOBALgravity;
float GLOBALv_accel;
float GLOBALcut_v_vel;
float GLOBALmax_v_vel;

sf::View camera;

/**
 * Class representing any rectangular entity.
 */
class RectEntity
{
public:
	// Underlying SFML rectangle shape
	sf::RectangleShape rect;

	/**
	 * @brief Draws this rectangular entity
	 * @param[in] window Window where this entity will be drawn
	 */
	void draw(sf::RenderWindow& window)
	{
		window.draw(rect);
	}

	/**
	 * @brief Sets the size of this rectangular entity
	 * @param[in] width Width
	 * @param[in] height Height
	 */
	void setSize(int width, int height)
	{
		rect.setSize(sf::Vector2f(width, height));

		// RectangleShape's origin is the upper-left (0, 0) by default.
		// We want the origin to be the center of the rectangle.
		// width >> 1 and height >> 1 can be interchanged with width / 2 and height / 2 respectively.
		rect.setOrigin(width >> 1, height >> 1);
	}

	/**
	 * @brief Sets the color of this entity
	 * @param color Color
	 */
	void setColor(sf::Color color)
	{
		rect.setFillColor(color);
	}

	/**
	 * @brief Sets the position of this entity.
	 * @param[in] x X-position
	 * @param[in] y Y-position
	 * @note This entity's origin is at the center, not the upper-left corner.
	 */
	void setPosition(float x, float y)
	{
		rect.setPosition(x, y);
	}

	Vector2f getPosition() {
		return rect.getPosition();
	}
	/**
	 * Checks whether this rectangle entity intersects with another rectangle entity
	 * @param[in] other Other rectnagle entity to check collision with
	 * @return True if this entity intersects with the other rect entity.
	 */
	bool aabbIntersect(RectEntity other)
	{
		// Center-radius AABB overlap check.
		// Refer to the Bounding Boxes slide set from GDEV41
		sf::RectangleShape o = other.rect;
		sf::Vector2f dist = rect.getPosition() - o.getPosition();
		sf::Vector2f check = (rect.getSize() + o.getSize()) * 0.5f;
		return fabs(dist.x) <= check.x && fabs(dist.y) <= check.y;
	}
};

/**
 * Class representing the player entity
 */
class PlayerEntity : public RectEntity
{
public:
	int canJumpNumFrames;   // number of frames remaining after leaving the ground before we can't jump anymore
	int jumpNumFrames;  // number of frames remaining until we don't apply jump acceleration anymore
	float hAccel;   // current horizontal acceleration
	float vAccel;   // current vertical acceleration
	float hVel;     // current horizontal velocity
	float vVel;     // current vertical velocity

	/**
	 * @brief Constructor
	 */
	PlayerEntity()
	{
		// assume player is grounded at the beginning, hence the initialized values
		canJumpNumFrames = V_SAFE;
		jumpNumFrames = V_HOLD;
		hAccel = vAccel = hVel = vVel = 0;
	}

	/**
	 * @brief Process horizontal movement
	 */
	void hMove()
	{

		bool hIdle = true; // flag indicating whether there is input (FALSE) or not (TRUE)
		float newAccel = 0; // new acceleration value

		if (sf::Keyboard::isKeyPressed(KEY_LEFT))
		{
			cout<< "Center:"<< camera.getCenter().x<<","<< camera.getCenter().y<<endl;
			
			hIdle = false; // there is input, so we're not idle

			newAccel = -GLOBALh_accel;
			if (hVel > 0)
			{ // input is opposite direction of current velocity
				newAccel *= H_OPPOSITE;
			}
		}
		if (sf::Keyboard::isKeyPressed(KEY_RIGHT))
		{
			hIdle = false; // there is input, so we're not idle
			cout << "Center:" << camera.getCenter().x << "," << camera.getCenter().y<<endl;
			newAccel = GLOBALh_accel;
			if (hVel < 0)
			{ // input is opposite direction of current velocity
				newAccel *= H_OPPOSITE;
			}
		}

		if (jumpNumFrames > 0)
		{ // character not grounded, so apply in-air acceleration adjustment
			newAccel *= H_AIR;
		}

		if (hIdle)
		{ // if we're idle, replace acceleration with factor of current velocity in the opposite direction
			newAccel = -hVel * H_COEFF;
		}

		// apply acceleration
		hAccel = newAccel;
		hVel += hAccel;

		if (fabs(hVel) < MIN_H_VEL)
		{ // minimum velocity for full-stop reached
			hVel = 0;
		}
		else if (hVel > GLOBALmax_h_vel)
		{ // max velocity (right) reached
			hVel = GLOBALmax_h_vel;
		}
		else if (hVel < -GLOBALmax_h_vel)
		{ // max velocity (left) reached
			hVel = -GLOBALmax_h_vel;
		}

		// move rectangle based on horizontal velocity
		rect.move(hVel, 0);
	}

	/**
	 * @brief Process vertical movement
	 */
	void vMove()
	{
		// always experiencing gravity
		// note: +y axis in SFML goes down, hence gravity is positive acceleration
		vAccel = GLOBALgravity;

		if (sf::Keyboard::isKeyPressed(KEY_JUMP))
		{ // pressed jump
			cout << "Center:" << camera.getCenter().x << "," << camera.getCenter().y<<endl;
			if (canJumpNumFrames > 0)
			{ // we can still jump even after leaving the ground
				canJumpNumFrames = 0; // can't jump anymore after jumping
				jumpNumFrames = V_HOLD; // just started jumping, so reset number of frames until we don't apply jump acceleration anymore
				vVel = 0; // reset vertical velocity upon jumping
			}
			if (jumpNumFrames > 0)
			{ // can still apply jump acceleration
				vAccel = GLOBALv_accel;
			}
		}
		else
		{ // free fall
			if (vVel < GLOBALcut_v_vel)
			{ // maximum upward velocity reached
				vVel = GLOBALcut_v_vel;
			}
		}
		canJumpNumFrames -= 1; // in case we just left ground, decrease number of frames where we can still jump
		jumpNumFrames -= 1; // in case we jumped, decrease number of frames where we apply jump acceleration
		vVel += vAccel; // apply vertical acceleration to vertical velocity
		if (vVel > GLOBALmax_v_vel)
		{ // max falling velocity reached
			vVel = GLOBALmax_v_vel;
		}

		// move rectangle based on vertical velocity
		rect.move(0, vVel);
	}

	/**
	 * @brief Process horizontal collision of this player entity with another rectangle entity
	 * @param[in] other Reference to the other rectangle entity
	 */
	void hBump(RectEntity other)
	{
		sf::RectangleShape& otherRect = other.rect;
		float otherX = otherRect.getPosition().x;
		float ourX = rect.getPosition().x;
		if (ourX > otherX)
		{	// player moving left, hit right side of other rectangle
			otherX += otherRect.getSize().x * 0.5f; // x-position of rightmost edge of the other rectangle
			ourX -= rect.getSize().x * 0.5f; // x-position of leftmost edge of the player rectangle

			// calculate penetration distance, and move the player away from the other rectangle 
			// based on the penetration distance + the gap specified in the properties file
			rect.move(otherX - ourX + GAP, 0);

			hVel = 0; // force player horizontal velocity to 0 since we hit another entity
		}
		else if (ourX < otherX)
		{	// player moving right, hit left side of other rectangle
			otherX -= otherRect.getSize().x * 0.5f;	// x-position of leftmost edge of the other rectangle	
			ourX += rect.getSize().x * 0.5f; // x-position of rightmost edge of the player rectangle

			// calculate penetration distance, and move the player away from the other rectangle 
			// based on the penetration distance + the gap specified in the properties file
			rect.move(otherX - ourX - GAP, 0);

			hVel = 0; // force player horizontal velocity to 0 since we hit another entity
		}
	}

	/**
	 * @brief Process vertical collision of this player entity with another rectangle entity
	 * @param[in] other Reference to the other rectangle entity
	 */
	void vBump(RectEntity other)
	{
		sf::RectangleShape& otherRect = other.rect;
		float otherY = otherRect.getPosition().y;
		float ourY = rect.getPosition().y;
		if (ourY > otherY)
		{	// player moving up, hit bottom side of other rectangle
			otherY += otherRect.getSize().y * 0.5f; // y-position of bottom edge of the other rectangle
			ourY -= rect.getSize().y * 0.5f; // y-position of top edge of the player rectangle

			// calculate penetration distance, and move the player away from the other rectangle 
			// based on the penetration distance + the gap specified in the properties file
			rect.move(0, otherY - ourY + GAP);

			vVel = 0; // force player vertical velocity to 0 since we hit another entity
		}
		else if (ourY < otherY)
		{	// player moving down, hit top side of other rectangle
			otherY -= (otherRect.getSize()).y * 0.5f; // y-position of top edge of the other rectangle
			ourY += (rect.getSize()).y * 0.5f; // y-position of bottom edge of the player rectangle

			// calculate penetration distance, and move the player away from the other rectangle 
			// based on the penetration distance + the gap specified in the properties file
			rect.move(0, otherY - ourY - GAP);

			vVel = 0; // force player vertical velocity to 0 since we hit another entity
			canJumpNumFrames = V_SAFE; // player is grounded, so reset number of frames until player can't jump anymore
			jumpNumFrames = V_HOLD; // player is grounded, so reset number of frames until we don't apply jump acceleration anymore
		}
	}
};

int main()
{
	
	RectEntity line;
	RectEntity line2; 
	line.setColor(Color::White);;
	line.setSize(3, 30);
	line2.setColor(Color::White);
	line2.setSize(30, 3);
	sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "Platfomer Basics");
	
	camera.setSize(800, 800);

	window.setFramerateLimit(FPS);
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

	H_ACCEL = propertyList[0];
	H_COEFF = propertyList[1];
	H_OPPOSITE = propertyList[2];
	H_AIR = propertyList[3];
	MIN_H_VEL = propertyList[4];;
	MAX_H_VEL = propertyList[5];
	GRAVITY = propertyList[6];
	V_ACCEL = propertyList[7];
	V_HOLD = propertyList[8];
	V_SAFE = propertyList[9];
	CUT_V_VEL = propertyList[10];
	MAX_V_VEL = propertyList[11];
	GAP = propertyList[12];
	float upperX= propertyList[13];
	float upperY = propertyList[14];
	float lowerX = propertyList[15];
	float lowerY = propertyList[16];

	// pre-calculate values of properties affected by FPS
	GLOBALh_accel = H_ACCEL / FPS;
	GLOBALmax_h_vel = MAX_H_VEL / FPS;
	GLOBALgravity = GRAVITY / FPS;
	GLOBALv_accel = V_ACCEL / FPS;
	GLOBALcut_v_vel = CUT_V_VEL / FPS;
	GLOBALmax_v_vel = MAX_V_VEL / FPS;

	ifstream file;
	file.open("stage.txt");

	float playerX = 0;
	float playerY = 0;
	file >> playerX;
	file >> playerY;
	RectangleShape playerCharacter(Vector2f(24, 32));
	PlayerEntity player;
	player.setSize(PLAYER_W, PLAYER_H);
	player.setColor(Color::Blue);
	player.setPosition(playerX, playerY);

	int numBlocks;
	file >> numBlocks;
	std::vector<RectEntity> blocks;
	blocks.resize(numBlocks);

	for (int i = 0; i < blocks.size(); i++)
	{
		int blockX, blockY, blockWidth, blockHeight;
		file >> blockX >> blockY >> blockWidth >> blockHeight;

		blocks[i].setSize(blockWidth, blockHeight);
		blocks[i].setColor(sf::Color(192, 192, 192));
		blocks[i].setPosition(blockX, blockY);
	}

	
	input_file.close();
	// run the program as long as the window is open
	while (window.isOpen())
	{
		
			camera.setCenter(player.getPosition());
		
		

		line.setPosition(camera.getCenter().x,camera.getCenter().y);
		line2.setPosition(camera.getCenter().x, camera.getCenter().y);
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{	// close program (ex. X button on corner clicked)
				window.close();
			}
		}
	
		// clear the window with black color
		window.clear(sf::Color::Black);

		// Check if the escape key was pressed, which closes the window
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		// perform horizontal movement on player
		player.hMove();
		// apply physics between player and blocks after horizontal movement
		for (auto& e : blocks)
		{
			if (player.aabbIntersect(e))
			{
				player.hBump(e);
			}
		}
		
		// perform vertical movement on player
		player.vMove();
		// apply physics between player and blocks after vertical movement
		for (auto& e : blocks)
		{
			if (player.aabbIntersect(e))
			{
				player.vBump(e);
			}
		}

		window.setView(camera);
		// draw everything here
		player.draw(window);
		player.draw(window);
		for (auto& e : blocks)
		{
			e.draw(window);
		}
		line.draw(window);
		line2.draw(window);
		
		
		// displays all the things drawn onto the screen
		window.display();
	}

	return EXIT_SUCCESS;
	return 0;
}







