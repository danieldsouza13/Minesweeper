#include "classes.h"

string elapsed_string;

sf::RectangleShape setRectangle(sf::Vector2f size, float position_x, float position_y, float origin_x, float origin_y, sf::Color color) { // rectangle setting function
	sf::RectangleShape rect;
	rect.setSize(size);
	rect.setPosition(position_x, position_y);
	rect.setOrigin(origin_x, origin_y);
	rect.setFillColor(color);
	return rect;
}

void RevealAdjacentTiles(vector<vector<int>> board, vector<vector<Tile>>& tiles, int row, int col, int numRows, int numCols) { // adjacent tile recursive function
	if (board[row][col] != 0) {
		return;
	}
	vector<int> check_adjacent_tiles = { row - 1, col - 1, row, col - 1, row + 1, col - 1,row - 1, col, row + 1, col,row - 1, col + 1, row, col + 1, row + 1, col + 1 };

	tiles[row][col].is_revealed = true;

	for (int i = 0; i < check_adjacent_tiles.size(); i += 2) {
		if (0 <= check_adjacent_tiles[i] && check_adjacent_tiles[i] < numRows && 0 <= check_adjacent_tiles[i + 1] && check_adjacent_tiles[i + 1] < numCols) {
			if (board[check_adjacent_tiles[i]][check_adjacent_tiles[i + 1]] == 0 && !tiles[check_adjacent_tiles[i]][check_adjacent_tiles[i + 1]].is_revealed && !tiles[check_adjacent_tiles[i]][check_adjacent_tiles[i + 1]].has_flag) {
				RevealAdjacentTiles(board, tiles, check_adjacent_tiles[i], check_adjacent_tiles[i + 1], numRows, numCols);
			}
			if (board[check_adjacent_tiles[i]][check_adjacent_tiles[i + 1]] != -1 && !tiles[check_adjacent_tiles[i]][check_adjacent_tiles[i + 1]].has_flag) {
				tiles[check_adjacent_tiles[i]][check_adjacent_tiles[i + 1]].is_revealed = true;
			}
		}
	}
} 

bool DisplayLeaderboard(int rows, int cols, string game_time, bool game_won) { // leaderboard function
	float height = (rows * 16) + 50;
	float width = 16 * cols;
	sf::RenderWindow leaderboard_window(sf::VideoMode(width, height), "Minesweeper");

	ifstream infile;
	infile.open("files/leaderboard.txt");
	vector<string> user_times;
	if (infile.is_open()) {
		string line;
		while (getline(infile, line)) {
			user_times.push_back(line);
		}
		infile.close();
	}
	sort(user_times.begin(), user_times.end());

	auto location = game_time.find(",");
	string time_stamp = game_time.substr(0, location);

	if (game_won) {
		int temp_i = 0;
		for (int i = 0; i < user_times.size(); i++) {
			auto comma_index = user_times[i].find(",");
			string current_time_stamp = user_times[i].substr(0, comma_index);
			if (current_time_stamp > time_stamp) {
				temp_i = i;
				break;
			}
			else {
				temp_i = i + 1;
			}
		}
		user_times.insert(user_times.begin() + temp_i, game_time);
		user_times.pop_back();
	}

	ofstream output_file("files/leaderboard.txt");
	if (output_file.is_open()) {
		for (int i = 0; i < user_times.size(); i++) {
			output_file << user_times[i] << "\n";
		}
		output_file.close();
	}


	string sorted_time_stamps = "";
	for (int i = 0; i < 5; i++) {
		string temp;
		size_t comma_index = user_times[i].find(",");
		string time_stamp = user_times[i].substr(0, comma_index);
		string name = user_times[i].substr(comma_index + 1);
		string index = to_string(i + 1);
		temp += index + ".    " + time_stamp + "      " + name;
		sorted_time_stamps += temp + "\n\n";
	}

	sf::Font font;
	if (!font.loadFromFile("files/font.ttf")) {

	}

	sf::Text rank(sorted_time_stamps, font, 18);
	rank.setStyle(sf::Text::Bold);
	rank.setFillColor(sf::Color::White);

	sf::Text leaderboard_title("LEADERBOARD", font, 20);
	leaderboard_title.setStyle(sf::Text::Bold);
	leaderboard_title.setStyle(sf::Text::Underlined);

	sf::FloatRect rank_rect = rank.getLocalBounds();
	rank.setOrigin(rank_rect.left + rank_rect.width / 2.0f, rank_rect.top + rank_rect.height / 2.0f);
	rank.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f + 20));

	sf::FloatRect title_rect = leaderboard_title.getLocalBounds();
	leaderboard_title.setOrigin(title_rect.left + title_rect.width / 2.0f, title_rect.top + title_rect.height / 2.0f);
	leaderboard_title.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f - 120));




	while (leaderboard_window.isOpen()) {
		sf::Event leaderboard_event;
		while (leaderboard_window.pollEvent(leaderboard_event)) {
			if (leaderboard_event.type == sf::Event::Closed) {
				leaderboard_window.close();
				return false;
			}
		}
		leaderboard_window.clear(sf::Color::Blue);
		leaderboard_window.draw(rank);
		leaderboard_window.draw(leaderboard_title);
		leaderboard_window.display();
	}
	return false;
}


void Board::DrawBoard(string user_name) {
	chrono::system_clock::time_point pause_time;
	int elapsed_pause_time = 0;

	int num_columns;
	int num_rows;
	int mine_count;
	ifstream board_dimensions_file("files/board_config.cfg");
	board_dimensions_file >> num_columns;
	board_dimensions_file >> num_rows;
	board_dimensions_file >> mine_count;

	int tile_count = num_columns * num_rows;
	int num_revealed_tiles = 0;
	sf::RectangleShape game_window_rectangle = setRectangle(sf::Vector2f(num_columns * 32, (num_rows * 32) + 100), 0, 0, 0, 0, sf::Color::White); // white background rectangle

	// TEXTURES
	sf::Texture hidden_tile_texture;
	hidden_tile_texture.loadFromFile("files/images/tile_hidden.png");
	sf::Sprite hidden_tile;
	hidden_tile.setTexture(hidden_tile_texture);

	sf::Texture revealed_tile_texture;
	revealed_tile_texture.loadFromFile("files/images/tile_revealed.png");
	sf::Sprite revealed_tile;
	revealed_tile.setTexture(revealed_tile_texture);

	sf::Texture mine_texture;
	mine_texture.loadFromFile("files/images/mine.png");
	sf::Sprite mine;
	mine.setTexture(mine_texture);

	sf::Texture flag_texture;;
	flag_texture.loadFromFile("files/images/flag.png");
	sf::Sprite flag;
	flag.setTexture(flag_texture);

	sf::Texture happy_face_button_texture;
	happy_face_button_texture.loadFromFile("files/images/face_happy.png");
	sf::Sprite happy_face_button;
	happy_face_button.setTexture(happy_face_button_texture);

	sf::Texture face_win_texture;
	face_win_texture.loadFromFile("files/images/face_win.png");
	sf::Sprite face_win;
	face_win.setTexture(face_win_texture);

	sf::Texture face_lose_texture;
	face_lose_texture.loadFromFile("files/images/face_lose.png");
	sf::Sprite face_lose;
	face_lose.setTexture(face_lose_texture);

	sf::Texture debug_button_texture;
	debug_button_texture.loadFromFile("files/images/debug.png");
	sf::Sprite debug_button;
	debug_button.setTexture(debug_button_texture);

	sf::Texture pause_button_texture;
	pause_button_texture.loadFromFile("files/images/pause.png");
	sf::Sprite pause_button;
	pause_button.setTexture(pause_button_texture);

	sf::Texture play_button_texture;
	play_button_texture.loadFromFile("files/images/play.png");
	sf::Sprite play_button;
	play_button.setTexture(play_button_texture);

	sf::Texture leaderboard_texture;
	leaderboard_texture.loadFromFile("files/images/leaderboard.png");
	sf::Sprite leaderboard_button;
	leaderboard_button.setTexture(leaderboard_texture);

	sf::Texture number_1_texture;
	number_1_texture.loadFromFile("files/images/number_1.png");
	sf::Sprite number_1;
	number_1.setTexture(number_1_texture);

	sf::Texture number_2_texture;
	number_2_texture.loadFromFile("files/images/number_2.png");
	sf::Sprite number_2;
	number_2.setTexture(number_2_texture);

	sf::Texture number_3_texture;
	number_3_texture.loadFromFile("files/images/number_3.png");
	sf::Sprite number_3;
	number_3.setTexture(number_3_texture);

	sf::Texture number_4_texture;
	number_4_texture.loadFromFile("files/images/number_4.png");
	sf::Sprite number_4;
	number_4.setTexture(number_4_texture);

	sf::Texture number_5_texture;
	number_5_texture.loadFromFile("files/images/number_5.png");
	sf::Sprite number_5;
	number_5.setTexture(number_5_texture);

	sf::Texture number_6_texture;
	number_6_texture.loadFromFile("files/images/number_6.png");
	sf::Sprite number_6;
	number_6.setTexture(number_6_texture);

	sf::Texture number_7_texture;
	number_7_texture.loadFromFile("files/images/number_7.png");
	sf::Sprite number_7;
	number_7.setTexture(number_7_texture);

	sf::Texture number_8_texture;
	number_8_texture.loadFromFile("files/images/number_8.png");
	sf::Sprite number_8;
	number_8.setTexture(number_8_texture);

	// TIMER
	sf::Texture digit_texture;
	digit_texture.loadFromFile("files/images/digits.png");
	sf::Sprite timer_sprites[4];
	sf::Clock clock;

	// BOOLEANS
	bool pause_game = false;
	bool debug = false;
	bool reset_game = false;
	bool leaderboard = false;
	bool game_lose = false;
	bool game_win = false;

	const int cell_size = 32;
	sf::RenderWindow game_window(sf::VideoMode(num_columns * 32, (num_rows * 32) + 100), "Minesweeper");
	vector<vector<int>> board(num_rows, vector<int>(num_columns, 0)); // Board 2D vector
	vector<vector<Tile>> tiles(num_rows, vector<Tile>(num_columns)); // Tiles 2D vector

	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_columns; j++) {
			board[i][j] = 0; // Initialize each tile to 0
		}
	}


	srand(time(nullptr)); // seed random number generator
	int num_mines_placed = 0;
	while (num_mines_placed < mine_count) {
		int row = std::rand() % num_rows;
		int col = std::rand() % num_columns;
		if (board[row][col] != -1) { // make sure there isn't already a mine here
			board[row][col] = -1; // -1 represents a mine
			num_mines_placed++;
			tiles[row][col].has_mine = true;
		}
	}

	// count the number of adjacent mines for each tile
	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_columns; j++) {
			if (board[i][j] != -1) { // Check if the tile is not a mine
				int num_adjacent_mines = 0;

				// Check the eight neighboring tiles for mines
				for (int row_offset = -1; row_offset <= 1; row_offset++) {
					for (int col_offset = -1; col_offset <= 1; col_offset++) {
						int row_to_check = i + row_offset;
						int col_to_check = j + col_offset;

						// Make sure the neighboring tile is within the bounds of the board
						if (row_to_check >= 0 && row_to_check < num_rows && col_to_check >= 0 && col_to_check < num_columns) {
							if (board[row_to_check][col_to_check] == -1 && !(row_offset == 0 && col_offset == 0)) {
								num_adjacent_mines++;
							}
						}
					}
				}
				board[i][j] = num_adjacent_mines;
			}
		}
	}

	while (game_window.isOpen()) {
		Board new_board; // new instance of a board used when resetting the board (clicking the happy face button)
		game_window.clear();
		sf::Event event;
		while (game_window.pollEvent(event)) { // event loop
			if (event.type == sf::Event::Closed) {
				game_window.close();
				exit(0);
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) { // when left mouse button is clicked
				sf::Vector2i mousePos = sf::Mouse::getPosition(game_window);
				int row = mousePos.y / 32;
				int col = mousePos.x / 32;

				if (debug_button.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
					if (!game_lose && !game_win && !pause_game) {
						debug = !debug;
					}
				}
				if (pause_button.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
					if (!game_lose && !game_win) {
						pause_game = !pause_game;
						if (pause_game) {
							pause_time = chrono::system_clock::now();
						}
						else {
							auto unpause_time = chrono::system_clock::now();
							elapsed_pause_time += chrono::duration_cast <chrono::seconds>(unpause_time - pause_time).count();
						}
					}
				}
				if (happy_face_button.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
					reset_game = !reset_game;
				}
				if (leaderboard_button.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {

					leaderboard = !leaderboard;
					if (leaderboard) {
						pause_time = chrono::system_clock::now();
					}
					else {
						auto unpause_time = chrono::system_clock::now();
						elapsed_pause_time += chrono::duration_cast <chrono::seconds>(unpause_time - pause_time).count();
					}
				}
				if (row >= 0 && row < num_rows && col >= 0 && col < num_columns) {
					if (tiles[row][col].is_revealed == false) {
						if (!tiles[row][col].has_flag && !game_lose && !game_win && !pause_game) {
							if (tiles[row][col].has_mine && !tiles[row][col].has_flag) { // game over
								game_lose = true;
								debug = true;
							}
							else {
								tiles[row][col].is_revealed = true;
								RevealAdjacentTiles(board, tiles, row, col, num_rows, num_columns);
							}
						}
					}
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) { // when right mouse button is clicked
				sf::Vector2i mousePos = sf::Mouse::getPosition(game_window);
				int row = mousePos.y / 32;
				int col = mousePos.x / 32;
				if (row >= 0 && row < num_rows && col >= 0 && col < num_columns) {
					if (!tiles[row][col].is_revealed && !tiles[row][col].has_flag) {
						tiles[row][col].has_flag = true;
						num_mines_placed -= 1;
					}
					else if (tiles[row][col].has_flag) {
						tiles[row][col].has_flag = false;
						num_mines_placed += 1;
					}
				}
			}
		}

		if (game_lose == false && game_win == false) {
			if (pause_game == false || leaderboard == false) {
				sf::Time elapsed = clock.getElapsedTime(); // minutes and seconds
				int seconds = elapsed.asSeconds();
				seconds -= elapsed_pause_time;
				int minutes = seconds / 60;
				seconds %= 60;

				timer_sprites[0].setTexture(digit_texture); // minutes digit 1
				timer_sprites[0].setTextureRect(sf::IntRect(21 * (minutes / 10), 0, 21, 32));
				timer_sprites[0].setPosition(((num_columns) * 32) - 97, 32 * ((num_rows)+0.5f) + 16);

				timer_sprites[1].setTexture(digit_texture); // minutes digit 2
				timer_sprites[1].setTextureRect(sf::IntRect(21 * (minutes % 10), 0, 21, 32));
				timer_sprites[1].setPosition(((num_columns) * 32) - 76, 32 * ((num_rows)+0.5f) + 16);

				timer_sprites[2].setTexture(digit_texture); // seconds digit 1
				timer_sprites[2].setTextureRect(sf::IntRect(21 * (seconds / 10), 0, 21, 32));
				timer_sprites[2].setPosition(((num_columns) * 32) - 54, 32 * ((num_rows)+0.5f) + 16);

				timer_sprites[3].setTexture(digit_texture); // seconds digit 2
				timer_sprites[3].setTextureRect(sf::IntRect(21 * (seconds % 10), 0, 21, 32));
				timer_sprites[3].setPosition(((num_columns) * 32) - 33, 32 * ((num_rows)+0.5f) + 16);

				string seconds_string;
				string minutes_string;
				if (seconds < 10) {
					seconds_string = "0" + to_string(seconds);
				}
				else {
					seconds_string = to_string(seconds);
				}

				if (minutes < 10) {
					minutes_string = "0" + to_string(minutes);
				}
				else {
					minutes_string = to_string(minutes);
				}
				elapsed_string = minutes_string + ":" + seconds_string;
			}
		}
	
		sf::Sprite counter[3];
		counter[0].setTexture(digit_texture); // counter digit 1
		counter[0].setPosition(33, 32 * (num_rows + 0.5f) + 16);
		counter[1].setTexture(digit_texture); // counter digit 2
		counter[1].setPosition(33 + 21, 32 * (num_rows + 0.5f) + 16);
		counter[2].setTexture(digit_texture); // counter digit 3
		counter[2].setPosition(33 + 42, 32 * (num_rows + 0.5f) + 16);
		counter_num = num_mines_placed;

		if (counter_num < 0) {
			counter_num = abs(counter_num);
			counter[0].setTextureRect(sf::IntRect(210, 0, 21, 32));
			counter[1].setTextureRect(sf::IntRect(21 * ((counter_num / 10) % 10), 0, 21, 32));
			counter[2].setTextureRect(sf::IntRect(21 * (counter_num % 10), 0, 21, 32));
		}
		else {
			counter[0].setTextureRect(sf::IntRect(21 * (counter_num / 100), 0, 21, 32));
			counter[1].setTextureRect(sf::IntRect(21 * ((counter_num / 10) % 10), 0, 21, 32));
			counter[2].setTextureRect(sf::IntRect(21 * (counter_num % 10), 0, 21, 32));
		}

		game_window.draw(game_window_rectangle);

		happy_face_button.setPosition((((num_columns) / 2.0) * 32) - 32, 32 * ((num_rows)+0.5f)); // happy face button
		game_window.draw(happy_face_button);

		debug_button.setPosition(((num_columns) * 32) - 304, 32 * ((num_rows)+0.5f)); // debug button
		game_window.draw(debug_button);

		pause_button.setPosition(((num_columns) * 32) - 240, 32 * ((num_rows)+0.5f)); // pause button
		game_window.draw(pause_button);

		leaderboard_button.setPosition(((num_columns) * 32) - 176, 32 * ((num_rows)+0.5f)); // leaderboard button
		game_window.draw(leaderboard_button);


		for (int i = 0; i < 4; i++) {  // draws timer
			game_window.draw(timer_sprites[i]);
		}

		for (int i = 0; i < 3; i++) {  // draws counter
			game_window.draw(counter[i]);
		}

		// Draws game_board
		for (int i = 0; i < num_rows; i++) {
			for (int j = 0; j < num_columns; j++) {
				hidden_tile.setPosition(j * 32, i * 32);  // draws original board
				game_window.draw(hidden_tile);


				if (tiles[i][j].has_flag && !game_lose && !game_win) {
					flag.setPosition(j * 32, i * 32);
					game_window.draw(flag);
				}

				if (debug == true) { // debug button logic
					if (board[i][j] == -1) { // mines
						mine.setPosition(j * 32, i * 32);
						game_window.draw(mine);
					}
				}

				if (reset_game == true) { // happy face button logic
					game_window.close();
					new_board.DrawBoard(user_name);
				}

				if (tiles[i][j].is_revealed == true) {
					revealed_tile.setPosition(j * 32, i * 32);
					game_window.draw(revealed_tile);
					if (tile_count - mine_count == num_revealed_tiles) {
						game_win = true;
					}
				}

				if (game_win == true) {
					face_win.setPosition((((num_columns) / 2.0) * 32) - 32, 32 * ((num_rows)+0.5f));
					game_window.draw(face_win);
					counter_num = 0;
					if (board[i][j] == -1) {
						flag.setPosition(j * 32, i * 32);
						game_window.draw(flag);
					}
					/*leaderboard = true;*/
				}

				if (game_lose == true) {
					face_lose.setPosition((((num_columns) / 2.0) * 32) - 32, 32 * ((num_rows)+0.5f));
					game_window.draw(face_lose);
				}

				if (board[i][j] == 1 && tiles[i][j].is_revealed) {
					number_1.setPosition(j * 32, i * 32);
					game_window.draw(number_1);
				}
				if (board[i][j] == 2 && tiles[i][j].is_revealed) {
					number_2.setPosition(j * 32, i * 32);
					game_window.draw(number_2);
				}
				if (board[i][j] == 3 && tiles[i][j].is_revealed) {
					number_3.setPosition(j * 32, i * 32);
					game_window.draw(number_3);
				}
				if (board[i][j] == 4 && tiles[i][j].is_revealed) {
					number_4.setPosition(j * 32, i * 32);
					game_window.draw(number_4);
				}
				if (board[i][j] == 5 && tiles[i][j].is_revealed) {
					number_5.setPosition(j * 32, i * 32);
					game_window.draw(number_5);
				}
				if (board[i][j] == 6 && tiles[i][j].is_revealed) {
					number_6.setPosition(j * 32, i * 32);
					game_window.draw(number_6);
				}
				if (board[i][j] == 7 && tiles[i][j].is_revealed) {
					number_7.setPosition(j * 32, i * 32);
					game_window.draw(number_7);
				}
				if (board[i][j] == 8 && tiles[i][j].is_revealed) {
					number_8.setPosition(j * 32, i * 32);
					game_window.draw(number_8);
				}
				if (pause_game == true) { // pause button logic
					play_button.setPosition(((num_columns) * 32) - 240, 32 * ((num_rows)+0.5f));
					game_window.draw(play_button);
					revealed_tile.setPosition(j * 32, i * 32);
					game_window.draw(revealed_tile);
				}
				if (leaderboard == true) { // leaderboard logic
					revealed_tile.setPosition(j * 32, i * 32);
					game_window.draw(revealed_tile);
					DisplayLeaderboard(num_columns, num_columns, elapsed_string + "," + user_name, game_win);
				}
			}
		}
		num_revealed_tiles = 0; // game_win logic
		for (int i = 0; i < num_rows; i++) {
			for (int j = 0; j < num_columns; j++) {
				if (tiles[i][j].is_revealed) {
					num_revealed_tiles++;
				}
				if (tile_count - mine_count == num_revealed_tiles)
					game_win == true;
			}
		}
		game_window.display();
	}
}

