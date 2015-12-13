#pragma once

using namespace sf;
using namespace std;

struct Sys {
	Vector2u window_size = Vector2u(800, 600);
	string dir_path = "";
}typedef Sys;

struct Files {
	string *files = new string[0];
	unsigned int name_size = 0;
	unsigned int arr_size = 0;
	string path;
}typedef Files;

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
	Texture *p_texture = new Texture;
	Texture *n_texture = new Texture;
	bool pr = false;
	bool ne = false;
	bool error = true;
	Sprite *sprite = new Sprite;
	unsigned int num = 0;
	float size;
}typedef Picture;


void picture_mid(Vector2u window_size, Picture & picture);
