#include "raylib.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - load and draw image");

    // Load image and convert it to a texture
    Image image = LoadImage("assets/horse_white.png"); // Load image from file
    Texture2D texture = LoadTextureFromImage(image);     // Convert image to texture
    UnloadImage(image);                                  // Unload image from CPU memory (not required anymore)
//
    SetTargetFPS(60);                                    // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())                         // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // Draw texture
            DrawTexture(texture, screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/2, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);                              // Unload texture

    CloseWindow();                                       // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
