#include "jugador.hpp"
//#include <stdio.h>

using namespace std;

enum Estados{IDLE, CORRIENDO, SUBIENDO, CAYENDO};
Estados estado;

Jugador::Jugador(Vector2 pos, int tam_celda)
{
	cuerpo.x = pos.x;
	cuerpo.y = pos.y;
	cuerpo.width = (float) tam_celda;
	cuerpo.height = (float) tam_celda;
	act_celda();
	vel = {};
	dir = {};
	vel_mov = (float) tam_celda * 5;
	fuerza_salto = tam_celda * 7.0f;
	puede_saltar = false;

	sprite = LoadTexture("Graficos/jugador.png");
	frame = { 0, 0, 32, 32 };
	frame_actual = 0;
	frame_cont = 0;
	estado = IDLE;
}

Jugador::~Jugador()
{
	UnloadTexture(sprite);
}

void Jugador::act_celda()
{
	celda = { (int) ((cuerpo.x + cuerpo.width/2.0f) / cuerpo.width),
			  (int) ((cuerpo.y + cuerpo.width/2.0f) / cuerpo.width) };
}

void Jugador::act_anim()
{
	// ver estado
	
	if (dir.x == 0)
	{
		estado = IDLE;
		frame.y = 0;
	} else if (dir.x != 0)
	{
		estado = CORRIENDO;
		frame.y = 32;
	}
	
	if (vel.y < 0)
	{
		estado = SUBIENDO;
		frame.y = 64;
	} else if (vel.y > 0)
	{
		estado = CAYENDO;
		frame.y = 64;
	}

	if (estado == IDLE or estado == CORRIENDO)
	{
		frame_cont++;
		if (frame_cont >= 10) // 10 fps
		{
			frame_cont = 0;
			frame_actual++;

			if (frame_actual > 5)
				frame_actual = 0;
		}
	}
	if (estado == SUBIENDO)
	{
		frame_actual = 0;
	}
	if (estado == CAYENDO)
	{
		frame_actual = 1;
	}
	
	if (dir.x > 0)
	{
		frame.width = 32;
	}
	if (dir.x < 0)
	{
		frame.width = -32;
	}
	frame.x = abs(frame_actual * (int)frame.width);
	//printf("%d %d\n", (int)frame.x, (int)frame.y);
}
 
void Jugador::mov_hor(bool izq, bool der, float delta)
{
	dir.x = 0;
	if (IsKeyDown(KEY_LEFT))
		dir.x = -1;
	if (IsKeyDown(KEY_RIGHT))
		dir.x = 1;

	if (der and dir.x >= 0 and (celda.x * cuerpo.width - cuerpo.x) <= 0)
		cuerpo.x = celda.x * cuerpo.width;
	else if (izq and dir.x <= 0 and (celda.x * cuerpo.width - cuerpo.x) >= 0)
		cuerpo.x = celda.x * cuerpo.width;
	else
		vel.x = dir.x * vel_mov;
	
	if (dir.x)
		cuerpo.x += vel.x * delta;
}

void Jugador::mov_ver(bool abajo, bool arriba, float delta)
{
	// condiciones:
	// - estoy cayendo o parado?
	// - hay un bloque abajo?
	// - estoy pegado al bloque?
	if (vel.y >= 0 and abajo
		and (celda.y * cuerpo.width - cuerpo.y) <= 0)
	{
		vel.y = 0;
		cuerpo.y = celda.y * cuerpo.width;
		puede_saltar = true;
	}
	else
	{
		vel.y += GRAVEDAD * delta;
		if (vel.y > 12 * cuerpo.width)
			vel.y = 12 * cuerpo.width;
	}

	if (IsKeyDown(KEY_LEFT_ALT) and puede_saltar and !arriba)
	{
		vel.y = -fuerza_salto;
		puede_saltar = false;
	}
	
	// se puede hacer coyote jump si se quita
	puede_saltar = false;

	// si quitas el and se queda un efecto de agarrarse al techo
	// condiciones:
	// - estoy subiendo?
	// - hay un bloque arriba?
	// - estoy pegado al bloque?
	if (arriba and vel.y < 0 
		and cuerpo.y < celda.y * cuerpo.width)
	{
		vel.y = 0;
		cuerpo.y = celda.y * cuerpo.width;
	}
		
	cuerpo.y += vel.y * delta;
}

void Jugador::revisar_pos(bool ocupada)
{
	if (ocupada)
	{
		if(vel.y > 0)
			cuerpo.y -= 16;
		if(vel.y < 0)
			cuerpo.y += 16;
	}
}

void Jugador::dibujar()
{	 
	DrawTextureRec(sprite, frame, { cuerpo.x, cuerpo.y }, WHITE);

	// Dibuja toda la textura
	//DrawTexture(sprite, int(cuerpo.x), int(cuerpo.y), WHITE);
}

void Jugador::dibujar_colision()
{
	//DrawRectangleRec(cuerpo, BLUE);
	DrawRectangleLines(celda.x * (int)cuerpo.width, celda.y * (int)cuerpo.width,
		(int) cuerpo.width, (int) cuerpo.width, RED);
}

void Jugador::dibujar_estado()
{
	switch (estado)
	{
	case IDLE:
		DrawText("IDLE", 1600, 1000, 32, YELLOW);
		break;
	case CORRIENDO:
		DrawText("CORRIENDO", 1600, 1000, 32, YELLOW);
		break;
	case SUBIENDO:
		DrawText("SUBIENDO", 1600, 1000, 32, YELLOW);
		break;
	case CAYENDO:
		DrawText("CAYENDO", 1600, 1000, 32, YELLOW);
		break;
	default:
		break;
	}
}
