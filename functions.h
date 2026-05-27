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

//Raycasting e o desenho das paredes com textura
void CastRay(Vector2 playerPos, float angle, int mapa[MAP_H][MAP_W], Texture2D wallTex)
{
    float maxDist = 800;

    // o angulo em que o primeiro raio é lançado
    float startAngle = angle - FoV/2;

    // quantidade do angulo entre cada raio
    float angleStep = FoV / NUM_RAYS;

    // largura das colunas dependendo da resolução e da quantidade de raios
    int columnWidth = windowW / NUM_RAYS;

    if (columnWidth < 1)
        columnWidth = 1;

    //Lógica de um raio, que é repetida a quantidade de vezes definida pela variável NUM_RAYS
    for (int i = 0; i < NUM_RAYS; i++)
    {
        //angulo do raio atual
        float rayAngle = startAngle + i * angleStep;

        //Posicionamento inicial do raio, que é a posição do jogador
        float rayX = playerPos.x;
        float rayY = playerPos.y;

        //Vetor de direção do raio, calculado a partir do angulo do raio
        float dx = cosf(rayAngle);
        float dy = sinf(rayAngle);

        float dist = 0;

        //O raio avança até atingir a distância máxima ou colidir com uma parede, também calcula a distancia percorrida pelo raio
        for (; dist < maxDist; dist += 0.5f)
        {
            //Atualiza a posição do raio, avançando na direção dos vetores de direção
            rayX += dx * 0.5f;
            rayY += dy * 0.5f;

            //Calcula a posição onde o raio está atualmente no Tilemap
            int mapX = rayX / TILE;
            int mapY = rayY / TILE;

            //Verifica se o raio saiu dos limites do mapa ou colidiu com uma parede
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

        // Tira a distorção causada pelo efeito Olho de Peixe, corrigindo a distância do raio com base no angulo do raio e do jogador
        float correctedDist = dist * cosf(rayAngle - angle);

        if (correctedDist < 1)
            correctedDist = 1;

        // Calcula a distância projetada, que é a distância do plano de projeção ao jogador, e a altura da parede a ser desenhada na tela
        float projDist = (windowW / 2.0f) / tanf(FoV / 2.0f);

        //Calcula a altura da parede a ser desenhada na tela
        float wallHeight = (TILE * projDist) / correctedDist;

        // calcula o offset para pegar o pixel correto da textura, dependendo de onde o raio colidiu com a parede
        float offsetX = fmodf(rayX, TILE);
        float offsetY = fmodf(rayY, TILE);

        float texX;

        // detecta qual lado bateu usando o offset calculado
        if (offsetX < 1 || offsetX > TILE - 1)
        {
            texX = offsetY;
        }
        else
        {
            texX = offsetX;
        }

        // converte para coordenada da textura, que tem a mesma proporção da parede, e depois para coordenada do pixel da textura
        texX = (texX / TILE) * wallTex.width;

        int textureX = (int)texX;

        if (textureX < 0)
            textureX = 0;

        if (textureX >= wallTex.width)
            textureX = wallTex.width - 1;
        // Pega a coluna da textura a ser desenhada
        Rectangle source = {
            textureX,
            0,
            1,
            wallTex.height
        };
       // Define o retângulo onde a coluna da parede será desenhada na tela
        Rectangle dest = {
            i * columnWidth,
            windowH/2 - wallHeight/2,
            columnWidth + 1,
            wallHeight
        };
        
        // Variavel de sobreamento
        int shade = 255 - (correctedDist * 255 / maxDist);

        if (shade < 0)
            shade = 0;

        if (shade > 255)
            shade = 255;

        // Define a cor da textura com base na distância
        Color tint = {
            shade,
            shade,
            shade,
            255
        };

        // Desenha a coluna da parede, que só desenha a parte da textura correspondente à coluna, e com a cor de sobreamento aplicada
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