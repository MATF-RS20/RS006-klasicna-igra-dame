#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLocalMultiplayer_clicked();

    void on_btn1Back_clicked();

    void on_btnVsComputer_clicked();

    void on_btn2Back_clicked();

    void on_btn1NewGame_clicked();

    void on_btn2NewGame_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
