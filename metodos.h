#include "raylib.h"
#define MAP_W 20
#define MAP_H 20
#define windowW 640
#define windowH 640
#define TILE 64
#define FoV (PI/3)
#define NUM_RAYS 640
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {struct Node *prev;int data;int item_id;struct Node *next;} Node;
Node* criar_lista(void);
bool inserir_inicio(Node *cabecalho, int valor, int id);
bool inserir_fim(Node *cabecalho, int valor, int id);
Node* buscar(Node *cabecalho, int id);
bool remover(Node *cabecalho, int id);
void exibir_lista(Node *cabecalho);
void destruir_lista(Node *cabecalho);
typedef struct Enemy{Vector2 pos;bool alive;int frame;int frameCount;int frameWidth;int frameHeight;} Enemy;
void loadMap(int mapa[MAP_H][MAP_W]);
void drawEnemy(Vector2 playerPos,float angle,Enemy enemy,Texture2D enemyTex,float zBuffer[]);
void castRay(Vector2 playerPos,float angle,int mapa[MAP_H][MAP_W],Texture2D wallTex, float zBuffer[]);
void drawFloorAndCeiling(Vector2 playerPos,float angle,Color*floorPixels,Color*ceilPixels,Texture2D floorTex,Texture2D ceilTex,Image screenBuffer,Texture2D screenTex);
bool checkCollisionCircleMap(Vector2 pos,float radius,int mapa[MAP_H][MAP_W]);
void playerMovement(Vector2*playerPos,float*angle,float radius,int mapa[MAP_H][MAP_W],float*sprint,float*sprint_stamina);