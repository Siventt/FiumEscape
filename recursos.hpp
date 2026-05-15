#pragma once

struct Vector2i
{
	int x;
	int y;

	bool operator==(Vector2i const& obj)
	{
		return x == obj.x and y == obj.y;
	}

	Vector2i operator+(Vector2i const& obj)
	{
		return {x+obj.x, y + obj.y};
	}
};

struct Recurso
{
	Vector2i pos;
	bool consumido;
};
