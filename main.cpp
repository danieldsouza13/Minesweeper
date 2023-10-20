#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "classes.h"
using namespace std;




int main() {	
	int num_columns;
	int num_rows;
	int mine_count;
	ifstream board_dimensions_file("files/board_config.cfg");
	board_dimensions_file >> num_columns;
	board_dimensions_file >> num_rows;
	board_dimensions_file >> mine_count;

	sf::RenderWindow welcome_window(sf::VideoMode(num_columns * 32, (num_rows * 32) + 100), "Minesweeper");

	sf::RectangleShape rectangle = setRectangle(sf::Vector2f(num_columns * 32, (num_rows * 32) + 100), 0, 0, 0, 0, sf::Color::Blue);

	sf::Font font;
	font.loadFromFile("files/font.ttf");

	sf::Text welcome_text;
	welcome_text.setFont(font);
	welcome_text.setString("WELCOME TO MINESWEEPER!");
	welcome_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
	welcome_text.setOutlineColor(sf::Color::White);
	welcome_text.setCharacterSize(24);
	sf::FloatRect textRect = welcome_text.getLocalBounds();
	welcome_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	welcome_text.setPosition((num_columns * 32) / 2.0f, (num_rows * 32) / 2.0f - 150);

	sf::Text input_prompt_text;
	input_prompt_text.setFont(font);
	input_prompt_text.setString("Enter your name:");
	input_prompt_text.setStyle(sf::Text::Bold);
	input_prompt_text.setOutlineColor(sf::Color::White);
	input_prompt_text.setCharacterSize(20);
	sf::FloatRect textRect2 = input_prompt_text.getLocalBounds();
	input_prompt_text.setOrigin(textRect2.left + textRect2.width / 2.0f, textRect2.top + textRect2.height / 2.0f);
	input_prompt_text.setPosition((num_columns * 32) / 2.0f, (num_rows * 32) / 2.0f - 75);

	sf::Text input_text;
	string user_name = "";
	int user_name_count = 0;

	while (welcome_window.isOpen()) {

		sf::Event event;
		while (welcome_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				welcome_window.close();
				exit(0);
			}

			if (event.type == sf::Event::TextEntered) {
				if (event.text.unicode >= 65 && event.text.unicode <= 90 || event.text.unicode >= 97 && event.text.unicode <= 122) {
					if (user_name.size() < 10) {
						if (user_name_count == 0) {
							user_name += static_cast<char>(toupper(event.text.unicode));
							input_text.setString((user_name) + "|");
							user_name_count += 1;
						}
						else {
							user_name += static_cast<char>(tolower(event.text.unicode));
							input_text.setString((user_name)+"|");
							user_name_count += 1;
						}
					}
				}
				
				else if (event.text.unicode == 8) {
					if (user_name.size() > 0) {
						user_name.pop_back();
						input_text.setString(user_name + "|");
						user_name_count -= 1;
					}
				}

				input_text.setFont(font);
				input_text.setStyle(sf::Text::Bold);
				input_text.setOutlineColor(sf::Color::Yellow);
				input_text.setCharacterSize(18);
				sf::FloatRect textRect3 = input_text.getLocalBounds();
				input_text.setOrigin(textRect3.left + textRect3.width / 2.0f, textRect3.top + textRect3.height / 2.0f);
				input_text.setPosition((num_columns * 32) / 2.0f, (num_rows * 32) / 2.0f - 45);
			}

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Key::Enter) {
					welcome_window.close();
					break;
				}
			}



		}
		welcome_window.clear();
		welcome_window.draw(rectangle);
		welcome_window.draw(welcome_text);
		welcome_window.draw(input_prompt_text);
		welcome_window.draw(input_text);
		welcome_window.display();
	}

	Board game_board;
	game_board.DrawBoard(user_name);


	return 0;
}