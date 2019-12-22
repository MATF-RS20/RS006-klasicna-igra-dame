#include "board.h"

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        auto x = static_cast<int>(round(event->scenePos().x()));
        auto y = static_cast<int>(round(event->scenePos().y()));

        if(!_selected && (_game_state->isValidPixelPieceSelection(x, y)))
        {
            _selected = true;
            _selected_x = x;
            _selected_y = y;
            return;
        }

        if(_selected)
        {
            _game_state->makePixelMove(_selected_x, _selected_y, x, y);
            _game_state->show();
            _selected = false;
        }
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

bool Board::isEmptyField(int x, int y)
{
    if(x >= field_num || x < 0)
        return false;
    if(y >= field_num || y < 0)
        return false;

    return (pieceAt(x, y) == nullptr);
}

bool Board::isOppositePlayer(int player, int x, int y)
{
    return ((pieceAt(x,y) != nullptr) && (pieceAt(x, y)->player() == player * (-1)));
}

bool Board::isValidJump(Piece *piece, int end_x, int end_y)
{
    /* XXX: Podrazumevamo da smo dali ispravan skok u tome da je zeljeno polje
     * direktno dijagonalno i odgovarajuce sa statusom figure (dama ili ne). */
    bool free = isEmptyField(end_x, end_y);
    // Polje izmedju krajnjeg polja skoka i pocetnog je na njihovom proseku
    bool jumped =
            isOppositePlayer(piece->player(), (piece->x + end_x) / 2, (piece->y + end_y) / 2);
    return free && jumped;
}

bool Board::canJump(Piece* piece)
{
    // Podrazumevamo da je potez pravog igraca.
    bool hasToJump = false;
    int player = piece->player();
    int opposite_player = player * (-1);
    int x = piece->x, y = piece->y;

    if(isValidJump(piece, x + 2, y + 2 * player))
        hasToJump = true;
    else if(isValidJump(piece, x - 2, y + 2 * player))
        hasToJump = true;
    else if(piece->isKing())
    {
        if(isValidJump(piece, x + 2, y + 2 * opposite_player))
            hasToJump = true;
        else if(isValidJump(piece, x - 2, y + 2 * opposite_player))
            hasToJump = true;
    }

    return hasToJump;
}

void Board::pixelConvert(int x, int y, int &return_x, int &return_y)
{
    return_x = x / field_size;
    return_y = y / field_size;
}

Piece* Board::pieceAt(int x, int y)
{
    Piece* res = nullptr;
    for(int i = 0; i < piece_count; i++)
        if(pieces[i]->x == x && pieces[i]->y == y)
        {
            res = pieces[i];
            break;
        }

    return res;
}

void Board::removePiece(Piece *piece)
{
    for(int i = 0; i < piece_count; i++)
        if(piece == pieces[i])
        {
            delete piece;
            for(int j = i; j < piece_count - 1; j++)
                pieces[j] = pieces[j + 1];

            piece_count--;
            break;
        }
}

Board::Board(BoardScene* _display, QLabel* _move_display, int _size)
    : display(_display), move_display(_move_display),  size(_size)
{
    // Predpostavljamo da je scena kvadrat.
    field_size = size / field_num;
}

void Board::set()
{
    int white_remaining = piece_num, black_remaining = piece_num;
    int i = 0, j = 1;
    while(white_remaining > 0)
    {
        pieces[piece_count++] = new Piece(player_white, j, i);
        if(j+2 < field_num)
            j+=2;
        else
        {
            i++;
            // Odredjujemo koje je polje crno.
            j = (i+1)%2;
        }
        white_remaining--;
    }

    i = 5, j = 0;
    while(black_remaining > 0)
    {
        pieces[piece_count++] = new Piece(player_black, j, i);
        if(j+2 < field_num)
            j+=2;
        else
        {
            i++;
            // Odredjujemo koje je polje crno.
            j = (i+1)%2;
        }
        black_remaining--;
    }
}

void Board::show()
{
    this->display->clear();

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
                new QGraphicsEllipseItem(field_size * cur->x + padding, field_size * cur->y + padding,
                                         piece_size, piece_size);

        if(cur->player() == player_white)
            piece->setBrush(Qt::lightGray);
        else
            piece->setBrush(Qt::darkRed);

        // Zelimo crni okvir oko figura, to je podrazumevano.
        display->addItem(piece);
    }
}

bool Board::makeMove(int start_x, int start_y, int end_x, int end_y)
{
    // Proveravanje da li je ispravno odabrana pocetna pozicija.
    if(!isValidPieceSelection(start_x, start_y))
        return false;

    Piece *cur = pieceAt(start_x, start_y);

    // Proveravanje da li je zeljeni potez validan potez bez preskakanja.
    // Proveramo da li igrac mora da preskace, tj. da li ima priliku da preskace.
    int player = cur->player();
    int opposite_player = player * (-1);
    bool hasToJump = false;

    for(int i = 0; i < piece_count; i++)
    {
        Piece *piece = pieces[i];
        if(piece->player() != player)
            continue;

        hasToJump = canJump(piece);

        if(hasToJump) break;
    }

    bool validSmallMove = false;
    // Sama provera da li je ovo ispravan mali potez (figura dijagonalno jedno polje).
    if(!hasToJump && (start_x - end_x == 1 || start_x - end_x == -1))
    {
        if(end_y - start_y == player)
            validSmallMove = true;
        if(cur->isKing() && (end_y - start_y == opposite_player))
            validSmallMove = true;
    }

    // Ako je ovo slucaj izvrsavamo zeljeni potez.
    if(validSmallMove)
    {
        cur->x = end_x;
        cur->y = end_y;
        // Podrazumeva se da potezom figura moze doci do ivica samo u slucaju da treba da postane dama.
        if(end_y == 0 || end_y == field_num - 1)
            cur->makeKing(); // Nije problem vise puta zvati makeKing() nad damom.

        player_turn = opposite_player;
        //return true;
    }


    // Provera da li je ovo ispravno preskakanje.
    bool validJump = false;
    /* isValidJump koristimo nepravilno ali nece doci do greske jer ne koristimo rezultat ako
     * uslov nije ispunjen. FIX: */
    if((start_x - end_x == 2 || start_x - end_x == -2) && isValidJump(cur, end_x, end_y))
    {
        if(end_y - start_y == player * 2)
            validJump = true;
        if(cur->isKing() && end_y - start_y == opposite_player * 2)
            validJump = true;
    }
    //std::cout << validJump << std::endl;

    if(validJump)
    {
        cur->x = end_x;
        cur->y = end_y;

        // Uklanjamo figuru na proseku pocetka i kraja.
        removePiece(pieceAt((start_x + end_x) / 2, (start_y + end_y) /2));

        if(!canJump(cur))
            player_turn = opposite_player;

        // ???: Sta ako figura postane kralj i onda ima mogucnost skoka? Mi radimo da ne skace vise.
        if(end_y == 0 || end_y == field_num - 1)
            cur->makeKing(); // Nije problem vise puta zvati makeKing() nad damom.
    }

    if(player_turn == player_black)
        move_display->setText("Na potezu je: Crni");
    else
        move_display->setText("Na potezu je: Beli");
    return false;
}

bool Board::makePixelMove(int start_x, int start_y, int end_x, int end_y)
{
    int sx, sy, ex, ey;
    pixelConvert(start_x, start_y, sx, sy);
    pixelConvert(end_x, end_y, ex, ey);
    return makeMove(sx, sy, ex, ey);
}

bool Board::isValidPieceSelection(int x, int y)
{
    Piece *cur = pieceAt(x, y);

    if(cur == nullptr || cur->player() != player_turn)
        return false;
    else
        return true;
}

bool Board::isValidPixelPieceSelection(int x, int y)
{
    int nx, ny;
    pixelConvert(x, y, nx, ny);
    return isValidPieceSelection(nx, ny);
}


