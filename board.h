#ifndef BOARD_H
#define BOARD_H

#include <QLabel>
#include <QString>
#include <QCursor>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <cmath>

static const int field_num = 8;
static const int piece_num = 12;
/* Sledeca linija se ne moze menjati da se kod ne pokvari, ove vrednosti se takodje
 * koriste da oznace nacin kretanja figura odredjenog igraca */
static const int player_white = 1, player_black = -1;
static const int max_pieces = 32;

class BoardScene;
class VsComputerBoardScene;
class Piece;
class Board;
class VsComputerBoard;

static Board *localMPBoard;
static BoardScene *localMPBoardScene;

static Board *vsComputerBoard;
static BoardScene *vsComputerScene;

/* Kreiramo klasu koja nasledjuje QGraphicsScene da bi dodali funkciju koja odgovara
 * na dogadjaje misa. */
class BoardScene : public QGraphicsScene
{
private:

public:
    void setBoard(Board* board);
    void reset();
protected:
    Board* _game_state;
    bool _selected = false;
    int _selected_x, _selected_y;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

class VsComputerBoardScene : public BoardScene
{
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

class Piece
{
private:
    bool _king = false;
    int _player;

public:
    int x, y;
    Piece(int player, int _x, int _y);
    bool isKing();
    void makeKing();
    int player();
};

class Board
{
private:

public:
    Board(BoardScene* _display, QLabel* _move_display, QLabel* _result_display, int _size);
    void set();
    void show();
    bool makeMove(int start_x, int start_y, int end_x, int end_y);
    bool makePixelMove(int start_x, int start_y, int end_x, int end_y);
    bool isValidPieceSelection(int x, int y);
    bool isValidPixelPieceSelection(int x, int y);
    void movemax(int board[][8],int* best_score,int x,int y,int depth);
    void movemin(int board[][8],int* best_score,int x,int y,int depth);
    void jumpmax(int board[][8],int* best_score,int x,int y,int depth);
    void jumpmin(int board[][8],int* best_score,int x,int y,int depth);
    void generateBoardArray();
    bool canJumpUpLeft(int  board[][8],int i,int j);
    bool canJumpBottomLeft(int board[][8],int i,int j);
    bool canJumpUpRight(int board[][8],int i,int j);
    bool canJumpBottomRight(int board[][8],int i,int j);
    void minimax();
    void changeTurn();

protected:
    BoardScene *display;
    QLabel *turn_display;
    QLabel *result_display;
    int size, field_size;
    int piece_count = 0;
    int player_turn = player_black;
    Piece *piece_in_use = nullptr;
    Piece *pieces[max_pieces];
    int player_color = player_black;
    int board_array[8][8];
    bool isEmptyField(int x, int y);
    bool isOppositePlayer(int player, int x, int y);
    bool isValidJump(Piece *piece, int end_x, int end_y);
    bool isValidMove(Piece *piece, int end_x, int end_y);
    bool canJump(Piece* piece);
    bool canMove(Piece* piece);
    bool hasJump();
    bool hasMove();
    void updateResult();
    void pixelConvert(int x, int y, int &return_x, int &return_y);
    Piece* pieceAt(int x, int y);
    void removePiece(Piece *piece);

};

class VsComputerBoard : public Board
{
    /* XXX: Treba redefinisati isValidPieceSelection tako da jedino vraca da ako je to
     * igraceva figura (na igracevom potezu). */

public:
    VsComputerBoard(BoardScene* _display, QLabel* _move_display, QLabel* _result_display, int _size);
};

#endif // BOARD_H
