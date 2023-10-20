#pragma once
#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <random>
using namespace std;

struct Tile {
	bool has_mine = false;
	bool has_flag = false;
	bool is_revealed = false;
};
	

struct Board {
	int counter_num;
	void DrawBoard(string user_name);

};

sf::RectangleShape setRectangle(sf::Vector2f size, float position_x, float position_y, float origin_x, float origin_y, sf::Color color);

