#include "raylib.h"
#include <stdio.h>

// Constantes para o mapa, janela, tiles, campo de visão e número de raios
#define MAP_W 20
#define MAP_H 20
#define TILE 32

// Matriz do mapa, onde 1 representa parede e 0 representa espaço vazio
int mapa[MAP_H][MAP_W] = {0};

// Função para desenhar o mapa na tela
void DrawMap()
{
    // Desenha o mapa, onde cada tile é desenhado como um quadrado
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if (mapa[y][x] == 1)
            {
                DrawRectangle(
                    x * TILE,
                    y * TILE,
                    TILE,
                    TILE,
                    DARKGRAY
                );
            }

            DrawRectangleLines(
                x * TILE,
                y * TILE,
                TILE,
                TILE,
                GRAY
            );
        }
    }
}

// Função para editar o mapa usando o mouse
void EditMap()
{
    // Pega a posição do mouse e converte para coordenadas de tilemap
    Vector2 mouse = GetMousePosition();
    int tileX = mouse.x / TILE;
    int tileY = mouse.y / TILE;

    // Verifica se o tile selecionado está dentro dos limites do mapa
    if (tileX >= 0 && tileX < MAP_W &&
        tileY >= 0 && tileY < MAP_H)
    {
        // Se o botão esquerdo do mouse estiver pressionado, adiciona uma parede no tile selecionado
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            mapa[tileY][tileX] = 1;
        }

        // Se o botão direito do mouse estiver pressionado, remove a parede do tile selecionado
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            mapa[tileY][tileX] = 0;
        }
    }
}

// Função para salvar o mapa atual no arquivo "map.txt"
void SaveMap()
{
    FILE *f = fopen("../../build/map.txt", "w");

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            fprintf(f, "%d ", mapa[y][x]);
        }

        fprintf(f, "\n");
    }

    fclose(f);
}

int main()
{
    // Inicializa a janela do editor de mapas
    InitWindow(640, 640, "Editor");

    // Define o FPS do editor para 60
    SetTargetFPS(60);

    // Loop principal do editor, que continua até a janela ser fechada
    while (!WindowShouldClose())
    {
        // Chama a função para editar o mapa usando o mouse
        EditMap();

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Chama a função para desenhar o mapa na tela
        DrawMap();

        // Se a tecla F5 for pressionada, salva o mapa atual no arquivo "map.txt"
        if (IsKeyPressed(KEY_F5)) SaveMap();

        EndDrawing();
    }
    
    // Fecha a janela do editor
    CloseWindow();
}