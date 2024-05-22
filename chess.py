# Example file showing a basic pygame "game loop"
import pygame
import os 

# pygame setup
pygame.init()
screen = pygame.display.set_mode((1280, 720))
clock = pygame.time.Clock()
running = True

class Piece(object):
    COLORS = ["black", "white"] 
    PIECES = ["king", "queen", "rook", "bishop", "horse", "pawn"] 

    P_KING = 0 
    P_QUEEN = 1 
    P_ROOK = 2 
    P_BISHOP = 3 
    P_HORSE = 4 
    P_PAWN = 5 

    P_BLACK = 0 
    P_WHITE = 1 

    def __init__(self, screen, x=0, y=0,size=50, piece=P_KING, color=P_BLACK):
        self.piece = piece 
        self.color = color 
        self.asset_name = self.get_assetname()
        self.path = self.get_path() 
        self.surf = None
        self.x = x 
        self.y = y 
        self.is_dragged = False 
        self.is_captured = False
        self.screen = screen
        self.size = size
        self.associated_rect = None
    
    def make_surf(self):
        return pygame.image.load(self.path)
    
    def set_associ_rect(self, rect):
        self.associated_rect = rect

    def get_surf(self):
        if (self.surf == None):
            self.surf = self.make_surf()
            return self.surf 
        else:
            return self.surf 
    
    def get_assetname(self):
        return Piece.get_piece_name(self.piece) + "_" + Piece.get_color_name(self.color) + ".png"

    def get_path(self):
        return os.path.join("assets", self.asset_name) 
    
    def set_pos(self, x,y):
        self.x = x 
        self.y = y 
    
    @classmethod
    def get_piece_name(classz, piece):
        return Piece.PIECES[piece] 
    
    @classmethod
    def get_color_name(classz, color):
        return Piece.COLORS[color]
    
    def draw(self):
        self.screen.blit(self.get_surf(), (self.x, self.y))

    def set_pos_from_rect(self, rect):
        center_x = (rect.x )
        center_y = (rect.y ) 
        
        center_x += (self.size//8) 
        center_y += (self.size//8) 

        self.set_pos(center_x, center_y)
        self.set_associ_rect(rect)

class Board(object):
    SQ_SIZE = 8 
    def __init__(self, x, y, screen, size=20, c=(255,0,0), c2=(0,255,0)):
        self.size = size 
        self.x = x 
        self.y = y 
        self.screen = screen
        self.rects = [] 
        self.calc_rects() 
        self.color = c
        self.fcolor = c2 
    
    def change_color(self, c, c2):
        self.color = c 
        self.fcolor = c2 

    def calc_rects(self):
        ix = self.x 
        iy = self.y 
        for _ in range(Board.SQ_SIZE):
            for _ in range(Board.SQ_SIZE): 
                r = pygame.Rect(ix, iy, self.size, self.size) 
                self.rects.append(r)
                ix += self.size
            ix = self.x 
            iy += self.size 

    def draw_base_grid(self):
        change = True 
        for i, r in enumerate(self.rects):
            if (i % Board.SQ_SIZE == 0):
                change = not change 
            current_color = self.color if change else self.fcolor 
            pygame.draw.rect(self.screen, current_color, r)
            change = not change

    def get_rect(self, x, y):
        return self.rects[((y * Board.SQ_SIZE) + x)]  

    def draw(self):
        self.draw_base_grid()
    

# Override this class to support different board configurations . e.g. Fishers' 
# Override the make_board_config method 
class BoardSetup(object):
    def __init__(self, board:Board, screen, piece_size=50, board_size=Board.SQ_SIZE) -> None:
        self.board = board 
        self.pieces = [] 
        self.screen = screen 
        self.piece_size = piece_size 
        self.board_size = board_size
    
    def make_piece(self, x, y, piece, color):
        r = board.get_rect(x, y)
        p = Piece(screen, x=0, y=0, size=self.piece_size, piece=piece, color=color)
        p.set_pos_from_rect(r)
        return p

    def add_piece(self, x, y, p, c):
        self.pieces.append(self.make_piece(x,y,p,c))

    def make_pawn_line(self, line, c):
        for i in range(self.board_size):
            self.add_piece(i, line, Piece.P_PAWN, c)

    def make_board_config(self):
        self.add_piece(0, 0, Piece.P_ROOK, Piece.P_BLACK)
        self.add_piece(1, 0, Piece.P_HORSE, Piece.P_BLACK)
        self.add_piece(2, 0, Piece.P_BISHOP, Piece.P_BLACK)
        self.add_piece(3, 0, Piece.P_QUEEN, Piece.P_BLACK)
        self.add_piece(4, 0, Piece.P_KING, Piece.P_BLACK)
        self.add_piece(5, 0, Piece.P_BISHOP, Piece.P_BLACK)
        self.add_piece(6, 0, Piece.P_HORSE, Piece.P_BLACK)
        self.add_piece(7, 0, Piece.P_ROOK, Piece.P_BLACK)

        self.make_pawn_line(1, Piece.P_BLACK)
        self.make_pawn_line(6, Piece.P_WHITE)

        self.add_piece(0, 7, Piece.P_ROOK, Piece.P_WHITE)
        self.add_piece(1, 7, Piece.P_HORSE, Piece.P_WHITE)
        self.add_piece(2, 7, Piece.P_BISHOP, Piece.P_WHITE)
        self.add_piece(3, 7, Piece.P_QUEEN, Piece.P_WHITE)
        self.add_piece(4, 7, Piece.P_KING, Piece.P_WHITE)
        self.add_piece(5, 7, Piece.P_BISHOP, Piece.P_WHITE)
        self.add_piece(6, 7, Piece.P_HORSE, Piece.P_WHITE)
        self.add_piece(7, 7, Piece.P_ROOK, Piece.P_WHITE)

    def draw_layout(self):
        for p in self.pieces: 
            p.draw()

             
board = Board(50, 50, screen, size=69)  
boardSetup = BoardSetup(board, screen, 50) 
boardSetup.make_board_config()

while running:
    # poll for events
    # pygame.QUIT event means the user clicked X to close your window
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # fill the screen with a color to wipe away anything from last frame
    screen.fill("black")

    # RENDER YOUR GAME HERE
    
    board.draw()
    boardSetup.draw_layout()

    # flip() the display to put your work on screen
    pygame.display.flip()

    clock.tick(60)  # limits FPS to 60

pygame.quit()
