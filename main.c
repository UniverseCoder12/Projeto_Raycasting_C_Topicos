#include "raylib.h"
#include "functions.h"

int main()
{
    InitWindow(640, 640, "Raycasting");

    Vector2 player = {100, 100};
    float angle = 0;
    int playerray = 8;

    SetTargetFPS(100);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_A)) angle -= 0.05f;
        if (IsKeyDown(KEY_D)) angle += 0.05f;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(
    0,
    0,
    windowW,
    windowH/2,
    SKYBLUE,
    BLACK
    );
    DrawRectangleGradientV(
        0,
        windowH/2,
        windowW,
        windowH/2,
        BLACK,
        GRAY
    );
        PlayerMovement(&player, &angle, playerray);
        CastRay(player, angle);

        EndDrawing();
    }

    CloseWindow();
}