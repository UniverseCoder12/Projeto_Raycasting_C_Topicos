#include "raylib.h"
#include <stdio.h>

#define MAP_W 20
#define MAP_H 20
#define TILE 32

int mapa[MAP_H][MAP_W] = {0};

void DrawMap()
{
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

void EditMap()
{
    Vector2 mouse = GetMousePosition();

    int tileX = mouse.x / TILE;
    int tileY = mouse.y / TILE;

    if (tileX >= 0 && tileX < MAP_W &&
        tileY >= 0 && tileY < MAP_H)
    {
        // botão esquerdo coloca parede
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            mapa[tileY][tileX] = 1;
        }

        // botão direito remove
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            mapa[tileY][tileX] = 0;
        }
    }
}

void SaveMap()
{
    FILE *f = fopen("map.txt", "w");

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
    InitWindow(640, 640, "Editor");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        EditMap();

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawMap();

        if (IsKeyPressed(KEY_F5)) SaveMap();

        EndDrawing();
    }



    CloseWindow();
}