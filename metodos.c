#include "raylib.h"
#include "metodos.h"
#include <math.h>
#include <stdio.h>
#define MAP_W 20
#define MAP_H 20
#define windowW 640
#define windowH 640
#define TILE 64
#define FoV (PI/3)
#define NUM_RAYS 640

typedef struct Enemy
{
    Vector2 pos;
    bool alive;
    
    int frame;
    int frameCount;
    int frameWidth;
    int frameHeight;
} Enemy;

void loadMap(int mapa[MAP_H][MAP_W])
{
    FILE*f=fopen("map.txt","r");

    if(!f)return;

    for(int y=0;y<MAP_H;y++)
    {
        for(int x=0;x<MAP_W;x++)
        {
            fscanf(f,"%d",&mapa[y][x]);
        }
    }

    fclose(f);
}

void drawEnemy(Vector2 playerPos,float angle,Enemy enemy,Texture2D enemyTex,float zBuffer[])
{
    if (!enemy.alive)
        return;

    float dirX = cosf(angle);
    float dirY = sinf(angle);

    float planeX = -dirY * tanf(FoV/2);
    float planeY =  dirX * tanf(FoV/2);

    float spriteX = enemy.pos.x - playerPos.x;
    float spriteY = enemy.pos.y - playerPos.y;

    float invDet = 1.0f / (planeX*dirY - dirX*planeY);

    float transformX = invDet * (dirY*spriteX - dirX*spriteY);

    float transformY = invDet * (-planeY*spriteX + planeX*spriteY);

    if (transformY <= 0.1f) return;

    int spriteScreenX = (int)(((windowW/2) * (1 + transformX/transformY)));

    float projDist = (windowW/2.0f) / tanf(FoV/2);

    int spriteHeight = fabs((int)((TILE * projDist) / transformY));

    int spriteWidth = spriteHeight/2;

    int drawStartY = windowH/2 - spriteHeight/2;

    int drawEndY = drawStartY + spriteHeight;

    int drawStartX = spriteScreenX - spriteWidth/2;

    int drawEndX = spriteScreenX + spriteWidth/2;

    static int anim = 0;
    static int frame = 0;

    if (anim == 0){
        frame += 1;
        if(frame > 11)frame = 0;
    }
    else {
        if (anim == 3){
        anim = -1;
    }
    }
    anim++;

    for (int stripe = drawStartX; stripe < drawEndX; stripe++)
    {
        if (stripe < 0 || stripe >= windowW) continue;

        int ray = stripe * NUM_RAYS / windowW;

        if (transformY >= zBuffer[ray]) continue;

        int texX = (stripe - drawStartX) * enemy.frameWidth/spriteWidth;

        Rectangle src = {frame * enemy.frameWidth + texX, 0, 1, enemy.frameHeight};

        Rectangle dst = {stripe, drawStartY, 1, spriteHeight};

        DrawTexturePro(enemyTex, src, dst, (Vector2){0,0}, 0, WHITE);
    }
}

void castRay(Vector2 playerPos,float angle,int mapa[MAP_H][MAP_W],Texture2D wallTex, float zBuffer[])
{
    float maxDist=800;

    float dirX = cosf(angle);
    float dirY = sinf(angle);

    float planeX = -dirY * tanf(FoV/2);
    float planeY = dirX * tanf(FoV/2);

    int columnWidth = windowW / NUM_RAYS;
    if(columnWidth<1) columnWidth=1;

    for(int i=0;i<NUM_RAYS;i++)
    {
        float cameraX = 2.0f * i / NUM_RAYS -1.0f;

        float rayDirX = dirX + planeX * cameraX;
        float rayDirY = dirY + planeY * cameraX;

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

        if(rayDirX<0)
        {
            stepX = -1;
            sideDistX = (playerPos.x / TILE - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0f - playerPos.x / TILE) * deltaDistX;
        }

        if(rayDirY<0)
        {
            stepY = -1;
            sideDistY = (playerPos.y / TILE - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0f - playerPos.y / TILE) * deltaDistY;
        }

        while(!hit)
        {
            if(sideDistX<sideDistY)
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

            if(mapX<0 || mapX>=MAP_W || mapY<0 || mapY>=MAP_H)
            {
                break;
            }

            if(mapa[mapY][mapX]==1)
            {
                hit = 1;
            }
        }

        float perpWallDist;

        if(side==0)
        {
            perpWallDist = (mapX - playerPos.x / TILE + (1 - stepX) / 2.0f) / rayDirX;
        }
        else
        {
            perpWallDist = (mapY - playerPos.y / TILE + (1 - stepY) / 2.0f) / rayDirY;
        }

        perpWallDist = fabsf(perpWallDist) * TILE;
        if(perpWallDist<0)
        {
            perpWallDist = 0;
        }

        float projDist = (windowW/2.0f)/tanf(FoV/2.0f);
        float wallHeight = (TILE * windowH) / perpWallDist;

        int drawStart = windowH / 2 - wallHeight / 2;
        int drawEnd = windowH / 2 + wallHeight / 2;

        float wallX;

        if(side==0)
        {
            wallX = playerPos.y / TILE + ((mapX - playerPos.x / TILE + (1 - stepX) / 2.0f) / rayDirX) * rayDirY;
        }
        else
        {
            wallX = playerPos.x / TILE + ((mapY - playerPos.y / TILE + (1 - stepY) / 2.0f) / rayDirY) * rayDirX;
        }

        wallX -= floorf(wallX);

        int texX = (int)(wallX * wallTex.width);

        if(side==0 && rayDirX>0)
        {
            texX = wallTex.width - texX - 1;
        }

        if(side==1 && rayDirY<0)
        {
            texX = wallTex.width - texX - 1;
        }

        Rectangle source = {(float)texX, 0, 1, (float)wallTex.height};

        Rectangle dest = {i*columnWidth, drawStart, columnWidth, drawEnd-drawStart};

        int shade= 255 - (perpWallDist * 255 * 4 / maxDist);

        if(shade<0) shade = 0;
        if(shade>255) shade = 255;

        Color tint = {shade, shade, shade, 255};
        
        DrawTexturePro(wallTex, source, dest, (Vector2){0,0}, 0, tint);
        zBuffer[i] = perpWallDist;
    }
}
void drawFloorAndCeiling(Vector2 playerPos,float angle,Color*floorPixels,Color*ceilPixels,Texture2D floorTex,Texture2D ceilTex,Image screenBuffer,Texture2D screenTex)
{
    ImageClearBackground(&screenBuffer,BLACK);

    float dirX = cosf(angle);
    float dirY = sinf(angle);

    float planeX = -dirY*tanf(FoV/2.0f);
    float planeY= dirX*tanf(FoV/2.0f);

    for(int y=0;y<windowH;y++) {
        bool isFloor = y > windowH/2;
        float p = isFloor ?y-windowH/2.0f: windowH/2.0f-y;
        if(p<1) p = 1;
        float posZ = 0.5f * windowH;
        float rowDistance = posZ/p;

        for(int x=0;x<windowW;x++) {
            float cameraX = 2.0f * x/(float)windowW - 1.0f;

            float rayDirX = dirX + planeX * cameraX;
            float rayDirY = dirY + planeY * cameraX;

            float worldX = playerPos.x/TILE + rowDistance * rayDirX;
            float worldY = playerPos.y/TILE + rowDistance * rayDirY;

            int tx = (int)(floorTex.width*(worldX-floorf(worldX)));
            int ty = (int)(floorTex.height*(worldY-floorf(worldY)));

            if(tx<0) tx += floorTex.width;
            if(ty<0) ty += floorTex.height;

            tx %= floorTex.width;
            ty %= floorTex.height;

            Color color;

            if(isFloor) {
                color = floorPixels[ty *floorTex.width + tx];
            }
            else {
                color = ceilPixels[ty * ceilTex.width + tx];
            }

            float shade = expf(-rowDistance*0.9f);

            if(shade<0.0f) shade=0.0f;

            color.r*=shade;
            color.g*=shade;
            color.b*=shade;
            
            ImageDrawPixel(&screenBuffer, x, y, color);
        }
    }

    UpdateTexture(screenTex,screenBuffer.data);
    DrawTexture(screenTex,0,0,WHITE);
}
bool checkCollisionCircleMap(Vector2 pos,float radius,int mapa[MAP_H][MAP_W]) {
    int left = (pos.x-radius)/TILE;
    int right = (pos.x+radius)/TILE;
    int top = (pos.y-radius)/TILE;
    int bottom = (pos.y+radius)/TILE;

    for(int y = top; y <= bottom; y++) {
        for(int x = left; x <= right; x++) {
            if(mapa[y][x] == 1) {
                Rectangle wall = {
                    x*TILE,
                    y*TILE,
                    TILE,
                    TILE
                };

                if(CheckCollisionCircleRec(pos,radius,wall)) {
                    return true;
                }
            }
        }
    }

    return false;
}
void playerMovement(Vector2*playerPos,float*angle,float radius,int mapa[MAP_H][MAP_W],float*sprint,float*sprint_stamina) {
    float speed=1.5f;

    if(IsKeyDown(KEY_LEFT_SHIFT)&&*sprint>0 &&*sprint_stamina>0) {
        speed = 3.0f;
        *sprint -= 0.2f;
        *sprint_stamina =* sprint_stamina - 0.2f;
    }
    else {
        speed = 1.5f;
        if(*sprint<100.0f) *sprint += 0.1f;
        if(*sprint_stamina<100.0f) *sprint_stamina += 0.1f;
        if(*sprint<=0.0f) *sprint_stamina =- 100.0f;
        if(*sprint>=100.0f) *sprint_stamina = 100.0f;
    }

    Vector2 newPos=*playerPos;

    if(IsKeyDown(KEY_W))
    {
        newPos.x += cosf(*angle) * speed;
        newPos.y += sinf(*angle) * speed;
    }
    if(IsKeyDown(KEY_S))
    {
        newPos.x -= cosf(*angle) * speed;
        newPos.y -= sinf(*angle) * speed;
    }
    
    if(!checkCollisionCircleMap(newPos,radius,mapa))
    {
        *playerPos=newPos;
    }
}
