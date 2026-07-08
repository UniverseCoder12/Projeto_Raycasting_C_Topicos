#include "raylib.h"
#include "functions.h"

int main()
{
    InitWindow(windowW, windowH, "Raycaster");
    SetTargetFPS(60);

    Vector2 player = {100, 100};
    float angle = 0;
    int playerray = 8;
    float sprint = 100.0f;
    float sprint_stamina = 100.0f;
    int mapa[MAP_H][MAP_W];
    int vol = 1;
    int shading = 255;

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
            mapa,
            &sprint,
            &sprint_stamina);

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

        if (sprint_stamina < 0) {
            if (shading >= 255) {
                vol = 0;
            }
            if (shading <= 0) {
                vol = 1;
            }
            if (vol == 0) {
                shading -= 5;
            } 
            else {
                shading += 5;
            }
        }
        else {
            if (shading < 255) {
                shading += 5;
            }
        }
        Color sprintColor = (Color){255, 255, 255, shading};
        DrawRectangleLines(windowW / 2 - 103, windowH - 32, 205, 25, WHITE);
        DrawRectangle(windowW / 2 - 101, windowH - 30, sprint*2, 20, sprintColor);
        printf("Sprinting: %.2f, Stamina: %.2f\n", sprint, sprint_stamina);


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