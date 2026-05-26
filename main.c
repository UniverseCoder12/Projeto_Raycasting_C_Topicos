#include "raylib.h"
#include "functions.h"

int main()
{
    InitWindow(640, 640, "Raycasting");

    Vector2 player = {100, 100};
    float angle = 0;
    int playerray = 8;
    int mapa[MAP_H][MAP_W];

    SetTargetFPS(100);

    LoadMap(mapa);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_A)) angle -= 0.05f;
        if (IsKeyDown(KEY_D)) angle += 0.05f;

        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangleGradientV(
        0,
        0,
        windowW,
        windowH/2,
        SKYBLUE,
        WHITE
        );
        DrawRectangleGradientV(
        0,
        windowH/2,
        windowW,
        windowH/2,
        WHITE,
        GRAY
        );
        PlayerMovement(&player, &angle, playerray);
        CastRay(player, angle, mapa);

        EndDrawing();
    }

    CloseWindow();
}