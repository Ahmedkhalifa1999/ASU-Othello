#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QThread>

#include <AI.h>
#include <Board.h>
#include <BoardWidget.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Board mainBoard;

    QThread AI_Thread;

    AI whiteAI;

    AI blackAI;

    QDockWidget whitePlayerSettings;

    QDockWidget blackPlayerSettings;

    BoardWidget boardWidget;

public:
    MainWindow(QWidget *parent = nullptr);

public slots:
    void whiteNextMoveComputed(Move move);

    void blackNextMoveComputed(Move move);

signals:
    void whiteComputeNextMove();

    void blackComputeNextMove();
};
#endif // MAINWINDOW_H
