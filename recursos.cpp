#include "recursos.hpp"
#include <stdlib.h>

struct Celda
{
	Vector2i elem;
	struct Celda* sig;
};
typedef struct Celda* Celda;

struct ColaRep
{
	Celda frente, posterior;
};

Cola crea()
{
	Cola c = (Cola) malloc(sizeof(struct ColaRep));
	c->frente = (Celda) malloc(sizeof(struct Celda));
	c->frente->sig = NULL;
	c->posterior = c->frente;
	return c;
}

void libera(Cola c)
{
	while (c->frente != NULL)
	{
		Celda aux = c->frente;
		c->frente = aux->sig;
		free(aux);
	}
	free(c);
}

void inserta(Cola c, Vector2i e)
{
	c->posterior->sig = (Celda) malloc(sizeof(struct Celda));
	c->posterior = c->posterior->sig;
	c->posterior->elem = e;
	c->posterior->sig = NULL;
}

void suprime(Cola c)
{
	Celda eliminado = c->frente;
	c->frente = eliminado->sig;
	free(eliminado);
}

Vector2i recupera(Cola c) { return c->frente->sig->elem; }

int vacia(Cola c) { return (c->frente->sig == NULL); }