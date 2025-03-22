#pragma once
#include <raylib.h>
#include <vector>

struct Bloque 
{
	Rectangle rec;
	Vector2 vel;

	Bloque(Vector2 pos, float vel_)
	{
		rec = { pos.x, pos.y, 32, 32 };
		vel = { vel_, 0 };
	}

	void dibujar(void)
	{
		DrawRectangleRec(rec, DARKPURPLE);
	}
};

std::vector<Bloque> GenerarMapa();