#include "raylib.h"
#include <stdbool.h> 

#define FILE_NAME_MAX_SIZE 400 
#define MAX_IMG_ASSETS     30

#define BOARD_SIZE      8 
#define BOARD_CELL_SIZE 70 
#define BOARD_POS_X     50 
#define BOARD_POS_Y     50

#define STATE_INIT_CONFIG    0 
#define STATE_RENDER_INTERPL 1 
#define STATE_PAUSED         2 

#define NONE                 -1 
#define PIECE_BLACK          0x0   // 0000 0000
#define PIECE_WHITE          0x1   // 0000 0001 

#define PIECE_PAWN           0x02  // 0000 0010 
#define PIECE_KNIGHT         0x04  // 0000 0100
#define PIECE_BISHOP         0x08  // 0000 1000
#define PIECE_ROOK           0x10  // 0001 0000
#define PIECE_QUEEN          0x20  // 0010 0000
#define PIECE_KING           0x40  // 0100 0000

#define OUT_OF_BOUNDS        -1000

const Color FOREGROUND_CELL = RED; 
const Color BACKGROUND_CELL = GREEN;

typedef struct {
    int board[BOARD_SIZE*BOARD_SIZE];     
} Board; 

typedef struct {
    char filename[FILE_NAME_MAX_SIZE]; 
    Image image; 
    Texture2D texture; 
} BoardImgAsset; 

typedef struct {
    BoardImgAsset assets[MAX_IMG_ASSETS];
    int count;      
} BoardImgAssets; 

int GetChessBoardCellValue(Board * board, int x, int y){
    int ind = ((y*BOARD_SIZE)+x); 
    if (ind < 0 || ind >= BOARD_SIZE*BOARD_SIZE) return OUT_OF_BOUNDS; 
    return board->board[ind]; 
}

Board InitChessBoard(){
    Board b; 
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            b.board[(i*BOARD_SIZE)+j] = NONE; 
        }
    }
    return b; 
}

void DrawChessBoard(int x, int y, Board * b){
    bool flag = true; 
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            int cell_value = GetChessBoardCellValue(b, j, i); 
            if (cell_value == OUT_OF_BOUNDS) return;              
            Color c = flag ? FOREGROUND_CELL : BACKGROUND_CELL; 
            DrawRectangle(x, y, BOARD_CELL_SIZE, BOARD_CELL_SIZE, c);

            //draw texture 

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
