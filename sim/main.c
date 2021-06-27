#include "raylib.h"

/*
struct Game;

struct Game
{
    int something;
};

typedef struct Game Game;
*/

int main(void)
{
    InitWindow(1280, 720, "sim");
    Image image = GenImageColor(256, 128, WHITE);
    Texture2D texture = LoadTextureFromImage(image);
    ImageDrawRectangle(&image, 0, 0, 16, 16, BLUE);
    UpdateTexture(texture, image.data);
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }
    UnloadTexture(texture);
    UnloadImage(image);
    CloseWindow();
    return 0;
}