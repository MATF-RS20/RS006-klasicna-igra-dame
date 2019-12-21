#ifndef BOARD_H
#define BOARD_H

#include <QString>
#include <QCursor>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <iostream>

static const int field_num = 8;
static const int piece_num = 12;
static const int player_white = 1, player_black = 0;
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
    int size, field_size;
    int piece_count = 0;
    Piece *pieces[max_pieces];

public:
    Board(BoardScene* _display, int _size);
    void set();
    void show();
};

#endif // BOARD_H
