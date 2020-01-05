#include "board.h"
void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        auto x = static_cast<int>(round(event->scenePos().x()));
        auto y = static_cast<int>(round(event->scenePos().y()));

        /* Ova podesavanja su da ulepsaju koristenje biranja figure i pravljenja
         * poteza, tj. DWIM. */
        //  ???: Dodatno testirati da li reaguje svaki put ispravno.
        bool not_move = true;
        if(_selected)
        {

            not_move = !(_game_state->makePixelMove(_selected_x, _selected_y, x, y));
            _game_state->show();
            _selected = false;

        }

        if(not_move && (_game_state->isValidPixelPieceSelection(x, y)))
        {
            _selected = true;
            _selected_x = x;
            _selected_y = y;
        }
    }
}

void BoardScene::reset()
{
    _selected = false;
}

void VsComputerBoardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        auto x = static_cast<int>(round(event->scenePos().x()));
        auto y = static_cast<int>(round(event->scenePos().y()));

        /* Ova podesavanja su da ulepsaju koristenje biranja figure i pravljenja
         * poteza, tj. DWIM. */
        //  ???: Dodatno testirati da li reaguje svaki put ispravno.
        bool not_move = true;
        if(_selected)
        {

            not_move = !(_game_state->makePixelMove(_selected_x, _selected_y, x, y));
            _game_state->minimax();
            _game_state->changeTurn();
            _game_state->show();
            _selected = false;
        }

        if(not_move && (_game_state->isValidPixelPieceSelection(x, y)))
        {
            _selected = true;
            _selected_x = x;
            _selected_y = y;
        }
    }
}

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
    : display(_display), turn_display(_move_display),  size(_size)
{
    // Predpostavljamo da je scena kvadrat.
    field_size = size / field_num;
}

void Board::set()
{
    // Resetovati stanje table za slucaj da je pokrenuta nova igra.
    while(piece_count > 0)
        removePiece(pieces[0]);
    player_turn = player_black;
    piece_in_use = nullptr;
    turn_display->setText("Na potezu je: Crni");
    display->reset();

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

    this->show();
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

    // ???: Cini mi se da border crne figure nije iste sirine svuda. M.D.
    // Prikazivanje figura igre. Ovo je zavisno od velicine scene.
    int padding = 5, king_padding = 6;
    // ???: Smanjujemo za jedan velicinu jer tako izgleda simetricno.
    int piece_size = field_size - padding * 2 - 1;
    int crown_size = piece_size - king_padding * 2 + 1;
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

        if(cur->isKing())
        {
            QGraphicsEllipseItem* crown =
                    new QGraphicsEllipseItem(field_size * cur->x + padding + king_padding,
                                             field_size * cur->y + padding + king_padding,
                                             crown_size, crown_size);

            if(cur->player() == player_white)
                crown->setBrush(Qt::gray);
            else
                crown->setBrush(Qt::red);

            crown->setPen(Qt::NoPen);
            display->addItem(crown);
        }
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
    bool made_move = false;

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
    if(!hasToJump && (start_x - end_x == 1 || start_x - end_x == -1)
            && isEmptyField(end_x, end_y))
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
        made_move = true;
    }


    // Provera da li je ovo ispravno preskakanje.
    bool validJump = false;
    /* isValidJump koristimo nepravilno ali nece doci do greske jer ne koristimo rezultat ako
     * uslov nije ispunjen. FIX: */
    if((start_x - end_x == 2 || start_x - end_x == -2) && isValidJump(cur, end_x, end_y)
            && (piece_in_use == nullptr || piece_in_use == cur))
    {
        if(end_y - start_y == player * 2)
            validJump = true;
        if(cur->isKing() && end_y - start_y == opposite_player * 2)
            validJump = true;
    }

    if(validJump)
    {
        cur->x = end_x;
        cur->y = end_y;

        // Uklanjamo figuru na proseku pocetka i kraja.
        removePiece(pieceAt((start_x + end_x) / 2, (start_y + end_y) /2));

        if(!canJump(cur))
        {
            player_turn = opposite_player;
            piece_in_use = nullptr;
        }
        else
            piece_in_use = cur;

        // ???: Sta ako figura postane kralj i onda ima mogucnost skoka? Mi radimo da ne skace vise.
        if(end_y == 0 || end_y == field_num - 1)
            cur->makeKing(); // Nije problem vise puta zvati makeKing() nad damom.

        made_move = true;
    }

    if(player_turn == player_black)
        turn_display->setText("Na potezu je: Crni");
    else
        turn_display->setText("Na potezu je: Beli");

    return made_move;
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

VsComputerBoard::VsComputerBoard(BoardScene* _display, QLabel* _move_display, int _size)
    : Board(_display, _move_display, _size)
{}
//generisemo int matricu koja odgovara tabeli na ekranu
void Board::generateBoardArray(){
    Piece* temp;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            temp=pieceAt(j,i);
            if(temp==nullptr) board_array[i][j]=0;
            else{
                if(temp->player()==1){
                    if(temp->isKing()) board_array[i][j]=2;
                    else board_array[i][j]=1;
                }
                else{
                    if(temp->isKing()) board_array[i][j]=-2;
                    else board_array[i][j]=-1;
                }
            }
        }
    }
}
//mozemo li skociti gore levo
bool Board::canJumpUpLeft(int board[][8],int i,int j){
    if(i>1 && j>1){
        if(!(board[i-2][j-2]==0)) return false;
        int k=board[i][j];
        if(k>0 && board[i-1][j-1]<0) return true;
        else if(k<0 && board[i-1][j-1]>0) return true;
    }
    return false;
}
//mozemo li skociti dole levo
bool Board::canJumpBottomLeft(int board[][8],int i,int j){
    if(i<6 && j>1){
        if(!(board[i+2][j-2]==0)) return false;
        int k=board[i][j];
        if(k>0 && board[i+1][j-1]<0) return true;
        else if(k<0 && board[i+1][j-1]>0) return true;
    }
    return false;
}
//mozemo li skociti gore desno
bool Board::canJumpUpRight(int board[][8],int i,int j){
    if(i>1 && j<6){
        if(!(board[i-2][j+2]==0)) return false;
        int k=board[i][j];
        if(k>0 && board[i-1][j+1]<0) return true;
        else if(k<0 && board[i-1][j+1]>0) return true;
    }
    return false;
}
//mozemo li skociti dole desno
bool Board::canJumpBottomRight(int board[][8],int i,int j){
    if(i<6 && j<6){
        if(!(board[i+2][j+2]==0)) return false;
        int k=board[i][j];
        if(k>0 && board[i+1][j+1]<0) return true;
        else if(k<0 && board[i+1][j+1]>0) return true;
    }
    return false;
}
//ako ima mogucih pomeranja iz tacke (x,y) za racunar, izvrsi ih i evaluiraj dublje
void Board::movemax(int board[][8],int* best_score,int x,int y,int depth){
    int i,j,k,l;
    int temp[8][8],optimal[8][8];
    int min_int=-1000,max_int=1000;
    int score=max_int,bestscore=min_int;
    bool has_to_jump;
    //ako smo odradili dovoljno poteza, sracunaj stanje igre i vrati se
    if(depth>5){
        score=0;
        //vidi da li je racunar izgubio
        for(i=0;i<8;i++) for(j=0;j<8;j++) if(board[i][j]>0) break;
        if(i==8 && j==8){
            *best_score=min_int+1;
            return;
        }
        //da li je pobedio
        for(i=0;i<8;i++) for(j=0;j<8;j++) if(board[i][j]<0) break;
        if(i==8 && j==8){
            *best_score=max_int-1;
            return;
        }
        //saberi, +-1 za obicne i +-2 za kraljeve
        for(i=0;i<8;i++) for(j=0;j<8;j++) score+=board[i][j];
        if(score>*best_score) *best_score=score;
        return;
    }
    //da li je na ovoj lokaciji racunarska figura
    if(board[x][y]>0){
        //moze li dole levo
        if(x<7 && y>1) if(board[x+1][y-1]==0){
            //generisi temp tabelu kao da je taj potez odradjen
            for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
            temp[x+1][y-1]=temp[x][y];temp[x][y]=0;
            //da li je stigao dole, za krunisanje
            if(x+1==7) temp[x+1][y-1]=2;
            has_to_jump=false;
            score=max_int;
            //pogledaj sve moguce poteze od igraca,vidi koji daje najmanju min vrednost
            for(i=0;i<8;i++) for(j=0;j<8;j++){
                jumpmin(temp,&score,i,j,depth+1);
            }
            if(score<max_int) has_to_jump=true;
            if(!has_to_jump){
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    movemin(temp,&score,i,j,depth+1);
                }
            }
            if(score>bestscore){
                bestscore=score;
                for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
            }
        }
        //moze li dole desno
        if(x<7 && y<7) if(board[x+1][y+1]==0){
            //generisi temp tabelu kao da je taj potez odradjen
            for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
            temp[x+1][y+1]=temp[x][y];temp[x][y]=0;
            //da li je stigao dole, za krunisanje
            if(x+1==7) temp[x+1][y+1]=2;
            has_to_jump=false;
            score=max_int;
             //pogledaj sve moguce poteze od igraca,vidi koji daje najvecu min vrednost
            for(i=0;i<8;i++) for(j=0;j<8;j++){
                jumpmin(temp,&score,i,j,depth+1);
            }
            if(score<max_int) has_to_jump=true;
            if(!has_to_jump){
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    movemin(temp,&score,i,j,depth+1);
                }
            }
            if(score>bestscore){
                bestscore=score;
                for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
            }
        }
        //da li je krunisan
        if(board[x][y]==2){
            //gore levo
            if(x>0 && y>0) if(board[x-1][y-1]==0){
                //generisi kopiju s tim potezom
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y-1]=temp[x][y];temp[x][y]=0;
                has_to_jump=false;
                score=max_int;
                //vidi sve moguce igraceve poteze
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmin(temp,&score,i,j,depth+1);
                }
                if(score<max_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemin(temp,&score,i,j,depth+1);
                    }
                }
                if(score>bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
            //gore desno
            if(x>0 && y<7) if(board[x-1][y+1]==0){
                //generisi kopiju
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y+1]=temp[x][y];temp[x][y]=0;
                has_to_jump=false;
                score=max_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmin(temp,&score,i,j,depth+1);
                }
                if(score<max_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemin(temp,&score,i,j,depth+1);
                    }
                }
                if(score>bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
        }
    }
    //ako nema validnih poteza
    else return;
    //za potrebe debagovanja, ispisi najbolji potez iz tog stanja
    if(bestscore>min_int){
        for(i=0; i<8; i++){
            for(j=0; j<8; j++){
                std::cout<<" "<<optimal[i][j]<<" ";
            }
            std::cout<<"\n";
        }
    }
    //ako je rezultat bolji od postojeceg, prenesi u materinsku f-ju zajedno sa stanjem nakon poteza
    if(bestscore>*best_score){
        *best_score=bestscore;
        for(i=0;i<8;i++) for(j=0;j<8;j++) board[i][j]=optimal[i][j];
    }
}
void Board::movemin(int board[][8],int* best_score,int x,int y,int depth){
    int i,j,k,l;
    int temp[8][8],optimal[8][8];
    int max_int=1000,min_int=-1000;
    int score=min_int,bestscore=max_int;
    bool has_to_jump;
    //ako ima figura na polju
    if(board[x][y]<0){
        if(board[x][y]==-2){
            if(x<7 && y>0) if(board[x+1][y-1]==0){
                //dole levo
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x+1][y-1]=temp[x][y];temp[x][y]=0;
                if(x+1==7) temp[x+1][y-1]=-2;
                has_to_jump=false;
                score=min_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmax(temp,&score,i,j,depth+1);
                }
                if(score>min_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemax(temp,&score,i,j,depth+1);
                    }
                }
                if(score<bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
            //dole desno
            if(x<7 && y<7) if(board[x+1][y+1]==0){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x+1][y+1]=temp[x][y];temp[x][y]=0;
                if(x+1==7) temp[x+1][y+1]=-2;
                has_to_jump=false;
                score=max_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmax(temp,&score,i,j,depth+1);
                }
                if(score>min_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemax(temp,&score,i,j,depth+1);
                    }
                }
                if(score<bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
        }
            //gore levo
            if(x>0 && y>0) if(board[x-1][y-1]==0){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y-1]=temp[x][y];temp[x][y]=0;
                if(x-1==0) temp[x-1][y-1]=-2;
                has_to_jump=false;
                score=min_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmax(temp,&score,i,j,depth+1);
                }
                if(score>min_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemax(temp,&score,i,j,depth+1);
                    }
                }
                if(score<bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
            //gore desno
            if(x>0 && y<7) if(board[x-1][y+1]==0){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y+1]=temp[x][y];temp[x][y]=0;
                if(x-1==0) temp[x-1][y+1]=-2;
                score=min_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmax(temp,&score,i,j,depth+1);
                }
                if(score>min_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemax(temp,&score,i,j,depth+1);
                    }
                }
                if(score<bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
    }
    else return;
    if(bestscore<max_int){
        for(i=0; i<8; i++){
            for(j=0; j<8; j++){
                std::cout<<" "<<optimal[i][j]<<" ";
            }
            std::cout<<"\n";
        }
    }
    if(bestscore<*best_score) *best_score=bestscore;
}
void Board::jumpmin(int board[][8],int* best_score,int x,int y,int depth){
    int i,j,k,l;
    int temp[8][8],optimal[8][8];
    int max_int=1000,min_int=-1000;
    int score=min_int,bestscore=max_int;
    bool has_to_jump;
    if(board[x][y]<0){
        if(board[x][y]==-2){
            if(canJumpBottomLeft(board,x,y)){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x+1][y-1]=0; temp[x+2][y-2]=temp[x][y]; temp[x][y]=0;
                jumpmin(temp,&score,x+2,y-2,depth);
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x+1][y-1]=0; temp[x+2][y-2]=temp[i][j]; temp[x][y]=0;
                has_to_jump=false;
                score=min_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmax(temp,&score,i,j,depth+1);
                }
                if(score>min_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemax(temp,&score,i,j,depth+1);
                    }
                }
                if(score<bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
            if(canJumpBottomRight(board,x,y)){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x+1][y+1]=0; temp[x+2][y+2]=temp[x][y]; temp[x][y]=0;
                if(x+2==7) temp[x+2][y+2]=-2;
                jumpmin(temp,&score,x+2,y+2,depth);
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x+1][y+1]=0; temp[x+2][y+2]=temp[i][j]; temp[x][y]=0;
                if(x+2==7) temp[x+2][y+2]=-2;
                has_to_jump=false;
                score=min_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmax(temp,&score,i,j,depth+1);
                }
                if(score>min_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemax(temp,&score,i,j,depth+1);
                    }
                }
                if(score<bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
        }
            if(canJumpUpLeft(board,x,y)){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y-1]=0; temp[x-2][y-2]=temp[x][y]; temp[x][y]=0;
                if(x-2==0) temp[x-2][y-2]=-2;
                jumpmin(temp,&score,x+2,y+2,depth);
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y-1]=0; temp[x-2][y-2]=temp[i][j]; temp[x][y]=0;
                if(x-2==0) temp[x-2][y-2]=-2;
                has_to_jump=false;
                score=min_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmax(temp,&score,i,j,depth+1);
                }
                if(score>min_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemax(temp,&score,i,j,depth+1);
                    }
                }
                if(score<bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
            if(canJumpUpRight(board,x,y)){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y+1]=0; temp[x-2][y+2]=temp[x][y]; temp[x][y]=0;
                if(x-2==0) temp[x-2][y+2]=-2;
                jumpmin(temp,&score,x+2,y+2,depth);
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y+1]=0; temp[x-2][y+2]=temp[i][j]; temp[x][y]=0;
                if(x-2==0) temp[x-2][y+2]=-2;
                has_to_jump=false;
                score=min_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmax(temp,&score,i,j,depth+1);
                }
                if(score>min_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemax(temp,&score,i,j,depth+1);
                    }
                }
                if(score<bestscore){
                    bestscore=score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp[k][l];
                }
            }
    }
    else return;
    if(bestscore<max_int){
        for(i=0; i<8; i++){
            for(j=0; j<8; j++){
                std::cout<<" "<<optimal[i][j]<<" ";
            }
            std::cout<<"\n";
        }
    }
    if(bestscore<*best_score) *best_score=bestscore;
}
void Board::jumpmax(int board[][8],int* best_score,int x,int y,int depth){
    int i,j;
    int temp[8][8],optimal[8][8];
    int min_int=-1000,max_int=1000;
    int score=max_int,bestscore=min_int;
    bool has_to_jump;
    if(depth>5){
        score=0;
        //vidi da li je racunar izgubio
        for(i=0;i<8;i++) for(j=0;j<8;j++) if(board[i][j]>0) break;
        if(i==8 && j==8){
            *best_score=min_int+1;
            return;
        }
        //da li je pobedio
        for(i=0;i<8;i++) for(j=0;j<8;j++) if(board[i][j]<0) break;
        if(i==8 && j==8){
            *best_score=max_int-1;
            return;
        }
        //saberi, +-1 za obicne i +-2 za kraljeve
        for(i=0;i<8;i++) for(j=0;j<8;j++) score+=board[i][j];
        if(score>*best_score) *best_score=score;
        return;
    }
    if(board[x][y]>0){
        if(canJumpBottomLeft(board,x,y)){
            for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
            temp[x+1][y-1]=0; temp[x+2][y-2]=temp[x][y]; temp[x][y]=0;
            if(x+2==7) temp[x+2][y-2]=2;
            jumpmax(temp,&score,x+2,y-2,depth);
            for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
            temp[x+1][y-1]=0; temp[x+2][y-2]=temp[i][j]; temp[x][y]=0;
            if(x+2==7) temp[x+2][y-2]=2;
            has_to_jump=false;
            score=max_int;
            for(i=0;i<8;i++) for(j=0;j<8;j++){
                jumpmin(temp,&score,i,j,depth+1);
            }
            if(score<max_int) has_to_jump=true;
            if(!has_to_jump){
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    movemin(temp,&score,i,j,depth+1);
                }
            }
            if(score>bestscore){
                bestscore=score;
                for(i=0;i<8;i++) for(j=0;j<8;j++) optimal[i][j]=temp[i][j];
            }
        }
        if(canJumpBottomRight(board,x,y)){
            for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
            temp[x+1][y+1]=0; temp[x+2][y+2]=temp[x][y]; temp[x][y]=0;
            if(x+2==7) temp[x+2][y+2]=2;
            jumpmax(temp,&score,x+2,y+2,depth);
            if(score>bestscore){
                bestscore=score;
                for(i=0;i<8;i++) for(j=0;j<8;j++) optimal[i][j]=temp[i][j];
            }
            for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
            temp[x+1][y+1]=0; temp[x+2][y+2]=temp[i][j]; temp[x][y]=0;
            if(x+2==7) temp[x+2][y+2]=2;
            has_to_jump=false;
            score=max_int;
            for(i=0;i<8;i++) for(j=0;j<8;j++){
                jumpmin(temp,&score,i,j,depth+1);
            }
            if(score<max_int) has_to_jump=true;
            if(!has_to_jump){
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    movemin(temp,&score,i,j,depth+1);
                }
            }
            if(score>bestscore){
                bestscore=score;
                for(i=0;i<8;i++) for(j=0;j<8;j++) optimal[i][j]=temp[i][j];
            }
        }
        if(board[x][y]==2){
            if(canJumpUpLeft(board,x,y)){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y-1]=0; temp[x-2][y-2]=temp[x][y]; temp[x][y]=0;
                if(x-2==0) temp[x-2][y-2]=2;
                jumpmax(temp,&score,x+2,y+2,depth);
                if(score>bestscore){
                    bestscore=score;
                    for(i=0;i<8;i++) for(j=0;j<8;j++) optimal[i][j]=temp[i][j];
                }
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y-1]=0; temp[x-2][y-2]=temp[i][j]; temp[x][y]=0;
                if(x-2==0) temp[x-2][y-2]=2;
                has_to_jump=false;
                score=max_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmin(temp,&score,i,j,depth+1);
                }
                if(score<max_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemin(temp,&score,i,j,depth+1);
                    }
                }
                if(score>bestscore){
                    bestscore=score;
                    for(i=0;i<8;i++) for(j=0;j<8;j++) optimal[i][j]=temp[i][j];
                }
            }
            if(canJumpUpRight(board,x,y)){
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y+1]=0; temp[x-2][y+2]=temp[x][y]; temp[x][y]=0;
                if(x-2==0) temp[x-2][y+2]=2;
                jumpmax(temp,&score,x+2,y+2,depth);
                if(score>bestscore){
                    bestscore=score;
                    for(i=0;i<8;i++) for(j=0;j<8;j++) optimal[i][j]=temp[i][j];
                }
                for(i=0;i<8;i++) for(j=0;j<8;j++) temp[i][j]=board[i][j];
                temp[x-1][y+1]=0; temp[x-2][y+2]=temp[i][j]; temp[x][y]=0;
                if(x-2==0) temp[x-2][y+2]=2;
                has_to_jump=false;
                score=max_int;
                for(i=0;i<8;i++) for(j=0;j<8;j++){
                    jumpmin(temp,&score,i,j,depth+1);
                }
                if(score<max_int) has_to_jump=true;
                if(!has_to_jump){
                    for(i=0;i<8;i++) for(j=0;j<8;j++){
                        movemin(temp,&score,i,j,depth+1);
                    }
                }
                if(score>bestscore){
                    bestscore=score;
                    for(i=0;i<8;i++) for(j=0;j<8;j++) optimal[i][j]=temp[i][j];
                }
            }
        }
    }
    else return;
    if(bestscore>min_int){
        for(i=0; i<8; i++){
            for(j=0; j<8; j++){
                std::cout<<" "<<optimal[i][j]<<" ";
            }
            std::cout<<"\n";
        }
    }
    if(bestscore>*best_score){
        *best_score=bestscore;
        for(i=0;i<8;i++) for(j=0;j<8;j++) board[i][j]=optimal[i][j];
    }
}
//funkcija minimax algoritma
void Board::minimax(){
    //od trenutnog stanja generisi matricu nad kojom ce se raditi
    generateBoardArray();
    bool has_to_jump=false;
    //par promenljivih za laksu obradu
    int i,j,k,l,temp_score=-1000,score=-1000;
    int temp_board[8][8],optimal[8][8];
    for(i=0;i<8;i++) for(j=0;j<8;j++) optimal[i][j]=0;
    //prvo gledamo moguce skokove pa ako nema pomeraje
    //za svako polje na tabeli
    for(i=0;i<8;i++){
        for(j=0;j<8;j++){
            //ucitaj u temp tabelu trenutno stanje igre
            for(k=0;k<8;k++) for(l=0;l<8;l++) temp_board[k][l]=board_array[k][l];
            //vidi ako racunar moze skok s tog polja
            jumpmax(temp_board,&temp_score,i,j,0);
            //u temp_score je upisan najbolji rezultat pronadjen ovako, ako najbolji upisi najbolji rezultat i stanje
            if(temp_score>score){
                has_to_jump=true;
                score=temp_score;
                for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp_board[k][l];
            }
        }
    }
    //ako nema opcija za skakanje, isto samo s pomeranjem
    if(!has_to_jump){
        for(i=0;i<8;i++){
            for(j=0;j<8;j++){
                for(k=0;k<8;k++) for(l=0;l<8;l++) temp_board[k][l]=board_array[k][l];
                movemax(temp_board,&temp_score,i,j,0);
                if(temp_score>score){
                    score=temp_score;
                    for(k=0;k<8;k++) for(l=0;l<8;l++) optimal[k][l]=temp_board[k][l];
                }
            }
        }
    }
    //rekonstruisi tablu na osnovu novodobijene matrice
    for(i=0;i<8;i++){
        for(j=0;j<8;j++){
            if(optimal[i][j]==0 && pieceAt(i,j)!=nullptr) removePiece(pieceAt(i,j));
            if(optimal[i][j]!=0 && pieceAt(i,j)==nullptr){
                if(optimal[i][j]>0){
                     pieces[piece_count++] = new Piece(player_white, j, i);
                     if(optimal[i][j]==2) pieces[piece_count]->makeKing();
                }
                if(optimal[i][j]<0){
                     pieces[piece_count++] = new Piece(player_black, j, i);
                     if(optimal[i][j]==-2) pieces[piece_count]->makeKing();
                }
            }
        }
    }
}
void Board::changeTurn(){
    player_turn=player_turn*-1;
}
