#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h> 

#include "raylib.h"


#define FILE_NAME_MAX_SIZE    400 
#define MAX_IMG_ASSETS        30
#define BOARD_CONFIG_MAX_SIZE 15 

#define BOARD_SIZE      8 
#define BOARD_CELL_SIZE 70 
#define BOARD_POS_X     50 
#define BOARD_POS_Y     50
#define OFFSET_CONST    16

#define STATE_INIT_CONFIG    0 
#define STATE_RENDER_INTERPL 1 
#define STATE_PAUSED         2 

#define PIECE_NONE           -1 
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
    int flag; // stores the corresponding bit-flag related to the piece 
} BoardImgAsset; 

typedef struct {
    BoardImgAsset assets[MAX_IMG_ASSETS];
    int count;      
} BoardImgAssets; 

typedef struct {
    Board boards[BOARD_CONFIG_MAX_SIZE];
    int count; 
    int currentBoard; 
} BoardConfigurations;

void AddToChessImgAssets(BoardImgAssets * assets, const char * fname, int flag){
    BoardImgAsset asset = {0}; 
    strcpy(asset.filename, fname);
    asset.image = LoadImage(asset.filename); 
    asset.texture = LoadTextureFromImage(asset.image); 
    asset.flag = flag; 
    assets->assets[assets->count++] = asset; 
    UnloadImage(asset.image); 
    printf("Loading image asset: %s\n", fname);
}

void UnLoadAllChessAssets(BoardImgAssets * assets){
    for (int i = 0; i < assets->count; i++){
         printf("Unloading textures for : %s\n", assets->assets[i].filename); 
         UnloadTexture(assets->assets[i].texture);    
    }
}

void AddAllChessImgAssets(BoardImgAssets * assets){
    AddToChessImgAssets(assets, "assets/pawn_black.png", PIECE_BLACK | PIECE_PAWN); 
    AddToChessImgAssets(assets, "assets/pawn_white.png", PIECE_WHITE | PIECE_PAWN); 

    AddToChessImgAssets(assets, "assets/bishop_black.png", PIECE_BLACK | PIECE_BISHOP); 
    AddToChessImgAssets(assets, "assets/bishop_white.png", PIECE_WHITE | PIECE_BISHOP); 

    AddToChessImgAssets(assets, "assets/horse_black.png", PIECE_BLACK | PIECE_KNIGHT); 
    AddToChessImgAssets(assets, "assets/horse_white.png", PIECE_WHITE | PIECE_KNIGHT); 

    AddToChessImgAssets(assets, "assets/king_black.png", PIECE_BLACK | PIECE_KING); 
    AddToChessImgAssets(assets, "assets/king_white.png", PIECE_WHITE | PIECE_KING); 

    AddToChessImgAssets(assets, "assets/queen_black.png", PIECE_BLACK | PIECE_QUEEN); 
    AddToChessImgAssets(assets, "assets/queen_white.png", PIECE_WHITE | PIECE_QUEEN); 

    AddToChessImgAssets(assets, "assets/rook_black.png", PIECE_BLACK | PIECE_ROOK); 
    AddToChessImgAssets(assets, "assets/rook_white.png", PIECE_WHITE | PIECE_ROOK); 
}

int GetChessBoardIndex(int x, int y){
    return ((y*BOARD_SIZE)+x); 
}

int GetChessBoardCellValue(Board * board, int x, int y){
    int ind = GetChessBoardIndex(x, y); 
    if (ind < 0 || ind >= BOARD_SIZE*BOARD_SIZE) return OUT_OF_BOUNDS; 
    return board->board[ind]; 
}

Board InitChessBoard(){
    Board b; 
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            b.board[(i*BOARD_SIZE)+j] = PIECE_NONE; 
        }
    }
    return b; 
}

BoardImgAsset * FindChessAsset(BoardImgAssets * assets, int flag){
 for (int i = 0; i < assets->count; i++){
    BoardImgAsset ast = assets->assets[i]; 
    if (ast.flag == flag) return &(assets->assets[i]); 
 }
 return NULL; 
}

void AddBoardToBoardConfig(BoardConfigurations * config){
    if (config == NULL) return; 
    if (config->count >= BOARD_CONFIG_MAX_SIZE) return; 
    config->boards[config->count++] = InitChessBoard(); 
}

void AddChessPiece(Board * b, int gridX, int gridY, int flag){
  int ind = GetChessBoardIndex(gridX, gridY); 
  if (ind == OUT_OF_BOUNDS) return; 
  b->board[ind] = flag; 
}

void AddChessPieceToBoard(BoardConfigurations * config, int boardInd, int gridX, int gridY, int flag){
    if (config == NULL) return; 
    if (boardInd < 0 || boardInd >= config->count) return; 
    AddChessPiece(&(config->boards[boardInd]), gridX, gridY, flag); 
}



void DrawChessBoard(Board * b, BoardImgAssets * assets, int x, int y){
    bool flag = true; 
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            int cell_value = GetChessBoardCellValue(b, j, i); 
            if (cell_value == OUT_OF_BOUNDS) return;              
            Color c = flag ? FOREGROUND_CELL : BACKGROUND_CELL; 
            DrawRectangle(x, y, BOARD_CELL_SIZE, BOARD_CELL_SIZE, c);

            //draw texture if there is a piece on that cell 
            if (cell_value != PIECE_NONE){
                BoardImgAsset * asset = FindChessAsset(assets, cell_value);
                if (asset != NULL){
                    DrawTexture(asset->texture,    
                    (x+(BOARD_CELL_SIZE/OFFSET_CONST)), (y+(BOARD_CELL_SIZE/OFFSET_CONST)), WHITE); 
                }                 
            }

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
    
    
    BoardImgAssets assets = {0}; 
    AddAllChessImgAssets(&assets);

    BoardConfigurations boardConfigs = {0}; 
   
    AddBoardToBoardConfig(&boardConfigs);
    AddChessPieceToBoard(&boardConfigs, 0, 0, 0, (PIECE_BISHOP | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 0, 1, 1, (PIECE_KNIGHT | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 0, 2, 3, (PIECE_KNIGHT | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 0, 3, 2, (PIECE_PAWN | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 0, 5, 5, (PIECE_PAWN | PIECE_WHITE)); 

    AddBoardToBoardConfig(&boardConfigs);
    AddChessPieceToBoard(&boardConfigs, 1, 1, 1, (PIECE_KNIGHT | PIECE_BLACK)); 
    AddChessPieceToBoard(&boardConfigs, 1, 2, 3, (PIECE_QUEEN | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 1, 3, 2, (PIECE_PAWN | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 1, 5, 5, (PIECE_PAWN | PIECE_WHITE));  
   
    while (!WindowShouldClose())   
    {        
        if (IsKeyReleased(KEY_RIGHT)) {
         boardConfigs.currentBoard += 1; 
         boardConfigs.currentBoard %= boardConfigs.count; 
        }

        if (IsKeyReleased(KEY_LEFT)) {
         boardConfigs.currentBoard += 1; 
         boardConfigs.currentBoard %= boardConfigs.count; 
        }


        printf("%d\n", boardConfigs.currentBoard);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawChessBoard(&boardConfigs.boards[boardConfigs.currentBoard],
                                     &assets, BOARD_POS_X, BOARD_POS_Y);       
        EndDrawing();
    }
   
   UnLoadAllChessAssets(&assets); 
   CloseWindow();  
   
    return 0;
}
