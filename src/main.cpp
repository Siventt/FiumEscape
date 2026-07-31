#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <string>
#include "jugador.hpp"
#include "recursos.hpp"
#include "input_actions.hpp"

#define NUM_BATERIAS 20
#define NUM_BLOQUES 80

using namespace std;

const Color AZUL_FONDO = { 21, 17, 35, 255 };

const int U = 120;
const int ANCHO_PANTALLA = 16 * U;
const int ALTO_PANTALLA = 9 * U;
const int tam_celda = 32;
const int filas = 80;
const int columnas = ANCHO_PANTALLA / tam_celda;

const float GRAVEDAD = 12 * tam_celda;
bool debug_mode = false;
bool victoria = false;
int vic_cont = 0;
int vic_frame = 0;
bool fin = false;
int fin_cont = 0;

static void ActualizarCamara(Camera2D* camera, Jugador* jug);

// Punto de entrada del programa
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main()
{
    srand((unsigned int) time(NULL));

    InitWindow(ANCHO_PANTALLA, ALTO_PANTALLA, "FiumEscape");
    InitAudioDevice();

    HideCursor();
    // Se han quitado flags no soportados en DRM como ToggleFullscreen()
    
    Texture2D t_bloques  = LoadTexture(ASSETS_PATH"img/bloques.png");
    Texture2D t_fondo    = LoadTexture(ASSETS_PATH"img/fondo.png");
    Texture2D t_bloque   = LoadTexture(ASSETS_PATH"img/bloque.png");
    Texture2D t_bateria  = LoadTexture(ASSETS_PATH"img/bateria.png");
    Texture2D t_brillo   = LoadTexture(ASSETS_PATH"img/brillo.png");
    Texture2D t_victoria = LoadTexture(ASSETS_PATH"img/victoria.png");

    Music musica = LoadMusicStream(ASSETS_PATH"sounds/SuperGrottoEscape.wav");

    #pragma region Creacion_Escenario
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int matriz_colision[columnas][filas] = {};

    for (int i = 0; i < filas; i++)
    {
        matriz_colision[0][i] = 4;
        matriz_colision[columnas - 1][i] = 5;
    }

    matriz_colision[49][5] = 1;
    matriz_colision[50][5] = 1;
    matriz_colision[51][5] = 2;

    for (int x = 1; x < columnas-16; x++)
    {
        matriz_colision[x][7] = rand() % 2 + 1;
    }

    for (int x = 0; x < columnas; x++)
    {
        matriz_colision[x][0] = 3;
        matriz_colision[x][filas - 2] = rand()%2 + 1;
        matriz_colision[x][filas - 1] = 10; // bloque invisible
    }

    // Plataformas

    for (int x = 0; x < 5; x++)
    {
        matriz_colision[20 + x][30] = rand() % 2 + 1;
    }

    for (int x = 0; x < 5; x++)
    {
        matriz_colision[40 + x][25] = rand() % 2 + 1;
    }

    // La escalera
    for (int x = 0; x < 5; x++)
    {
        matriz_colision[10+x][60+x] = rand() % 2 + 1;
    }

    // El puente
    for (int x = 10; x < columnas - 20; x=x+2)
    {
        matriz_colision[x][40] = rand() % 2 + 1;
    }

    // Basura en el fondo
    for (int x = 5; x < columnas - 40; x++)
    {
        matriz_colision[x][filas-3] = 3;
    }
    for (int x = 7; x < columnas - 42; x++)
    {
        matriz_colision[x][filas - 4] = 3;
    }
    for (int x = 9; x < columnas - 46; x++)
    {
        matriz_colision[x][filas - 5] = 3;
    }
    #pragma endregion

    // Generar baterías y bloques rand
    Recurso baterias[NUM_BATERIAS] = {};
    baterias[0] = { {49, 4}, false };
    for (int i = 1; i < NUM_BATERIAS; i++)
    {
        baterias[i] = { { rand() % 49 + 1, i * 4}, false};
    }

    Recurso bloques[NUM_BLOQUES] = {};
    for (int i = 0; i < NUM_BLOQUES-10; i++)
    {
        bloques[i] = { { rand() % 49 + 1, i + 8}, false };
    }
    bloques[70] = { { 2, filas - 3 }, false };
    bloques[71] = { { 3, filas - 3 }, false };
    bloques[72] = { { 4, filas - 3 }, false };
    bloques[73] = { { 1, filas - 3 }, false };
    bloques[74] = { { 1, filas - 4 }, false };
    bloques[75] = { { 24, filas - 3 }, false };
    bloques[76] = { { 25, filas - 3 }, false };
    bloques[77] = { { 26, filas - 3 }, false };
    bloques[78] = { { 27, filas - 3 }, false };
    bloques[79] = { { 50, filas - 3 }, false };
    
    //~~

    Jugador jug = { {6*tam_celda, 6*tam_celda} , tam_celda};
    int num_bloques = 0;
    Color bloques_color = YELLOW;

    int bateria = 300;
    int bateria_cont = 0;
    bool linterna = true;
    Rectangle areaVision = { 0, 0, 400, 400 };
    Color bateria_color = YELLOW;
    Color bateria_indicador = GREEN;
    int frame_brillo = 0;
    int brillo_cont = 0;

    Vector2i selec_celda = {0,0};
    Vector2i selec_despl = {0,0};

    char str_buffer[100];

    Camera2D camara = {0};
    camara.target = { jug.cuerpo.x, jug.cuerpo.y };
    camara.offset = { ANCHO_PANTALLA / 2.0f, ALTO_PANTALLA / 2.0f };
    camara.rotation = 0;
    camara.zoom = 2.5f;

    bool tutorial = true;
    int tutorial_cont = -1000;
    
    float delta = 0;

    SetTargetFPS(60);

    PlayMusicStream(musica);

    // BUCLE PRINCIPAL
    while (!WindowShouldClose() and !fin)
    {
        if (IsKeyPressed(SALIR_1) and IsKeyPressed(SALIR_2))
            fin = true;

        // Control de la ventana
        if (IsKeyPressed(DEBUG))
            debug_mode = !debug_mode;

        // Actualizado
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        // Entrada
        

        // Jugador

        if (!victoria)
        {
            jug.mov_hor(matriz_colision[jug.celda.x - 1][jug.celda.y], 
                        matriz_colision[jug.celda.x + 1][jug.celda.y], delta);
            jug.mov_ver(matriz_colision[jug.celda.x][jug.celda.y + 1], 
                        matriz_colision[jug.celda.x][jug.celda.y - 1], delta);
            jug.act_celda();
            jug.revisar_pos(matriz_colision[jug.celda.x][jug.celda.y]);
            jug.act_anim();

            if (jug.celda == Vector2i{ 50, 4 })
            {
                victoria = true;
                linterna = false;
                jug.cuerpo = { 50 * tam_celda, 4 * tam_celda, 32 ,32 };
            }
        }

        delta = GetFrameTime();


        // Camara
        ActualizarCamara(&camara, &jug);

        selec_celda = jug.celda + selec_despl;

        if (IsKeyPressed(CURSOR_DER))
            selec_despl.x += 1;
        else if (IsKeyPressed(CURSOR_IZQ))
            selec_despl.x -= 1;
        else if (IsKeyPressed(CURSOR_ARR))
            selec_despl.y -= 1;
        else if (IsKeyPressed(CURSOR_ABJ))
            selec_despl.y += 1;

        if (IsKeyPressed(COLOCAR_BLOQUE) and matriz_colision[selec_celda.x][selec_celda.y] == 0
            and num_bloques > 0)
        {
            matriz_colision[selec_celda.x][selec_celda.y] = 3;
            num_bloques--;
        }

        // Recursos
        for (int i = 0; i < NUM_BATERIAS; i++)
        {
            Recurso* b = &baterias[i];
            if (b->pos == jug.celda and !b->consumido)
            {
                b->consumido = true;
                bateria += 100;
            }
        }

        for (int i = 0; i < NUM_BLOQUES; i++)
        {
            Recurso* b = &bloques[i];
            if (b->pos == jug.celda and !b->consumido)
            {
                b->consumido = true;
                num_bloques++;
            }
        }

        if (num_bloques == 0)
            bloques_color = RED;
        else
            bloques_color = YELLOW;

        if (linterna)
        {
            bateria_cont++;
            if (bateria_cont == 10)
            {
                if (bateria > 0)
                    bateria--;
                bateria_cont = 0;
            }

            if (bateria > 300)
                bateria_indicador = YELLOW;
            else
                bateria_indicador = WHITE;
        }
        if (bateria > 0 and IsKeyPressed(LINTERNA) and !victoria)
        {
            linterna = !linterna;
        }
        if (bateria == 0)
        {
            bateria_color = RED;
            linterna = false;
        }
        else
            bateria_color = YELLOW;
        
        areaVision.x = GetWorldToScreen2D({ jug.cuerpo.x, jug.cuerpo.y }, camara).x - 160;
        areaVision.y = GetWorldToScreen2D({ jug.cuerpo.x, jug.cuerpo.y }, camara).y - 200;

        brillo_cont++;
        if (brillo_cont == 14)
        {
            frame_brillo++;
            if (frame_brillo > 3) frame_brillo = 0;
            brillo_cont = 0;
        }

        // Audio
        UpdateMusicStream(musica);

        // Dibujado
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode2D(camara);

        // Fondo
        // Rejilla / Cuadricula
        for (int i = 0; i <= filas; i++)
        {
            // lineas horizontales
            DrawLine(0, i*tam_celda, ANCHO_PANTALLA, i*tam_celda, DARKGRAY);

            // lineas verticales
            if (i <= columnas)
                DrawLine(i * tam_celda, 0, i * tam_celda, filas*tam_celda, DARKGRAY);
        }

        if (!linterna)
        {
            for (int i = 0; i < NUM_BATERIAS; i++)
            {
                Recurso* b = &baterias[i];
                if (!b->consumido)
                    DrawTextureRec(t_brillo, {frame_brillo * 16.0f, 0, 16, 16}, { (float) b->pos.x * tam_celda + 8, (float) b->pos.y * tam_celda + 8 }, WHITE);
            }
        }

        if(!linterna)
            BeginScissorMode((int)areaVision.x, (int)areaVision.y, (int)areaVision.width, (int)areaVision.height);

        // Fondo 
        DrawRectangle(-1920, -1080, GetScreenWidth()*tam_celda, GetScreenHeight()*tam_celda, AZUL_FONDO);
        DrawTextureRec(t_fondo, { 64, 0, 32, 64 }, { (float)50 * tam_celda, (float)3 * tam_celda }, WHITE);
        DrawTextureRec(t_fondo, { 32, 0, 32, 32 }, { (float)49 * tam_celda, (float)3 * tam_celda }, WHITE);
        DrawTextureRec(t_fondo, { 32, 0, 32, 32 }, { (float)51 * tam_celda, (float)3 * tam_celda }, WHITE);
        DrawTextureRec(t_fondo, { 32, 0, 32, 32 }, { (float)50 * tam_celda, (float)(filas - 6) * tam_celda }, WHITE);

        // Pasada para los recursos

        for (int i = 0; i < NUM_BATERIAS; i++)
        {
            Recurso* b = &baterias[i];
            if(!b->consumido)
                DrawTexture(t_bateria, b->pos.x * tam_celda + 8, b->pos.y * tam_celda + 8, WHITE);
        }

        for (int i = 0; i < NUM_BLOQUES; i++)
        {
            Recurso* b = &bloques[i];
            if (!b->consumido)
                DrawTexture(t_bloque, b->pos.x * tam_celda + 8, b->pos.y * tam_celda + 8, WHITE);
        }
        
        // Bloques
        for (int y = 0; y < filas; y++)
        {
            for (int x = 0; x < columnas; x++)
            {
                switch (matriz_colision[x][y])
                {
                case 1: // bloque 1
                    DrawTextureRec(t_bloques, { 0, 0, 32, 32 }, {(float)x*tam_celda, (float)y * tam_celda}, WHITE);
                    break;
                case 2: // bloque 2
                    DrawTextureRec(t_bloques, { 32, 0, 32, 32 }, { (float)x * tam_celda, (float)y * tam_celda }, WHITE);
                    break;
                case 3: // bloque 3
                    DrawTextureRec(t_bloques, { 32, 32, 32, 32 }, { (float)x * tam_celda, (float)y * tam_celda }, WHITE);
                    break;
                case 4: // pared izq
                    DrawTextureRec(t_bloques, { 0, 32, -32, 32 }, { (float)x * tam_celda, (float)y * tam_celda }, WHITE);
                    break;
                case 5: // pared der
                    DrawTextureRec(t_bloques, { 0, 32, 32, 32 }, { (float)x * tam_celda, (float)y * tam_celda }, WHITE);
                    break;
                default:
                    break;
                }

            }
        }

        // Jugador
        if (!victoria)
        {
            jug.dibujar();
        }
        else
        {
            if (fin_cont < 150)
            {
                DrawTextureRec(t_victoria, {(float)vic_frame*tam_celda,0,32,32 }, { 50 * tam_celda, 4 * tam_celda }, WHITE);
                if (vic_cont++ == 10)
                {
                    vic_frame = (vic_frame + 1) % 8;
                    vic_cont = 0;
                }
            }
            fin_cont++;
        }

        if (debug_mode)
        {
            jug.dibujar_colision();
        }

        if (!linterna)
            EndScissorMode();
        
        if(num_bloques > 0)
            DrawRectangleLines(selec_celda.x * tam_celda, selec_celda.y * tam_celda, tam_celda, tam_celda, GREEN);

        //fin 2D
        EndMode2D();

        // Interfaz
        DrawTextureEx(t_bloque, { 20, 50 }, 0.0f, 2.0f, WHITE);

        DrawText(TextFormat("Bloques %d", num_bloques), 60, 50, 32, bloques_color);

        DrawTextureEx(t_bateria, { 20, 100 }, 0.0f, 2.0f, WHITE);
        DrawText("Batería", 60, 100, 32, bateria_color);
        DrawRectangleGradientH(20, 140, min(bateria, 300), 32, bateria_indicador, YELLOW);

        if(!linterna)
            DrawRectangleLinesEx(areaVision, 1, WHITE);

        if (tutorial)
        {
            if (tutorial_cont > -900)
            {
                DrawRectangle(600, 50, 1200, 150, BLACK);
                DrawRectangleLinesEx({ 600, 50, 1200, 150 }, 4.0f, YELLOW);
                
                if (tutorial_cont < -700)
                {
                    DrawText("<A> Izquierda, <D> Derecha, <SPACE> Saltar", 620, 70, 32, YELLOW);
                }

                if (tutorial_cont > -400 and tutorial_cont < -180)
                {
                    DrawText("Emm... y podrias apagar la linterna con <F>", 620, 70, 32, YELLOW);
                }
                if (tutorial_cont > -300 and tutorial_cont < -180)
                {
                    DrawText("La bateria se gasta...", 620, 110, 32, YELLOW);
                }

                if (tutorial_cont > -150)
                {
                    DrawText("Encuentra la salida. Suerte!!", 620, 70, 32, YELLOW);
                }
            }

            if (tutorial_cont++ > 0)
            {
                tutorial = false;
            }
        }

        if (tutorial_cont < 200)
        {
            DrawRectangle(600, 50, 1200, 150, BLACK);
            DrawRectangleLinesEx({ 600, 50, 1200, 150 }, 4.0f, YELLOW);
            DrawText("<CLICK IZQUIERDO> Poner Bloques", 620, 70, 32, YELLOW);
            DrawText("Cuidado, son limitados y no se quitan.", 620, 110, 32, YELLOW);
            tutorial_cont++;
        }

        if (victoria)
        {
            DrawRectangle(600, 50, 1200, 150, BLACK);
            DrawRectangleLinesEx({ 600, 50, 1200, 150 }, 4.0f, YELLOW);
            DrawText("¡Enhorabuena!", 1000, 75, 48, YELLOW);

            if (fin_cont > 200)
                fin = true;
        }

        if (debug_mode)
        {
            DrawText(TextFormat("Filas: %d Columnas: %d", filas, columnas), 1600, 20, 26, WHITE);
            // DrawText(TextFormat("Raton (%d, %d)", (int)raton.x, (int)raton.y), 1600, 75, 26, WHITE);
            DrawText(TextFormat("Celda (%d, %d)", selec_celda.x, selec_celda.y), 1600, 100, 26, RED);
            DrawText(TextFormat("Jugador (%d,%d)", (int)jug.cuerpo.x, (int)jug.cuerpo.y), 1600, 150, 26, WHITE);
            DrawText(TextFormat("Celda (%d,%d)", jug.celda.x, jug.celda.y), 1600, 175, 26, RED);
            jug.dibujar_estado();
        }

        EndDrawing();
        //~~
    }

    UnloadTexture(t_bloques);
    UnloadTexture(t_bloque);
    UnloadTexture(t_fondo);
    UnloadTexture(t_bateria);
    UnloadTexture(t_brillo);
    UnloadTexture(t_victoria);

    UnloadMusicStream(musica);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void ActualizarCamara(Camera2D* camera, Jugador* jug)
{
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D({ (1 - bbox.x) * 0.5f * ANCHO_PANTALLA, (1 - bbox.y) * 0.5f * ALTO_PANTALLA }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D({ (1 + bbox.x) * 0.5f * ANCHO_PANTALLA, (1 + bbox.y) * 0.5f * ALTO_PANTALLA }, *camera);
    camera->offset = { (1 - bbox.x) * 0.5f * ANCHO_PANTALLA, (1 - bbox.y) * 0.5f * ALTO_PANTALLA };

    if (jug->cuerpo.x < bboxWorldMin.x) camera->target.x = jug->cuerpo.x;
    if (jug->cuerpo.y < bboxWorldMin.y) camera->target.y = jug->cuerpo.y;
    if (jug->cuerpo.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (jug->cuerpo.x - bboxWorldMax.x);
    if (jug->cuerpo.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (jug->cuerpo.y - bboxWorldMax.y);
}
