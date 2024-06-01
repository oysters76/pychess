#include "raylib.h"

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Chess Interpolation");

    SetTargetFPS(60);        
    
    while (!WindowShouldClose())   
    {        
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }

   CloseWindow();  
   
    return 0;
}
