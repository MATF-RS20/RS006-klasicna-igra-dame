#include <board.h>

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {

    }
};

void BoardScene::setBoard(Board* board)
{
    _game_state = board;
}

Piece::Piece(int player, int _x, int _y)
    : _player(player), x(_x), y(_y)
    {}

bool Piece::isKing()
{
    return _king;
}

void Piece::makeKing()
{
    _king = true;
}

int Piece::player()
{
    return _player;
}

Board::Board(BoardScene* _display, int _size)
    : display(_display), size(_size)
{
    // Predpostavljamo da je scena kvadrat.
    field_size = size / field_num;
}

void Board::set()
{
    int white_count = piece_num, black_count = piece_num;
    int i = 0, j = 1;
    while(white_count > 0)
    {
        pieces[piece_count++] = new Piece(player_white, i, j);
        if(j+2 < field_num)
            j+=2;
        else
        {
            i++;
            // Odredjujemo koje je polje crno.
            j = (i+1)%2;
        }
        white_count--;
    }

    i = 5, j = 0;
    while(black_count > 0)
    {
        pieces[piece_count++] = new Piece(player_black, i, j);
        if(j+2 < field_num)
            j+=2;
        else
        {
            i++;
            // Odredjujemo koje je polje crno.
            j = (i+1)%2;
        }
        black_count--;
    }
}

void Board::show()
{
    // Prikazivanje polja table.
    for(int i = 0; i < field_num; i++)
        for(int j = 0; j < field_num; j++)
        {
            QGraphicsRectItem* field =
                    new QGraphicsRectItem(field_size * i, field_size * j, field_size, field_size);

            // Desno od svakog igraca treba da bude belo polje.
            if((i+j) % 2 == 0)
                field->setBrush(Qt::white);
            else
                field->setBrush(Qt::darkGreen);
            // Ne zelimo da crtamo granicu oko polja.
            field->setPen(Qt::NoPen);

            display->addItem(field);
        }

    // Prikazivanje figura igre.
    int padding = 5;
    // ???: Smanjujemo za jedan velicinu jer tako izgleda simetricno.
    int piece_size = field_size - padding * 2 - 1;
    for(int i = 0; i < piece_count; i++)
    {
        Piece* cur = pieces[i];
        QGraphicsEllipseItem* piece =
                new QGraphicsEllipseItem(field_size * cur->y + padding, field_size * cur->x + padding,
                                         piece_size, piece_size);

        if(cur->player() == player_white)
            piece->setBrush(Qt::lightGray);
        else
            piece->setBrush(Qt::darkRed);

        // Zelimo crni okvir oko figura, to je podrazumevano.
        display->addItem(piece);
    }
}


