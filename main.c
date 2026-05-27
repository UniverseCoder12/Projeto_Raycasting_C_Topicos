#include "raylib.h"
#include "functions.h"

int main()
{
    // Inicializa a janela do jogo
    InitWindow(640, 640, "Raycasting");

    // Variáveis para a posição do jogador, ângulo de visão, raio de colisão e o mapa
    Vector2 player = {100, 100};
    float angle = 0;
    int playerray = 8;
    int mapa[MAP_H][MAP_W];

    // define o FPS do jogo para 60
    SetTargetFPS(60);

    // Carrega a textura da parede
    Texture2D wallTex = LoadTexture("walltexture.jpg");
    SetTextureFilter(wallTex, TEXTURE_FILTER_POINT);

    // Carrega o mapa do arquivo "map.txt" para a matriz mapa
    LoadMap(mapa);

    // Loop principal do jogo, que continua até a janela ser fechada
    while (!WindowShouldClose())
    {
        // Atualiza o ângulo de visão do jogador com base nas teclas A e D
        if (IsKeyDown(KEY_A)) angle -= 0.05f;
        if (IsKeyDown(KEY_D)) angle += 0.05f;

        BeginDrawing();
        ClearBackground(WHITE);

        // Desenha o teto e o chão usando gradientes
        DrawRectangleGradientV(
        0,
        0,
        windowW,
        windowH/2,
        (Color){184, 164, 84, 255},
        BLACK
        );

        DrawRectangleGradientV(
        0,
        windowH/2,
        windowW,
        windowH/2,
        BLACK,
        (Color){184, 164, 84, 255}
        );
        // Chama as funções principais do jogo: movimentação do jogador e raycasting para desenhar as paredes
        PlayerMovement(&player, &angle, playerray, mapa);
        CastRay(player, angle, mapa, wallTex);

        EndDrawing();
    }

    // Fecha a janela do jogo
    CloseWindow();
}