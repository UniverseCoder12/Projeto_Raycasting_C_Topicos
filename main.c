#include "raylib.h"
#include "metodos.c"
#define INVENTARIO_COLUNAS 4
#define INVENTARIO_LINHAS 4
#define INVENTARIO_SLOTS 16

#define SLOT_SIZE 100
#define SLOT_ESPACO 15

typedef enum EstadoJogo {
        MENU,       
        JOGANDO,
        INVENTARIO    
    } EstadoJogo;

int main()
{
    InitWindow(windowW, windowH, "Raycaster");
    SetTargetFPS(60);

    Node *inventory = criar_lista();
    inserir_fim(inventory, 10, 1);
    Vector2 player = {100, 100};
    float angle = 0;
    int playerray = 8;
    float sprint = 100.0f;
    float sprint_stamina = 100.0f;
    int mapa[MAP_H][MAP_W];
    int vol = 1;
    int shading = 255;
    float zBuffer[NUM_RAYS];

    SetExitKey(KEY_P);

    EstadoJogo estadoAtual = MENU;

    Enemy enemy = {
        .pos = {300, 300},
        .alive = true,
        .frame = 0,
        .frameCount = 11,
        .frameWidth = 64,
        .frameHeight = 128
    };
    Texture2D enemyTex = LoadTexture("../images/enemy.png");

    Texture2D wallTex = LoadTexture("../images/wall.png");

    Image floorImg = LoadImage("../images/floortexture.png");

    Image ceilImg = LoadImage("../images/ceiltexture.png");

    Texture2D floorTex = LoadTextureFromImage(floorImg);

    Texture2D ceilTex = LoadTextureFromImage(ceilImg);

    Color *floorPixels = (Color *)floorImg.data;

    Color *ceilPixels = (Color *)ceilImg.data;

    SetTextureFilter( wallTex, TEXTURE_FILTER_BILINEAR);

    Image screenBuffer = GenImageColor(windowW, windowH, BLACK);

    Texture2D screenTex = LoadTextureFromImage(screenBuffer);

    loadMap(mapa);

    int opcaoSelecionada = 1;

    bool fecharJogo = false;

    while (!WindowShouldClose() && !fecharJogo)
    {
        BeginDrawing();
        switch (estadoAtual) {

            case MENU: { 
                ClearBackground(BLACK); 
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                    opcaoSelecionada = 1; 
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                    opcaoSelecionada = 0; 
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    if (opcaoSelecionada == 0) {
                        estadoAtual = JOGANDO; 
                    } else if (opcaoSelecionada == 1) {
                        fecharJogo = true; 
                    }
                }
                const char* title = "Back Room";
                int titleWidth = MeasureText(title, 44);
                DrawText(title, windowW / 2 - titleWidth / 2, 150, 44, GOLD); // [1, 19]

                if (opcaoSelecionada == 0) {
                    DrawText("> JOGAR <", windowW / 2 - MeasureText("> JOGAR <", 24) / 2, 320, 24, RAYWHITE);
                    DrawText("FECHAR", windowW / 2 - MeasureText("FECHAR", 20) / 2, 380, 20, GRAY);
                } else {
                    DrawText("JOGAR", windowW / 2 - MeasureText("JOGAR", 20) / 2, 320, 20, GRAY);
                    DrawText("> FECHAR <", windowW / 2 - MeasureText("> FECHAR <", 24) / 2, 380, 24, RAYWHITE);
                }

                const char* footer = "Navegue com W/S ou Setas e selecione com Enter";
                int footerWidth = MeasureText(footer, 14);
                DrawText(footer, windowW / 2 - footerWidth / 2, 520, 14, DARKGRAY);
                break;
            }
            case JOGANDO: {

        if (IsKeyPressed(KEY_ESCAPE)) {
                    estadoAtual = MENU;
                }

        if (IsKeyPressed(KEY_E)) {
                    estadoAtual = INVENTARIO;
                }

        if (IsKeyDown(KEY_A))
            angle -= 0.07f;

        if (IsKeyDown(KEY_D))
            angle += 0.07f;

        playerMovement(&player,&angle,playerray,mapa,&sprint,&sprint_stamina);

        ClearBackground(BLACK);

        drawFloorAndCeiling(player, angle, floorPixels, ceilPixels, floorTex, ceilTex, screenBuffer, screenTex);

        castRay(player,angle,mapa,wallTex,zBuffer);

        drawEnemy(player,angle,enemy,enemyTex,zBuffer);

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
        break;
        }

        case INVENTARIO: {
            ClearBackground(BLACK);

            if (IsKeyPressed(KEY_E)) {
                    estadoAtual = JOGANDO;
                }

            int inicioX = 100;
            int inicioY = 100;
            Color rectangleColor = (Color){55, 55, 55, 255};
            Vector2 mouse = GetMousePosition();

            for (int linha = 0; linha < INVENTARIO_LINHAS; linha++)
            {
                for (int coluna = 0; coluna < INVENTARIO_COLUNAS; coluna++)
                {
                    int x = inicioX + coluna * (SLOT_SIZE + SLOT_ESPACO);
                    int y = inicioY + linha * (SLOT_SIZE + SLOT_ESPACO);
                    if (mouse.x >= x && mouse.x <= x+SLOT_SIZE && mouse.y >= y && mouse.y <= y+SLOT_SIZE){
                        rectangleColor = (Color){155, 155, 155, 255};
                    }
                    else{
                        rectangleColor = (Color){55, 55, 55, 255};
                    }

                    DrawRectangle(
                        x,
                        y,
                        SLOT_SIZE,
                        SLOT_SIZE,
                        rectangleColor
                    );

                    DrawRectangleLines(
                        x,
                        y,
                        SLOT_SIZE,
                        SLOT_SIZE,
                        WHITE
                    );
                }
            }

        }
    }

        EndDrawing();
    }

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
