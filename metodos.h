#include "raylib.h"
#define MAP_W 20
#define MAP_H 20
#define windowW 640
#define windowH 640
#define TILE 64
#define FoV (PI/3)
#define NUM_RAYS 640
typedef struct Enemy{Vector2 pos;bool alive;int frame;int frameCount;int frameWidth;int frameHeight;} Enemy;
void loadMap(int mapa[MAP_H][MAP_W]);
void drawEnemy(Vector2 playerPos,float angle,Enemy enemy,Texture2D enemyTex,float zBuffer[]);
void castRay(Vector2 playerPos,float angle,int mapa[MAP_H][MAP_W],Texture2D wallTex, float zBuffer[]);
void drawFloorAndCeiling(Vector2 playerPos,float angle,Color*floorPixels,Color*ceilPixels,Texture2D floorTex,Texture2D ceilTex,Image screenBuffer,Texture2D screenTex);
bool checkCollisionCircleMap(Vector2 pos,float radius,int mapa[MAP_H][MAP_W]);
void playerMovement(Vector2*playerPos,float*angle,float radius,int mapa[MAP_H][MAP_W],float*sprint,float*sprint_stamina);