#include "raylib.h"
#include <math.h>
#include <stdio.h>

// Constantes para o mapa, janela, tiles, campo de visão e número de raios
#define MAP_W 20
#define MAP_H 20
#define windowW 640
#define windowH 640
#define TILE 64
#define FoV (PI/3)
#define NUM_RAYS 640

// exemplo de mapa, onde 1 representa parede e 0 representa espaço vazio
/* int mapa[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,0,0,0,0,1,0,0,0,1,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,1,1,0,0,1,0,1,1,1,1,1,0,0,1,0,0,1},
    {1,0,0,1,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1},
    {1,0,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,1,1},
    {1,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1},
    {1,0,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
}; */

//Carrega o mapa do arquivo "map.txt" para a matriz mapa
void LoadMap(int mapa[MAP_H][MAP_W])
{
    FILE *f = fopen("map.txt", "r");

    if (!f) return;

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            fscanf(f, "%d", &mapa[y][x]);
        }
    }

    fclose(f);
}

void CastRay(Vector2 playerPos, float angle, int mapa[MAP_H][MAP_W], Texture2D wallTex)
{
    float maxDist = 800;

    float dirX =
    cosf(angle);

    float dirY =
    sinf(angle);

    float planeX =
    -dirY *
    tanf(FoV / 2);

    float planeY =
    dirX *
    tanf(FoV / 2);

    int columnWidth = windowW / NUM_RAYS;

    if (columnWidth < 1)
        columnWidth = 1;

    for (int i = 0; i < NUM_RAYS; i++)
    {
        float cameraX =
            2.0f *
            i /
            NUM_RAYS
            - 1.0f;

        float rayDirX =
            dirX +
            planeX *
            cameraX;

        float rayDirY =
            dirY +
            planeY *
            cameraX;

        int mapX = (int)(playerPos.x / TILE);
        int mapY = (int)(playerPos.y / TILE);

        float deltaDistX = fabsf(1.0f / rayDirX);
        float deltaDistY = fabsf(1.0f / rayDirY);

        float sideDistX;
        float sideDistY;

        int stepX;
        int stepY;

        int hit = 0;
        int side;

        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (playerPos.x / TILE - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0f - playerPos.x / TILE) * deltaDistX;
        }

        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (playerPos.y / TILE - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0f - playerPos.y / TILE) * deltaDistY;
        }

        while (!hit)
{
    if (sideDistX < sideDistY)
    {
        sideDistX += deltaDistX;
        mapX += stepX;
        side = 0;
    }
    else
    {
        sideDistY += deltaDistY;
        mapY += stepY;
        side = 1;
    }

    // PRIMEIRO verifica limites
    if (mapX < 0 || mapX >= MAP_W ||
        mapY < 0 || mapY >= MAP_H)
    {
        break;
    }

    // DEPOIS acessa mapa
    if (mapa[mapY][mapX] == 1)
    {
        hit = 1;
    }
}

        // distância perpendicular (sem fish-eye)
        float perpWallDist;

        if (side == 0)
        {
            perpWallDist =
                (mapX - playerPos.x / TILE + (1 - stepX) / 2.0f)
                / rayDirX;
        }
        else
        {
            perpWallDist =
                (mapY - playerPos.y / TILE + (1 - stepY) / 2.0f)
                / rayDirY;
        }

        perpWallDist =
            fabsf(
                perpWallDist
                )
                *
                TILE;

        const float MIN_WALL_DIST =
            16.0f;

        if (perpWallDist < MIN_WALL_DIST)
        {
            perpWallDist =
            MIN_WALL_DIST;
        }

        // projeção
        float projDist =
            (windowW / 2.0f) / tanf(FoV / 2.0f);

        float wallHeight =
            (TILE * windowH)
            /
            perpWallDist;

            // limita altura máxima
        if (wallHeight > windowH)
        {
            wallHeight =
            windowH;
        }

        int drawStart =
            windowH/2 - wallHeight/2;

        int drawEnd =
            windowH/2 + wallHeight/2;

        if (drawStart < 0) drawStart = 0;
        if (drawEnd > windowH) drawEnd = windowH;

        // ===== TEXTURA DA PAREDE =====
// ===== TEXTURA DA PAREDE =====

float wallX;

if (side == 0)
{
    wallX =
        playerPos.y / TILE +
        (
            (
                mapX -
                playerPos.x / TILE +
                (1 - stepX) / 2.0f
            )
            /
            rayDirX
        )
        *
        rayDirY;
}
else
{
    wallX =
        playerPos.x / TILE +
        (
            (
                mapY -
                playerPos.y / TILE +
                (1 - stepY) / 2.0f
            )
            /
            rayDirY
        )
        *
        rayDirX;
}

// pega só parte decimal
wallX -= floorf(wallX);

// coluna correta
int texX =
(int)(
wallX *
wallTex.width
);

// espelhamento
if (side == 0 && rayDirX > 0)
{
    texX =
    wallTex.width -
    texX -
    1;
}

if (side == 1 && rayDirY < 0)
{
    texX =
    wallTex.width -
    texX -
    1;
}

Rectangle source = {
    (float)texX,
    0,
    1,
    (float)wallTex.height
};

Rectangle dest = {
    i * columnWidth,
    drawStart,
    columnWidth,
    drawEnd - drawStart
};

        int shade =
            255 - (perpWallDist * 255 * 2 / maxDist);

        if (shade < 0)
            shade = 0;

        if (shade > 255)
            shade = 255;

        Color tint = {
            shade,
            shade,
            shade,
            255
        };

        DrawTexturePro(
            wallTex,
            source,
            dest,
            (Vector2){0,0},
            0,
            tint
        );
    }
}

void DrawFloorAndCeilingSuperOtimizado(Vector2 playerPos,float angle,Color *floorPixels,Color *ceilPixels,Texture2D floorTex,Texture2D ceilTex,Image screenBuffer,Texture2D screenTex)
{
    ImageClearBackground(&screenBuffer, BLACK);

    float dirX = cosf(angle);
    float dirY = sinf(angle);

    float planeX =
        -dirY * tanf(FoV / 2.0f);

    float planeY =
        dirX * tanf(FoV / 2.0f);

    for (int y = 0; y < windowH; y++)
    {
        bool isFloor =
            y > windowH / 2;

        float p =
            isFloor
            ? y - windowH / 2.0f
            : windowH / 2.0f - y;

        if (p < 1)
            p = 1;

        float posZ =
            0.5f * windowH;

        float rowDistance =
            posZ / p;

        for (int x = 0; x < windowW; x++)
        {
            float cameraX =
                2.0f *
                x /
                (float)windowW
                - 1.0f;

            float rayDirX =
                dirX +
                planeX *
                cameraX;

            float rayDirY =
                dirY +
                planeY *
                cameraX;

            float worldX =
                playerPos.x / TILE +
                rowDistance *
                rayDirX;

            float worldY =
                playerPos.y / TILE +
                rowDistance *
                rayDirY;

            int tx =
                (
                    int
                    )(
                    floorTex.width *
                    (
                        worldX -
                        floorf(worldX)
                    )
                );

            int ty =
                (
                    int
                    )(
                    floorTex.height *
                    (
                        worldY -
                        floorf(worldY)
                    )
                );

            if (tx < 0)
                tx += floorTex.width;

            if (ty < 0)
                ty += floorTex.height;

            tx %= floorTex.width;
            ty %= floorTex.height;

            Color color;

            if (isFloor)
            {
                color =
                    floorPixels[
                        ty *
                        floorTex.width +
                        tx
                    ];
            }
            else
            {
                color =
                    ceilPixels[
                        ty *
                        ceilTex.width +
                        tx
                    ];
            }

            //=====================
            // SHADING POR DISTÂNCIA
            //=====================


            float shade =
                expf(
                    -rowDistance *
                    0.45f
                );

            // limite mínimo
            if (shade < 0.0f)
                shade = 0.0f;

            color.r *= shade;
            color.g *= shade;
            color.b *= shade;

            ImageDrawPixel(
                &screenBuffer,
                x,
                y,
                color
            );
        }
    }

    UpdateTexture(
        screenTex,
        screenBuffer.data
    );

    DrawTexture(
        screenTex,
        0,
        0,
        WHITE
    );
}

// Função para verificar colisão entre o jogador e as paredes do mapa
bool CheckCollisionCircleMap(Vector2 pos, float radius, int mapa[MAP_H][MAP_W])
{

    //Calcula os limites do círculo do jogador em relação ao mapa
    int left   = (pos.x - radius) / TILE;
    int right  = (pos.x + radius) / TILE;
    int top    = (pos.y - radius) / TILE;
    int bottom = (pos.y + radius) / TILE;

    // Verifica colisão com as paredes dentro dos limites calculados
    for (int y = top; y <= bottom; y++)
    {
        for (int x = left; x <= right; x++)
        {
            if (mapa[y][x] == 1)
            {
                Rectangle wall = {
                    x * TILE,
                    y * TILE,
                    TILE,
                    TILE
                };

                if (CheckCollisionCircleRec(pos, radius, wall))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//movimentação do jogador
void PlayerMovement(Vector2 *playerPos, float *angle, float radius, int mapa[MAP_H][MAP_W])
{
    // Define a velocidade de movimento, que é aumentada quando a tecla SHIFT é pressionada
    float speed = IsKeyDown(KEY_LEFT_SHIFT) ? 3.0f : 1.5f;

    Vector2 newPos = *playerPos;

    // Movimenta o jogador para frente ou para trás dependendo das teclas W e S, usando a direção do ângulo do jogador
    if (IsKeyDown(KEY_W))
    {
        newPos.x += cosf(*angle) * speed;
        newPos.y += sinf(*angle) * speed;
    }

    if (IsKeyDown(KEY_S))
    {
        newPos.x -= cosf(*angle) * speed;
        newPos.y -= sinf(*angle) * speed;
    }

    // Verifica colisão com as paredes do mapa usando a função CheckCollisionCircleMap, e só atualiza a posição do jogador se não houver colisão
    if (!CheckCollisionCircleMap(newPos, radius, mapa))
    {
        *playerPos = newPos;
    }
}