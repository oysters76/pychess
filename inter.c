#include "raylib.h"
#include <stdbool.h> 

#define BOARD_SIZE      8 
#define BOARD_CELL_SIZE 70 
#define BOARD_POS_X     50 
#define BOARD_POS_Y     50

const Color FOREGROUND_CELL = RED; 
const Color BACKGROUND_CELL = GREEN;

typedef struct {
    int board[BOARD_SIZE*BOARD_SIZE];     
} Board; 

void DrawChessBoard(int x, int y, Board * b){
    bool flag = true; 
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            Color c = flag ? FOREGROUND_CELL : BACKGROUND_CELL; 
            DrawRectangle(x, y, BOARD_CELL_SIZE, BOARD_CELL_SIZE, c);
            flag = !flag; 
            x += BOARD_CELL_SIZE;  
        }
        y += BOARD_CELL_SIZE; 
        x = BOARD_POS_X;
        flag = !flag; 
    }
}

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Chess Interpolation");

    SetTargetFPS(60);        
    
    Board board = {0}; 

    while (!WindowShouldClose())   
    {        
        BeginDrawing();
            ClearBackground(BLACK);
            DrawChessBoard(BOARD_POS_X, BOARD_POS_Y, &board);       
        EndDrawing();
    }

   CloseWindow();  
   
    return 0;
}
