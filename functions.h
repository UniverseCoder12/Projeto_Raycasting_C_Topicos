#include "raylib.h"
#include <math.h>

#define MAP_W 20
#define MAP_H 20
#define windowW 640
#define windowH 640
#define TILE 64
#define FoV (PI/3)
#define NUM_RAYS 640

int mapa[MAP_H][MAP_W] = {
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
};

void CastRay(Vector2 playerPos, float angle)
{
    float maxDist = 800;
    Color rayColor = (Color){0, 0, 255, 255};

    // começo do campo de visão
    float startAngle = angle - FoV/2;

    // quanto cada raio avança no ângulo
    float angleStep = FoV / NUM_RAYS;

    for (int i = 0; i < NUM_RAYS; i++)
    {
        float rayAngle = startAngle + i * angleStep;

        float rayX = playerPos.x;
        float rayY = playerPos.y;

        float dx = cosf(rayAngle);
        float dy = sinf(rayAngle);

        float dist = 0;

        for (; dist < maxDist; dist += 0.5f)
        {
            rayX += dx * 0.5f;
            rayY += dy * 0.5f;

            int mapX = rayX / TILE;
            int mapY = rayY / TILE;

            if (mapX < 0 || mapX >= MAP_W ||
                mapY < 0 || mapY >= MAP_H)
            {
                break;
            }

            if (mapa[mapY][mapX] == 1)
            {
                break;
            }
        }

        // corrige fish-eye
        float correctedDist = dist * cosf(rayAngle - angle);

        if (correctedDist < 1)
            correctedDist = 1;

        float projDist = (windowW / 2.0f) / tanf(FoV / 2.0f);

        float wallHeight = (TILE * projDist * (64/TILE)) / correctedDist;

        // largura de cada coluna
        int columnWidth = 1;

        DrawRectangle(
            i * columnWidth,
            windowH/2 - wallHeight/2,
            columnWidth,
            wallHeight,
            (Color){255- (correctedDist * 255 / maxDist), 0, 0, 255}
        );
//        DrawLineV(playerPos, (Vector2){rayX, rayY}, YELLOW);
    }
}

bool CheckCollisionCircleMap(Vector2 pos, float radius)
{
    int left   = (pos.x - radius) / TILE;
    int right  = (pos.x + radius) / TILE;
    int top    = (pos.y - radius) / TILE;
    int bottom = (pos.y + radius) / TILE;

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

void PlayerMovement(Vector2 *playerPos, float *angle, float radius)
{
    float speed = IsKeyDown(KEY_LEFT_SHIFT) ? 1.0f : 0.5f;

    Vector2 newPos = *playerPos;

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

    // colisão
    if (!CheckCollisionCircleMap(newPos, radius))
    {
        *playerPos = newPos;
    }
}

