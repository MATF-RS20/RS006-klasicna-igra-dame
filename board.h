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
class Piece;
class Board;

static Board *localMPBoard;
static BoardScene *localMPBoardScene;

/* Kreiramo klasu koja nasledjuje QGraphicsScene da bi dodali funkciju koja odgovara
 * na dogadjaje misa. */
class BoardScene : public QGraphicsScene
{
private:
    Board* _game_state;
    bool _selected = false;
    int _selected_x, _selected_y;
public:
    void setBoard(Board* board);
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
    BoardScene *display;
    QLabel *move_display;
    int size, field_size;
    int piece_count = 0;
    int player_turn = player_black;
    Piece *pieces[max_pieces];
    bool isEmptyField(int x, int y);
    bool isOppositePlayer(int player, int x, int y);
    bool isValidJump(Piece *piece, int end_x, int end_y);
    bool canJump(Piece* piece);
    void pixelConvert(int x, int y, int &return_x, int &return_y);
    Piece* pieceAt(int x, int y);
    void removePiece(Piece *piece);

public:
    Board(BoardScene* _display, QLabel* _move_display, int _size);
    void set();
    void show();
    bool makeMove(int start_x, int start_y, int end_x, int end_y);
    bool makePixelMove(int start_x, int start_y, int end_x, int end_y);
    bool isValidPieceSelection(int x, int y);
    bool isValidPixelPieceSelection(int x, int y);
};

#endif // BOARD_H