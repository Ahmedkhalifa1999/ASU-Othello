#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), whiteAI{WHITE_PLAYER, mainBoard}, blackAI{BLACK_PLAYER, mainBoard}, boardWidget{mainBoard}
{
    whiteAI.moveToThread(&AI_Thread);
    blackAI.moveToThread(&AI_Thread);

    QObject::connect(this, &MainWindow::whiteComputeNextMove, &whiteAI, &AI::computeNextMove);
    QObject::connect(this, &MainWindow::blackComputeNextMove, &blackAI, &AI::computeNextMove);
    QObject::connect(&whiteAI, &AI::nextMoveComputed, this, &MainWindow::whiteNextMoveComputed);
    QObject::connect(&blackAI, &AI::nextMoveComputed, this, &MainWindow::blackNextMoveComputed);
}

void MainWindow::whiteNextMoveComputed(Move move)
{
    mainBoard.doMove(move, WHITE_PLAYER);
}

void MainWindow::blackNextMoveComputed(Move move)
{
    mainBoard.doMove(move, BLACK_PLAYER);
}



