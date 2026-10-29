#include "raylib.h"
#include "metodos.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#define MAP_W 20
#define MAP_H 20
#define windowW 640
#define windowH 640
#define TILE 64
#define FoV (PI/3)
#define NUM_RAYS 640

#include <stdio.h>

void loadMap(int mapa[MAP_H][MAP_W])
{
    FILE*f=fopen("../map/map.txt","r");

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

void enemyWalk(
    Enemy **enemy,
    Vector2 *playerPos,
    int mapa[MAP_H][MAP_W]
)
{
    if (enemy == NULL || *enemy == NULL || playerPos == NULL)
        return;

    // Posição REAL do centro do inimigo no mapa.
    // Não subtraia frameWidth / 2.
    int ex = (int)floorf((*enemy)->pos.x / TILE);
    int ey = (int)floorf((*enemy)->pos.y / TILE);

    int px = (int)floorf(playerPos->x / TILE);
    int py = (int)floorf(playerPos->y / TILE);

    // Verifica os limites do mapa
    if (ex < 0 || ex >= MAP_W ||
        ey < 0 || ey >= MAP_H ||
        px < 0 || px >= MAP_W ||
        py < 0 || py >= MAP_H)
        return;

    // O inimigo e o jogador devem estar em células livres
    if (mapa[ey][ex] != 0 || mapa[py][px] != 0)
        return;

    // Distância mínima em número de células
    int dist[MAP_H][MAP_W];

    // Fila da busca BFS
    int filaX[MAP_W * MAP_H];
    int filaY[MAP_W * MAP_H];

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            dist[y][x] = -1;
        }
    }

    int inicio = 0;
    int fim = 0;

    // Começa a busca na posição do jogador
    filaX[fim] = px;
    filaY[fim] = py;
    fim++;

    dist[py][px] = 0;

    // Movimentos: direita, esquerda, baixo e cima
    int dirX[4] = { 1, -1, 0, 0 };
    int dirY[4] = { 0, 0, 1, -1 };

    // BFS: encontra as distâncias pelo caminho livre
    while (inicio < fim)
    {
        int x = filaX[inicio];
        int y = filaY[inicio];
        inicio++;

        for (int i = 0; i < 4; i++)
        {
            int nx = x + dirX[i];
            int ny = y + dirY[i];

            if (nx < 0 || nx >= MAP_W ||
                ny < 0 || ny >= MAP_H)
                continue;

            if (mapa[ny][nx] != 0)
                continue;

            if (dist[ny][nx] != -1)
                continue;

            dist[ny][nx] = dist[y][x] + 1;

            filaX[fim] = nx;
            filaY[fim] = ny;
            fim++;
        }
    }

    // Se não existe caminho até o jogador, não se move
    if (dist[ey][ex] <= 0)
        return;

    // Encontra o próximo bloco do menor caminho
    int proximoX = ex;
    int proximoY = ey;

    for (int i = 0; i < 4; i++)
    {
        int nx = ex + dirX[i];
        int ny = ey + dirY[i];

        if (nx < 0 || nx >= MAP_W ||
            ny < 0 || ny >= MAP_H)
            continue;

        if (dist[ny][nx] == dist[ey][ex] - 1)
        {
            proximoX = nx;
            proximoY = ny;
            break;
        }
    }

    // Se não encontrou um próximo bloco, para
    if (proximoX == ex && proximoY == ey)
        return;

    float velocidade = 1.8f;
    float raioInimigo = TILE / 4.0f;


Vector2 centroAtual = {
    (ex + 0.5f) * TILE,
    (ey + 0.5f) * TILE
};

Vector2 centroProximo = {
    (proximoX + 0.5f) * TILE,
    (proximoY + 0.5f) * TILE
};

Vector2 *pos = &(*enemy)->pos;

// Caminho horizontal
if (proximoX != ex)
{
    // Primeiro, alinhar o inimigo ao centro vertical do corredor
    float erroY = centroAtual.y - pos->y;

    if (fabsf(erroY) > 0.5f)
    {
        float passoY;

        if (fabsf(erroY) < velocidade)
            passoY = erroY;
        else
            passoY = (erroY > 0) ? velocidade : -velocidade;

        Vector2 tentativa = *pos;
        tentativa.y += passoY;

        if (!checkCollisionCircleMap(
                tentativa, raioInimigo, mapa))
        {
            pos->y = tentativa.y;
        }

        return;
    }

    // Corrige pequenos erros de alinhamento
    pos->y = centroAtual.y;

    // Continua avançando até o centro do próximo bloco
    float erroX = centroProximo.x - pos->x;

    float passoX;

    if (fabsf(erroX) < velocidade)
        passoX = erroX;
    else
        passoX = (erroX > 0) ? velocidade : -velocidade;

    Vector2 tentativa = *pos;
    tentativa.x += passoX;

    if (!checkCollisionCircleMap(
            tentativa, raioInimigo, mapa))
    {
        pos->x = tentativa.x;
    }
}

// Caminho vertical
else if (proximoY != ey)
{
    // Primeiro, alinhar o inimigo ao centro horizontal do corredor
    float erroX = centroAtual.x - pos->x;

    if (fabsf(erroX) > 0.5f)
    {
        float passoX;

        if (fabsf(erroX) < velocidade)
            passoX = erroX;
        else
            passoX = (erroX > 0) ? velocidade : -velocidade;

        Vector2 tentativa = *pos;
        tentativa.x += passoX;

        if (!checkCollisionCircleMap(
                tentativa, raioInimigo, mapa))
        {
            pos->x = tentativa.x;
        }

        return;
    }

    // Corrige pequenos erros de alinhamento
    pos->x = centroAtual.x;

    // Continua avançando até o centro do próximo bloco
    float erroY = centroProximo.y - pos->y;

    float passoY;

    if (fabsf(erroY) < velocidade)
        passoY = erroY;
    else
        passoY = (erroY > 0) ? velocidade : -velocidade;

    Vector2 tentativa = *pos;
    tentativa.y += passoY;

    if (!checkCollisionCircleMap(
            tentativa, raioInimigo, mapa))
    {
        pos->y = tentativa.y;
    }
}

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

bool checkCollisionCircleMap(
    Vector2 pos,
    float radius,
    int mapa[MAP_H][MAP_W]
)
{
    // Impede que o círculo saia dos limites do mundo
    if (pos.x - radius < 0 ||
        pos.y - radius < 0 ||
        pos.x + radius >= MAP_W * TILE ||
        pos.y + radius >= MAP_H * TILE)
    {
        return true;
    }

    int left = (int)floorf((pos.x - radius) / TILE);
    int right = (int)floorf((pos.x + radius) / TILE);
    int top = (int)floorf((pos.y - radius) / TILE);
    int bottom = (int)floorf((pos.y + radius) / TILE);

    for (int y = top; y <= bottom; y++)
    {
        for (int x = left; x <= right; x++)
        {
            // Proteção contra índices inválidos
            if (x < 0 || x >= MAP_W ||
                y < 0 || y >= MAP_H)
            {
                return true;
            }

            if (mapa[y][x] == 1)
            {
                Rectangle parede = {
                    (float)(x * TILE),
                    (float)(y * TILE),
                    (float)TILE,
                    (float)TILE
                };

                if (CheckCollisionCircleRec(pos, radius, parede))
                {
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
        *sprint -= 0.21875f;
        *sprint_stamina =* sprint_stamina - 0.21875f;
    }
    else {
        speed = 1.5f;
        if(*sprint<100.0f) *sprint += 0.109375f;
        if(*sprint_stamina<100.0f) *sprint_stamina += 0.109375f;
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

Node* criar_lista(void) {
    Node *cabecalho = (Node*) malloc(sizeof(Node));
    if (cabecalho == NULL) {
        printf("Erro crítico: Falha na alocação de memória para o nó cabeçalho.\n");
        return NULL;
    }
    cabecalho->data = 0;
    cabecalho->item_id = 0;
    cabecalho->prev = NULL;
    cabecalho->next = NULL;
    return cabecalho;
}


bool inserir_inicio(Node *cabecalho, int quant, int id) {
    if (cabecalho == NULL) return false;

    Node *novo_no = (Node*) malloc(sizeof(Node));
    if (novo_no == NULL) {
        printf("Erro: Falha na alocação de memória para novo nó.\n");
        return false;
    }

    novo_no->data = quant;
    novo_no->item_id = id;
    novo_no->prev = cabecalho;
    novo_no->next = cabecalho->next;
    cabecalho->next->prev = novo_no;
    cabecalho->next = novo_no;

    return true;
}


bool inserir_fim(Node *cabecalho, int valor, int id) {
    if (cabecalho == NULL) return false;

    Node *novo_no = (Node*) malloc(sizeof(Node));
    if (novo_no == NULL) {
        printf("Erro: Falha na alocação de memória para novo nó.\n");
        return false;
    }

    novo_no->data = valor;
    novo_no->item_id = id;
    novo_no->next = NULL;

    Node *atual = cabecalho;
    while (atual->next != NULL) {
        atual = atual->next;
    }
    
    atual->next = novo_no;
    novo_no->prev = atual;
    return true;
}


Node* buscar(Node *cabecalho, int id) {
    if (cabecalho == NULL) return NULL;

    Node *atual = cabecalho->next;
    while (atual != NULL) {
        if (atual->item_id == id) {
            return atual;
        }
        atual = atual->next;
    }
    return NULL;
}


bool remover(Node *cabecalho, int id) {
    if (cabecalho == NULL || cabecalho->next == NULL) {
        return false;
    }

    Node *anterior = cabecalho;
    Node *atual = cabecalho->next;
    Node *proximo = cabecalho->next->next;
    int i = 0;

    while (atual != NULL && i < id)
    {
        anterior = atual;
        atual = atual->next;
        proximo = atual->next;
        i++;
    }

    if (atual == NULL) {
        return false;
    }

    anterior->next = atual->next;
    if (atual->next != NULL) {
        atual->next->prev = anterior;
    }

    free(atual);
    return true;
}


Node *pegar_elemento_lista(Node *cabecalho, int pos)
{
    if (cabecalho == NULL || pos < 0)
        return NULL;

    Node *atual = cabecalho->next;
    int i = 0;

    while (atual != NULL && i < pos)
    {
        atual = atual->next;
        i++;
    }

    return atual;
}

void destruir_lista(Node *cabecalho) {
    if (cabecalho == NULL) return;

    Node *atual = cabecalho;
    while (atual != NULL) /* Definição da estrutura de um nó da lista encadeada */{
        Node *proximo = atual->next;
        free(atual);
        atual = proximo;
    }
}

bool consumir_item(Node *cabecalho, int pos)
{
    Node *item = pegar_elemento_lista(cabecalho, pos);

    if (item == NULL)
        return false;

    // Verifica se existe quantidade disponível
    if (item->data <= 0)
        return false;

    // Consome uma unidade
    item->data--;

    // Remove o item da lista se a quantidade chegar a zero
    if (item->data == 0)
    {
        remover(cabecalho, pos);
    }

    return true;
}