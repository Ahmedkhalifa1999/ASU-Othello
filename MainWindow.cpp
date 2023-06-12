#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), whiteAI{WHITE_PLAYER, mainBoard}, blackAI{BLACK_PLAYER, mainBoard}, boardWidget{mainBoard}
{
    QObject::connect(this, &MainWindow::whiteComputeNextMove, &whiteAI, &AI::computeNextMove);
    QObject::connect(this, &MainWindow::blackComputeNextMove, &blackAI, &AI::computeNextMove);
    QObject::connect(&whiteAI, &AI::nextMoveComputed, this, &MainWindow::whiteNextMoveComputed);
    QObject::connect(&blackAI, &AI::nextMoveComputed, this, &MainWindow::blackNextMoveComputed);

    //Widgets
    whitePlayerSettingsDockWidget.setParent(this);
    blackPlayerSettingsDockWidget.setParent(this);
    gameControlScoreDockWidget.setParent(this);
    boardWidget.setParent(this);
    this->addDockWidget(Qt::LeftDockWidgetArea, &whitePlayerSettingsDockWidget);
    this->addDockWidget(Qt::RightDockWidgetArea, &blackPlayerSettingsDockWidget);
    this->addDockWidget(Qt::BottomDockWidgetArea, &gameControlScoreDockWidget);
    this->setCentralWidget(&boardWidget);

    whitePlayerSettingsDockWidget.setFeatures(QDockWidget::NoDockWidgetFeatures);
    blackPlayerSettingsDockWidget.setFeatures(QDockWidget::NoDockWidgetFeatures);
    gameControlScoreDockWidget.setFeatures(QDockWidget::NoDockWidgetFeatures);


    whitePlayerSettingsDockWidget.setWindowTitle("White Player");
    blackPlayerSettingsDockWidget.setWindowTitle("Black Player");
    gameControlScoreDockWidget.setWindowTitle("Score and Game Control");
    boardWidget.setWindowTitle("Game Window");

    QWidget *whitePlayerSettingsWidget = new QWidget(&whitePlayerSettingsDockWidget);
    QWidget *blackPlayerSettingsWidget = new QWidget(&blackPlayerSettingsDockWidget);
    QWidget *gameControlScoreWidget = new QWidget(&gameControlScoreDockWidget);

    whitePlayerSettingsDockWidget.setWidget(whitePlayerSettingsWidget);
    blackPlayerSettingsDockWidget.setWidget(blackPlayerSettingsWidget);
    gameControlScoreDockWidget.setWidget(gameControlScoreWidget);

    QVBoxLayout *whitePlayerSettingsWidgetLayout = new QVBoxLayout(whitePlayerSettingsWidget);
    QVBoxLayout *blackPlayerSettingsWidgetLayout = new QVBoxLayout(blackPlayerSettingsWidget);
    QHBoxLayout *gameControlScoreWidgetLayout = new QHBoxLayout(gameControlScoreWidget);

    whitePlayerSettingsWidget->setLayout(whitePlayerSettingsWidgetLayout);
    blackPlayerSettingsWidget->setLayout(blackPlayerSettingsWidgetLayout);
    gameControlScoreWidget->setLayout(gameControlScoreWidgetLayout);

    QRadioButton *whiteAIRadioButton = new QRadioButton("AI", whitePlayerSettingsWidget);
    QRadioButton *whiteHumanRadioButton = new QRadioButton("Human", whitePlayerSettingsWidget);
    QRadioButton *blackAIRadioButton = new QRadioButton("AI", blackPlayerSettingsWidget);
    QRadioButton *blackHumanRadioButton = new QRadioButton("Human", blackPlayerSettingsWidget);

    QButtonGroup *whitePlayerSelectionButtonGroup = new QButtonGroup(whitePlayerSettingsWidget);
    QButtonGroup *blackPlayerSelectionButtonGroup = new QButtonGroup(blackPlayerSettingsWidget);

    whitePlayerSelectionButtonGroup->addButton(whiteAIRadioButton);
    whitePlayerSelectionButtonGroup->addButton(whiteHumanRadioButton);

    blackPlayerSelectionButtonGroup->addButton(blackAIRadioButton);
    blackPlayerSelectionButtonGroup->addButton(blackHumanRadioButton);

    whitePlayerSettingsWidgetLayout->addWidget(whiteAIRadioButton);
    whitePlayerSettingsWidgetLayout->addWidget(whiteHumanRadioButton);

    blackPlayerSettingsWidgetLayout->addWidget(blackAIRadioButton);
    blackPlayerSettingsWidgetLayout->addWidget(blackHumanRadioButton);

    whiteHumanRadioButton->setChecked(true);
    blackHumanRadioButton->setChecked(true);

    QObject::connect(whitePlayerSelectionButtonGroup, &QButtonGroup::buttonClicked, this, &MainWindow::whitePlayerSelectionButtonClicked);
    QObject::connect(blackPlayerSelectionButtonGroup, &QButtonGroup::buttonClicked, this, &MainWindow::blackPlayerSelectionButtonClicked);

    QHBoxLayout *whiteAIDepthLayout = new QHBoxLayout();
    QHBoxLayout *blackAIDepthLayout = new QHBoxLayout();

    whiteAIDepthLayout->addWidget(new QLabel("Depth", whitePlayerSettingsWidget));
    QSpinBox *whiteAIDepthSpinBox = new QSpinBox(whitePlayerSettingsWidget);
    whiteAIDepthSpinBox->setMinimum(1);
    whiteAIDepthSpinBox->setValue(whiteAI.parameters.depth);
    whiteAIDepthLayout->addWidget(whiteAIDepthSpinBox);

    blackAIDepthLayout->addWidget(new QLabel("Depth", blackPlayerSettingsWidget));
    QSpinBox *blackAIDepthSpinBox = new QSpinBox(blackPlayerSettingsWidget);
    blackAIDepthSpinBox->setMinimum(1);
    blackAIDepthSpinBox->setValue(whiteAI.parameters.depth);
    blackAIDepthLayout->addWidget(blackAIDepthSpinBox);

    QObject::connect(whiteAIDepthSpinBox, &QSpinBox::valueChanged, this, &MainWindow::whiteAIDepthValueChanged);
    QObject::connect(blackAIDepthSpinBox, &QSpinBox::valueChanged, this, &MainWindow::blackAIDepthValueChanged);

    whitePlayerSettingsWidgetLayout->addLayout(whiteAIDepthLayout);
    blackPlayerSettingsWidgetLayout->addLayout(blackAIDepthLayout);

    QVBoxLayout *whitePlayerScoreLayout = new QVBoxLayout();
    QVBoxLayout *blackPlayerScoreLayout = new QVBoxLayout();
    QPushButton *restartGameButton = new QPushButton("Restart Game", gameControlScoreWidget);

    QObject::connect(restartGameButton, &QPushButton::clicked, this, &MainWindow::restartButtonClicked);

    gameControlScoreWidgetLayout->addLayout(whitePlayerScoreLayout);
    gameControlScoreWidgetLayout->addWidget(restartGameButton);
    gameControlScoreWidgetLayout->addLayout(blackPlayerScoreLayout);

    whiteScore.setText("White Player: 0");
    blackScore.setText("Black Score: 0");

    whitePlayerScoreLayout->addWidget(&whiteScore);
    blackPlayerScoreLayout->addWidget(&blackScore);

    QObject::connect(&boardWidget, &BoardWidget::humanPlayed, this, &MainWindow::humanPlayed);
}

void MainWindow::whiteNextMoveComputed(Move move)
{
    mainBoard.doMove(move, WHITE_PLAYER);
    whiteScore.setText("White Score: " + QString::number(mainBoard.countDisks(WHITE_DISK)));
    blackScore.setText("Black Score: " + QString::number(mainBoard.countDisks(BLACK_DISK)));
    boardWidget.update();
    if (isBlackAI) {
        emit blackComputeNextMove();
    }
}

void MainWindow::blackNextMoveComputed(Move move)
{
    mainBoard.doMove(move, BLACK_PLAYER);
    whiteScore.setText("White Score: " + QString::number(mainBoard.countDisks(WHITE_DISK)));
    blackScore.setText("Black Score: " + QString::number(mainBoard.countDisks(BLACK_DISK)));
    boardWidget.update();
    if (isWhiteAI) {
        emit whiteComputeNextMove();
    }
}

void MainWindow::whitePlayerSelectionButtonClicked(QAbstractButton *button)
{
    if (button->text() == "AI") {
        isWhiteAI = true;
    }
    else if (button->text() == "Human") {
        isWhiteAI = false;
    }
}

void MainWindow::blackPlayerSelectionButtonClicked(QAbstractButton *button)
{
    if (button->text() == "AI") {
        isBlackAI = true;
    }
    else if (button->text() == "Human") {
        isBlackAI = false;
    }
}

void MainWindow::whiteAIDepthValueChanged(int depth)
{
    whiteAI.parameters.depth = depth;
}

void MainWindow::blackAIDepthValueChanged(int depth)
{
    blackAI.parameters.depth = depth;
}

void MainWindow::restartButtonClicked(bool checked)
{
    Q_UNUSED(checked)
    mainBoard = Board();

    whiteScore.setText("White Score: " + QString::number(mainBoard.countDisks(WHITE_DISK)));
    blackScore.setText("Black Score: " + QString::number(mainBoard.countDisks(BLACK_DISK)));
    boardWidget.update();
}

void MainWindow::humanPlayed(Move move)
{
    PlayerColor currentPlayer = mainBoard.getCurrentPlayer();

    mainBoard.doMove(move, currentPlayer);

    whiteScore.setText("White Score: " + QString::number(mainBoard.countDisks(WHITE_DISK)));
    blackScore.setText("Black Score: " + QString::number(mainBoard.countDisks(BLACK_DISK)));
    boardWidget.update();

    PlayerColor nextPlayer = (currentPlayer == WHITE_PLAYER)? BLACK_PLAYER:WHITE_PLAYER;

    if (nextPlayer == WHITE_PLAYER && isWhiteAI) {
        emit whiteComputeNextMove();
    }
    else if (nextPlayer == BLACK_PLAYER && isBlackAI) {
        emit blackComputeNextMove();
    }
}



