#pragma once

using namespace sf;
using namespace std;

struct Sys {
	Vector2u window_size = Vector2u(1024, 768);
	string dir_path = "";
};

struct Files {
	string *files = new string[0];
	unsigned int name_size = 0;
	unsigned int arr_size = 0;
	string path;
};

struct Picture {
	float shift_x;
	float shift_y;
	float st_drag_x, st_drag_y;
	bool dragMove = false;
	float zoom = 1.0;
	float smoth_zoom = 0.25;
	string title = "Error";
	float left;
	float top;
	Texture *texture = new Texture;
	bool checkForError = true;
	Sprite *sprite = new Sprite;
	unsigned int num = 0;
	float size;
};


void picture_mid(Vector2u window_size, Picture & picture);
