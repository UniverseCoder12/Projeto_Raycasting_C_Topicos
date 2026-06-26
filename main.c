#include "raylib.h"
#include "functions.h"

int main()
{
    InitWindow(windowW, windowH, "Raycaster");
    SetTargetFPS(60);

    Vector2 player = {100, 100};
    float angle = 0;
    int playerray = 8;

    int mapa[MAP_H][MAP_W];

    //=========================
    // CARREGA TEXTURAS
    //=========================

    Texture2D wallTex =
        LoadTexture("wall.png");

    Image floorImg =
        LoadImage("floortexture.png");

    Image ceilImg =
        LoadImage("ceiltexture.png");

    Texture2D floorTex =
        LoadTextureFromImage(floorImg);

    Texture2D ceilTex =
        LoadTextureFromImage(ceilImg);

    Color *floorPixels =
        (Color *)floorImg.data;

    Color *ceilPixels =
        (Color *)ceilImg.data;

    SetTextureFilter(
        wallTex,
        TEXTURE_FILTER_BILINEAR
    );

    //=========================
    // BUFFER DA TELA
    //=========================

    Image screenBuffer =
        GenImageColor(
            windowW,
            windowH,
            BLACK
        );

    Texture2D screenTex =
        LoadTextureFromImage(
            screenBuffer
        );

    //=========================
    // MAPA
    //=========================

    LoadMap(mapa);

    //=========================
    // LOOP PRINCIPAL
    //=========================

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_A))
            angle -= 0.05f;

        if (IsKeyDown(KEY_D))
            angle += 0.05f;

        PlayerMovement(
            &player,
            &angle,
            playerray,
            mapa
        );

        BeginDrawing();

        ClearBackground(BLACK);

        // TETO + CHÃO
        DrawFloorAndCeilingSuperOtimizado(
            player,
            angle,
            floorPixels,
            ceilPixels,
            floorTex,
            ceilTex,
            screenBuffer,
            screenTex
        );

        // PAREDES
        CastRay(
            player,
            angle,
            mapa,
            wallTex
        );

        EndDrawing();
    }

    //=========================
    // LIMPEZA
    //=========================

    UnloadTexture(wallTex);

    UnloadTexture(floorTex);
    UnloadTexture(ceilTex);

    UnloadTexture(screenTex);

    UnloadImage(floorImg);
    UnloadImage(ceilImg);

    UnloadImage(screenBuffer);

    CloseWindow();

    return 0;
}