#include <SFML/Graphics.hpp>
#include <string>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <array>
#include "config.h"


bool is_image(string file_name) { 
	if (!strrchr(file_name.c_str(), '.')) {
		return false;
	}
	std::array<string, 5> extArray = { "jpg","jpeg","png","gif","bmp" };
	return std::any_of(extArray.begin(), extArray.end(), [&](const std::string &ext) {
		return ext == file_name.substr(file_name.find_last_of(".") + 1);
	});
}

bool directory_exists(string &fileName) { 
	int code = GetFileAttributes(fileName.c_str());
	return (code != -1) && (FILE_ATTRIBUTE_DIRECTORY & code);
}

Files get_file_list(string const & oldPath) {
	string path = oldPath + string("*");
	Files files;
	files.path = oldPath;
	unsigned long i = 0;

	WIN32_FIND_DATA fileData;
	HANDLE firstFile = FindFirstFile(path.c_str(), &fileData);
	if (firstFile != INVALID_HANDLE_VALUE) {
		do {
			if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				files.arr_size++;
				if (strlen(fileData.cFileName) > files.name_size)
					files.name_size = strlen(fileData.cFileName);
			}

		} while (FindNextFile(firstFile, &fileData));

		files.files = new string[files.arr_size];
		i = 0;
		firstFile = FindFirstFile(path.c_str(), &fileData);
		do {
			if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				if (is_image(fileData.cFileName)) {
					files.files[i] = fileData.cFileName;
					i++;
				}
			}

		} while (FindNextFile(firstFile, &fileData));
		FindClose(firstFile);
	}
	files.arr_size = i;
	return files;
}

void texture_fun(Texture *& time_textur, Texture *textur, bool &act) {
	textur = time_textur;
	delete time_textur;
	act = false;
}

void sprite_for_window(Picture &pict, Sys &sys, Files const &files) {
	pict.sprite = new Sprite;
	pict.sprite->setPosition(0, 0);
	pict.sprite->setTexture(*(pict.texture));
	if (pict.texture->getSize().x > 1024 && pict.texture->getSize().y > 768 && pict.zoom == 1) {
		pict.zoom = float(sys.window_size.x) / pict.texture->getSize().x;
		pict.smoth_zoom = pict.zoom * 25 / 100;
	}
	cout << "zoom " << pict.zoom << "\n";
	pict.sprite->setScale(pict.zoom, pict.zoom);
	pict.sprite->setOrigin(float(pict.texture->getSize().x * 0.5), float(pict.texture->getSize().y / 2.0));
	pict.title = string(files.files[pict.num]);
}

string init_picture(RenderWindow & window, Files const &files, Picture &pic, Sys & sys) {
	if (!(pic.num < 0)) {
		if (!pic.error) {
			delete(pic.sprite);
		}
		Image *image = new Image;
		string path = files.path + files.files[pic.num];

		if (!(image->loadFromFile(files.path + files.files[pic.num]))) {
			std::cout << "error with: \n" << files.path + files.files[pic.num] << '\n';
			pic.error = true;
		}
		else {
			pic.error = false;
		}
		delete(pic.texture);
		pic.texture = new Texture;
		pic.texture->loadFromImage(*image);
		delete(image);
		sprite_for_window(pic, sys, files);
		picture_mid(window.getSize(), pic);
	}
	return pic.title;
}

void drag(Picture &picture, Vector2f & cursor, Sys sys) {
	Vector2f picture_pos = picture.sprite->getPosition();
	if (cursor.x < sys.window_size.x && cursor.x > 0 && cursor.y < sys.window_size.y && cursor.y > 0)
		picture.sprite->setPosition(cursor.x + picture.shift_x, cursor.y + picture.shift_y);
}

void picture_mid(Vector2u window_size, Picture &picture) {
	picture.left = window_size.x *0.5f;
	picture.top = window_size.y *0.5f;
	picture.sprite->setPosition(Vector2f(picture.left, picture.top));
}

void switch_prev(RenderWindow & window, Picture & picture, Files & files, Sys & sys) {
	picture.zoom = 1;
	picture.smoth_zoom = 0.25;
	if (picture.num == 0) {
		picture.num = files.arr_size;
	}
	picture.num--;
	window.setTitle(init_picture(window, files, picture, sys));
}

void switch_next(RenderWindow & window, Picture &picture, Files & files, Sys & sys) {
	picture.zoom = 1;
	picture.smoth_zoom = 0.25;
	if (picture.num + 1 == files.arr_size) {
		picture.num = 0;
	}
	else {
		picture.num++;
	}
	window.setTitle(init_picture(window, files, picture, sys));
}


void draw_buttons(RenderWindow & window, Sys & sys, Sprite & left, Sprite & right, Sprite &plus, Sprite &minus, char &activeZoom) {
	left.setPosition(Vector2f(0, float(sys.window_size.y * 0.5)));
	right.setPosition(Vector2f(float(sys.window_size.x), float(sys.window_size.y * 0.5)));
	plus.setPosition(Vector2f(float(sys.window_size.x) / 2 + 20, float(sys.window_size.y) - 15));
	minus.setPosition(Vector2f(float(sys.window_size.x) / 2 - 60, float(sys.window_size.y) - 15));
	window.draw(left);
	window.draw(right);
	if (activeZoom != 1)
		window.draw(plus);
	if (activeZoom != 2)
		window.draw(minus);
}


void draw_elements(RenderWindow & window, Sys & sys, Picture & picture, Sprite & left, Sprite & right, Sprite &plus, Sprite &minus) {
	picture.sprite->setTexture((*picture.texture));
	window.draw((*picture.sprite));
	char activeZoom = 0;
	if (picture.zoom > 5 - picture.smoth_zoom) {
		activeZoom = 1;
	}
	else if (picture.zoom <= picture.smoth_zoom) {
		activeZoom = 2;
	}
	draw_buttons(window, sys, left, right, plus, minus, activeZoom);
}

Sprite create_sprite(const char* fileName) {
	Sprite sprite;
	Texture *texture = new Texture;
	texture->loadFromFile(fileName);
	sprite.setTexture(*texture);
	return sprite;
}
void click_btn_mouse(RenderWindow &window, Files files, Picture &picture, Sprite left, Sprite right, Sprite plus, Sprite minus, Sys sys, Vector2f pos) {
	if (!picture.dragMove) {
		if (left.getGlobalBounds().contains(pos.x, pos.y)) {
			switch_prev(window, picture, files, sys);
		}
		else if (right.getGlobalBounds().contains(pos.x, pos.y)) {
			switch_next(window, picture, files, sys);
		}
		else if (plus.getGlobalBounds().contains(pos.x, pos.y) && picture.zoom < 5) {
			picture.zoom += picture.smoth_zoom;
			window.setTitle(init_picture(window, files, picture, sys));
		}
		else if (minus.getGlobalBounds().contains(pos.x, pos.y) && picture.zoom > picture.smoth_zoom) {
			picture.zoom -= picture.smoth_zoom;
			window.setTitle(init_picture(window, files, picture, sys));
		}
	}
}

void click_btn(RenderWindow &window, Files files, Picture &picture, Sprite left, Sprite right, Sprite plus, Sprite minus, Sys sys, Vector2f pos) {
	if (!picture.dragMove) {
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			switch_prev(window, picture, files, sys);
		}
		else if (Keyboard::isKeyPressed(Keyboard::Right)) {
			switch_next(window, picture, files, sys);
		}
		if (Keyboard::isKeyPressed(Keyboard::Add) && Keyboard::isKeyPressed(Keyboard::LControl) && picture.zoom < 5) {
			picture.zoom += picture.smoth_zoom;
			window.setTitle(init_picture(window, files, picture, sys));
		}
		if (Keyboard::isKeyPressed(Keyboard::Subtract) && Keyboard::isKeyPressed(Keyboard::LControl) && picture.zoom > picture.smoth_zoom) {
			picture.zoom -= picture.smoth_zoom;
			window.setTitle(init_picture(window, files, picture, sys));
		}
	}
}

void picture_check_drag(Picture &picture, Vector2f pos, Sys sys) {
	Vector2f picture_scale = picture.sprite->getScale();
	FloatRect picture_size = picture.sprite->getGlobalBounds();
	Vector2f picture_pos = picture.sprite->getPosition();
	float pictHeight = picture_size.height * picture_scale.x;
	float pictWidth = picture_size.width * picture_scale.y;
	if (pos.y < picture_pos.y + pictHeight / 2 && pos.x < picture_pos.x + pictWidth / 2 &&
		pos.y > picture_pos.y - pictHeight / 2 && pos.x > picture_pos.x - pictWidth / 2) {
		picture.dragMove = true;
		picture.st_drag_x = pos.x;
		picture.st_drag_y = pos.y;
		picture.shift_x = picture_pos.x - picture.st_drag_x;
		picture.shift_y = picture_pos.y - picture.st_drag_y;
	}
}
void start_program(RenderWindow &window, Files files, Picture &picture, Sprite left, Sprite right, Sprite plus, Sprite minus, Sys sys, View view) {
	while (window.isOpen()) {
		Event event;
		Vector2i pixelPos = Mouse::getPosition(window);
		Vector2f pos = window.mapPixelToCoords(pixelPos);
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				delete[] files.files;
				window.close();
			}
			if (picture.dragMove) {
				drag(picture, pos, sys);
			}
			if (event.type == Event::MouseButtonPressed && event.key.code == Mouse::Left) {
				click_btn_mouse(window, files, picture, left, right, plus, minus, sys, pos);
				picture_check_drag(picture, pos, sys);
			}

			if (event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left) {
				picture.dragMove = false;
			}

			click_btn(window, files, picture, left, right, plus, minus, sys, pos);

			if (event.type == Event::Resized) {
				sys.window_size = window.getSize();
				picture_mid(sys.window_size, picture);
			}

		}

		window.clear(Color(10, 10, 10));
		view = View(FloatRect(0, 0, float(sys.window_size.x), float(sys.window_size.y)));
		window.setView(view);


		draw_elements(window, sys, picture, left, right, plus, minus);
		window.display();
		sys.window_size = window.getSize();
	}
}

int main() {
	Sys sys;
	sys.dir_path = "D:/images/";
	cout << "input path (For Example: C:\\images\\)" << "\n";
	while (!directory_exists(sys.dir_path)) {
		cout << "input correct path" << "\n";
		cin >> sys.dir_path;
	}

	RenderWindow window(VideoMode(sys.window_size.x, sys.window_size.y), "Image Viewer");
	window.setVerticalSyncEnabled(true);
	View view = View(FloatRect(0, 0, float(sys.window_size.x), float(sys.window_size.y)));
	window.setView(view);
	Files files = get_file_list(sys.dir_path);
	Picture picture;
	if (files.arr_size > 0) {
		window.setTitle(init_picture(window, files, picture, sys));
	}
	Sprite left = create_sprite("sprites/left.png");
	Sprite right = create_sprite("sprites/right.png");
	Sprite plus = create_sprite("sprites/plus.png");
	Sprite minus = create_sprite("sprites/minus.png");
	left.setOrigin(Vector2f(0, left.getGlobalBounds().height));
	right.setOrigin(Vector2f(right.getGlobalBounds().width, right.getGlobalBounds().height));
	plus.setOrigin(Vector2f(0, plus.getGlobalBounds().height));
	minus.setOrigin(Vector2f(0, plus.getGlobalBounds().height));

	sys.window_size = window.getSize();
	picture_mid(sys.window_size, picture);

	start_program(window, files, picture, left, right, plus, minus, sys, view);

	return 0;
}