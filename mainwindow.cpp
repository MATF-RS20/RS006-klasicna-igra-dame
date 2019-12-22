#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "board.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /* Preuzeto sa https://stackoverflow.com/questions/16348919/disable-resizing-of-a-form,
     * onemogucavamo menjanje velicine forme. */
    setFixedSize(this->size());

    localMPBoardScene = new BoardScene();
    ui->graphicsView->setScene(localMPBoardScene);
    localMPBoard = new Board(localMPBoardScene, ui->lbl1TurnDisplay, ui->graphicsView->width() - 1);
    localMPBoard->set();
    localMPBoard->show();
    localMPBoardScene->setBoard(localMPBoard);

    ui->graphicsView->show();

    /* Preuzeto sa https://www.qtcentre.org/threads/31778-How-to-Disable-Scroll-Bar-of-QGraphicsView-(or-QWidget),
     * uklanjamo nezeljene skrol-barove koji se pojavljuju iako je velicina scene dovoljna da prikaze sadrzaj. */
    ui->graphicsView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    ui->graphicsView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    // Potvrdjujemo da je prikazana glavna strana.
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_btnLocalMultiplayer_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    ui->graphicsView->show();
}

void MainWindow::on_btn1Back_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
