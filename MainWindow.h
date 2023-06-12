#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>

#include <AI.h>
#include <Board.h>
#include <BoardWidget.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Board mainBoard;

    QThread AI_Thread;

    AI whiteAI;

    AI blackAI;

    bool isWhiteAI = false;

    bool isBlackAI = false;

    QDockWidget whitePlayerSettingsDockWidget;

    QDockWidget blackPlayerSettingsDockWidget;

    QDockWidget gameControlScoreDockWidget;

    BoardWidget boardWidget;

    QLabel whiteScore;

    QLabel blackScore;

public:
    MainWindow(QWidget *parent = nullptr);

public slots:
    void whiteNextMoveComputed(Move move);

    void blackNextMoveComputed(Move move);

    void whitePlayerSelectionButtonClicked(QAbstractButton *button);

    void blackPlayerSelectionButtonClicked(QAbstractButton *button);

    void whiteAIDepthValueChanged(int i);

    void blackAIDepthValueChanged(int i);

//    void whiteAIAlphaBetaPruningStateChanged(int state);

//    void blackAIAlphaBetaPruningStateChanged(int state);

//    void whiteAIIterativeDeepeningStateChanged(int state);

//    void blackAIIterativeDeepeningStateChanged(int state);

    void restartButtonClicked(bool checked);

    void humanPlayed(Move move);

signals:
    void whiteComputeNextMove();

    void blackComputeNextMove();
};
#endif // MAINWINDOW_H
