#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h> 

#include "raylib.h"


#define FILE_NAME_MAX_SIZE    400 
#define MAX_IMG_ASSETS        30
#define BOARD_CONFIG_MAX_SIZE 15 
#define MAX_BOARD_DIFFS       500
#define PIECE_COUNT_MAX       250
#define MAX_PIECE_DIFF_SIZE   100

#define BOARD_SIZE      8 
#define BOARD_CELL_SIZE 70 
#define BOARD_POS_X     50 
#define BOARD_POS_Y     50
#define OFFSET_CONST    16

#define STATE_INIT_CONFIG    0 
#define STATE_RENDER_INTERPL 1 
#define STATE_PAUSED         2 

#define BOARD_DIFF_TYPE_REMOVED   0 
#define BOARD_DIFF_TYPE_APPEARED  1
#define BOARD_DIFF_TYPE_MOVED     2 
#define BOARD_DIFF_NO_DIFF        3
#define BOARD_DIFF_TYPE_LEN       4 

#define PIECE_NONE           -1 
#define PIECE_BLACK          0x1   // 0000 0001
#define PIECE_WHITE          0x2   // 0000 0010 

#define PIECE_PAWN           0x04  // 0000 0100 
#define PIECE_KNIGHT         0x08  // 0000 1000
#define PIECE_BISHOP         0x10  // 0001 0000
#define PIECE_ROOK           0x20  // 0010 0000
#define PIECE_QUEEN          0x40  // 0100 0000
#define PIECE_KING           0x80  // 1000 0000
#define PIECE_SIZE              6

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

typedef struct {
      int flag; 
      int type; 

      int currGridX; 
      int currGridY; 
      int tgtGridX; 
      int tgtGridY; 
} BoardDiff; 

typedef struct {
    BoardDiff diffs[MAX_BOARD_DIFFS]; 
    int count; 
} BoardDiffs;

typedef struct { 
    int flag; 
    int count; 
    int piecePos[2*MAX_PIECE_DIFF_SIZE]; 
    int piecePosSize; 
} PieceCount; 

typedef struct { 
    PieceCount counts[PIECE_COUNT_MAX]; 
    int count; 
} PieceCounts; 

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

PieceCounts InitBoardPieceCounts(){
    PieceCounts counts = {0}; 
    int colors[] = {PIECE_BLACK, PIECE_WHITE};
    int pieces[] = {PIECE_PAWN, PIECE_KING, PIECE_QUEEN, PIECE_BISHOP, PIECE_ROOK, PIECE_KNIGHT}; 

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < PIECE_SIZE; j++){
            PieceCount count = {0}; 
            count.flag = colors[i] | pieces[j]; 
            count.count = 0; 
            counts.counts[counts.count++] = count; 
        }
    }  

    return counts; 
}

void FindDiff(BoardDiffs * boardDiffs, Board * currentBoard, Board * nextBoard){
    if (boardDiffs == NULL || currentBoard == NULL || nextBoard == NULL) return; 
   
    boardDiffs->count = 0;  

    PieceCounts currentBoardPieceCounts = InitBoardPieceCounts(); 
    PieceCounts nextBoardPieceCounts    =  InitBoardPieceCounts(); 

    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            int cell_value = GetChessBoardCellValue(currentBoard, j, i); 
            if (cell_value == OUT_OF_BOUNDS) return; 
            if (cell_value == PIECE_NONE) continue; 
            for (int k = 0; k < currentBoardPieceCounts.count; k++){
                if (currentBoardPieceCounts.counts[k].flag == cell_value){
                    currentBoardPieceCounts.counts[k].count++; 
                    currentBoardPieceCounts.counts[k].piecePos[currentBoardPieceCounts.counts[k].piecePosSize++] = i; 
                    currentBoardPieceCounts.counts[k].piecePos[currentBoardPieceCounts.counts[k].piecePosSize++] = j; 
                }
            }
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            int cell_value = GetChessBoardCellValue(nextBoard, j, i); 
            if (cell_value == OUT_OF_BOUNDS) return; 
            if (cell_value == PIECE_NONE) continue; 
            for (int k = 0; k < nextBoardPieceCounts.count; k++){
                if (nextBoardPieceCounts.counts[k].flag == cell_value){
                    nextBoardPieceCounts.counts[k].count++; 
                    nextBoardPieceCounts.counts[k].piecePos[nextBoardPieceCounts.counts[k].piecePosSize++] = i; 
                    nextBoardPieceCounts.counts[k].piecePos[nextBoardPieceCounts.counts[k].piecePosSize++] = j; 
                }
            }
        }
    }

    for (int k = 0; k < currentBoardPieceCounts.count; k++){
        PieceCount pieceCount = currentBoardPieceCounts.counts[k]; 
        int pieceCnt = pieceCount.count; 
        for (int l = 0; l < nextBoardPieceCounts.count; l++){
            PieceCount otherPieceCount = nextBoardPieceCounts.counts[l]; 
            int otherPieceCnt = otherPieceCount.count; 
            if (pieceCount.flag == otherPieceCount.flag){
                
                if (pieceCnt == 0 && otherPieceCnt == 0){ //both boards don't have this piece so no diff 
                    break; 
                }

                if (pieceCnt == otherPieceCnt){ //both boards have equal amount of pieces of the same type, move them all
                    for (int a = 0; a < pieceCnt; a++){
                        BoardDiff diff = {0}; 
                        diff.flag = pieceCount.flag; 
                        diff.type = BOARD_DIFF_TYPE_MOVED;
                        diff.currGridY = pieceCount.piecePos[a*2]; 
                        diff.currGridX = pieceCount.piecePos[(a*2)+1]; 
                        diff.tgtGridY  = otherPieceCount.piecePos[a*2]; 
                        diff.tgtGridX  = otherPieceCount.piecePos[(a*2)+1]; 
                        if (diff.currGridX == diff.tgtGridX && diff.currGridY == diff.tgtGridY){
                            diff.type = BOARD_DIFF_NO_DIFF;
                        }
                        boardDiffs->diffs[boardDiffs->count++] = diff; 
                    }
                }

                if (pieceCnt != otherPieceCnt){ //some pieces are added or disappeared in the boards, other pieces moved
                    if (pieceCnt > otherPieceCnt){
                        for (int a = 0; a < pieceCnt; a++){
                            if (otherPieceCnt == 0){
                                BoardDiff diff = {0}; 
                                diff.flag = pieceCount.flag; 
                                diff.type = BOARD_DIFF_TYPE_REMOVED;
                                boardDiffs->diffs[boardDiffs->count++] = diff; 
                            }else{
                                BoardDiff diff = {0}; 
                                diff.flag = pieceCount.flag; 
                                diff.type = BOARD_DIFF_TYPE_MOVED;
                                diff.currGridY = pieceCount.piecePos[a*2]; 
                                diff.currGridX = pieceCount.piecePos[(a*2)+1]; 
                                diff.tgtGridY  = otherPieceCount.piecePos[a*2]; 
                                diff.tgtGridX  = otherPieceCount.piecePos[(a*2)+1]; 
                                if (diff.currGridX == diff.tgtGridX && diff.currGridY == diff.tgtGridY){
                                    diff.type = BOARD_DIFF_NO_DIFF;
                                }
                                boardDiffs->diffs[boardDiffs->count++] = diff; 
                                otherPieceCnt--; 
                            }
                        }
                    }else{
                        for (int a = 0; a < otherPieceCnt; a++){
                            if (pieceCnt == 0){
                                BoardDiff diff = {0}; 
                                diff.flag = pieceCount.flag; 
                                diff.type = BOARD_DIFF_TYPE_APPEARED;
                                boardDiffs->diffs[boardDiffs->count++] = diff; 
                            }else{
                                BoardDiff diff = {0}; 
                                diff.flag = pieceCount.flag; 
                                diff.type = BOARD_DIFF_TYPE_MOVED;
                                diff.currGridY = pieceCount.piecePos[a*2]; 
                                diff.currGridX = pieceCount.piecePos[(a*2)+1]; 
                                diff.tgtGridY  = otherPieceCount.piecePos[a*2]; 
                                diff.tgtGridX  = otherPieceCount.piecePos[(a*2)+1]; 
                                if (diff.currGridX == diff.tgtGridX && diff.currGridY == diff.tgtGridY){
                                    diff.type = BOARD_DIFF_NO_DIFF;
                                }
                                boardDiffs->diffs[boardDiffs->count++] = diff; 
                                pieceCnt--; 
                            }
                        }
                    }
                }
                break; 
            }
        }
    }
}

char * GetPieceColor(int flag){
    if ((flag & PIECE_BLACK) == PIECE_BLACK) return "BLACK "; 
    if ((flag & PIECE_WHITE) == PIECE_WHITE) return "WHITE ";  
    return "NO_COLOR"; 
}

char * GetPieceName(int flag){
    if ((flag & PIECE_BISHOP) == PIECE_BISHOP) return "BISHOP"; 
    if ((flag & PIECE_KNIGHT) == PIECE_KNIGHT) return "KNIGHT"; 
    if ((flag & PIECE_PAWN) == PIECE_PAWN) return "PAWN"; 
    if ((flag & PIECE_KING) == PIECE_KING) return "KING"; 
    if ((flag & PIECE_ROOK) == PIECE_ROOK) return "ROOK"; 
    if ((flag & PIECE_QUEEN) == PIECE_QUEEN) return "QUEEN"; 
    return "NONE";  
}

char * GetFullPieceName(int flag){
    char * pieceColor = GetPieceColor(flag); 
    char * pieceName = GetPieceName(flag); 
    size_t n = strlen(pieceColor) + strlen(pieceName) + 1;
    char *result = (char *)malloc(n);
    if (result == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    result[0] = '\0'; 
    strcat(result, pieceColor); 
    strcat(result, pieceName); 
    return result;  
}

char * GetPieceAnimationName(int type){
    if (type == BOARD_DIFF_TYPE_APPEARED){
        return "APPEAR"; 
    }else if (type == BOARD_DIFF_TYPE_MOVED){
        return "MOVEMENT"; 
    }else if (type == BOARD_DIFF_TYPE_REMOVED){
        return "REMOVAL"; 
    }else if (type == BOARD_DIFF_NO_DIFF){
        return "NO_MOVEMENT";
    }
    return "ERR"; 
}

void PrintBoardDiff(BoardDiffs * diffs){
    if (diffs == NULL) return; 
    for (int i = 0; i < diffs->count; i++){
        BoardDiff diff = diffs->diffs[i]; 
        char * pieceName = GetFullPieceName(diff.flag); 
        char * animationName = GetPieceAnimationName(diff.type); 
        printf("Diff %d: Piece: %s Type: %s CurrentX: %d CurrentY: %d TgtX: %d TgtY: %d\n", 
            i+1, pieceName, animationName, diff.currGridX, diff.currGridY, diff.tgtGridX, diff.tgtGridY);
        free(pieceName);  
    }
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
    AddChessPieceToBoard(&boardConfigs, 0, 3, 2, (PIECE_PAWN | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 0, 5, 5, (PIECE_PAWN | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 0, 1, 5, (PIECE_PAWN | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 0, 2, 5, (PIECE_KNIGHT | PIECE_WHITE)); 

    AddBoardToBoardConfig(&boardConfigs);
    AddChessPieceToBoard(&boardConfigs, 1, 3, 2, (PIECE_PAWN | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 1, 5, 5, (PIECE_PAWN | PIECE_WHITE));  
    AddChessPieceToBoard(&boardConfigs, 1, 3, 5, (PIECE_PAWN | PIECE_WHITE)); 
    AddChessPieceToBoard(&boardConfigs, 1, 4, 5, (PIECE_KNIGHT | PIECE_WHITE)); 

    BoardDiffs diffs = {0}; 
    FindDiff(&diffs, &boardConfigs.boards[0], &boardConfigs.boards[1]); 
    PrintBoardDiff(&diffs);

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
