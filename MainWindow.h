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

    AI whiteAI;

    AI blackAI;

    bool isWhiteAI = false;

    bool isBlackAI = false;

    BoardWidget boardWidget;

    QDockWidget whitePlayerSettingsDockWidget;

    QDockWidget blackPlayerSettingsDockWidget;

    QDockWidget gameControlScoreDockWidget;

    QLabel whiteScore;

    QLabel blackScore;

public:
    MainWindow(QWidget *parent = nullptr);

public slots:
    void whiteNextMoveComputed(Move move);

    void blackNextMoveComputed(Move move);

    void whitePlayerSelectionButtonClicked(QAbstractButton *button);

    void blackPlayerSelectionButtonClicked(QAbstractButton *button);

    void whiteAIDepthValueChanged(int depth);

    void blackAIDepthValueChanged(int depth);

    void whiteAICornerWeightValueChanged(int depth);

    void blackAICornerWeightValueChanged(int depth);

    void whiteAISquareWeightValueChanged(int depth);

    void blackAISquareWeightValueChanged(int depth);

    void restartButtonClicked(bool checked);

    void humanPlayed(Move move);

signals:
    void whiteComputeNextMove();

    void blackComputeNextMove();
};
#endif // MAINWINDOW_H
