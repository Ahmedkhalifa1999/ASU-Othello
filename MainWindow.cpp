#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), whiteAI{WHITE_PLAYER, mainBoard}, blackAI{BLACK_PLAYER, mainBoard}, boardWidget{mainBoard}
{
    //AI Objects
    whiteAI.moveToThread(&AI_Thread);
    blackAI.moveToThread(&AI_Thread);

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
    whiteAIDepthSpinBox->setValue(whiteAI.getDepth());
    whiteAIDepthLayout->addWidget(whiteAIDepthSpinBox);

    blackAIDepthLayout->addWidget(new QLabel("Depth", blackPlayerSettingsWidget));
    QSpinBox *blackAIDepthSpinBox = new QSpinBox(blackPlayerSettingsWidget);
    blackAIDepthSpinBox->setMinimum(1);
    blackAIDepthSpinBox->setValue(blackAI.getDepth());
    blackAIDepthLayout->addWidget(blackAIDepthSpinBox);

    QObject::connect(whiteAIDepthSpinBox, &QSpinBox::valueChanged, this, &MainWindow::whiteAIDepthValueChanged);
    QObject::connect(blackAIDepthSpinBox, &QSpinBox::valueChanged, this, &MainWindow::blackAIDepthValueChanged);

    whitePlayerSettingsWidgetLayout->addLayout(whiteAIDepthLayout);
    blackPlayerSettingsWidgetLayout->addLayout(blackAIDepthLayout);

    QCheckBox *whiteAIAlphaBetaPruningCheckBox = new QCheckBox("Alpha-Beta Pruning", whitePlayerSettingsWidget);
    QCheckBox *blackAIAlphaBetaPruningCheckBox = new QCheckBox("Alpha-Beta Pruning", blackPlayerSettingsWidget);

    whitePlayerSettingsWidgetLayout->addWidget(whiteAIAlphaBetaPruningCheckBox);
    blackPlayerSettingsWidgetLayout->addWidget(blackAIAlphaBetaPruningCheckBox);

    whiteAIAlphaBetaPruningCheckBox->setChecked(whiteAI.getAlphaBetaPruning());
    blackAIAlphaBetaPruningCheckBox->setChecked(blackAI.getAlphaBetaPruning());

    QObject::connect(whiteAIAlphaBetaPruningCheckBox, &QCheckBox::stateChanged, this, &MainWindow::whiteAIAlphaBetaPruningStateChanged);
    QObject::connect(blackAIAlphaBetaPruningCheckBox, &QCheckBox::stateChanged, this, &MainWindow::blackAIAlphaBetaPruningStateChanged);

    QCheckBox *whiteAIIterativeDeepeningCheckBox = new QCheckBox("Iterative Deepening", whitePlayerSettingsWidget);
    QCheckBox *blackAIIterativeDeepeningCheckBox = new QCheckBox("Iterative Deepening", blackPlayerSettingsWidget);

    whitePlayerSettingsWidgetLayout->addWidget(whiteAIIterativeDeepeningCheckBox);
    blackPlayerSettingsWidgetLayout->addWidget(blackAIIterativeDeepeningCheckBox);

    whiteAIIterativeDeepeningCheckBox->setChecked(whiteAI.getIterativeDeepening());
    blackAIIterativeDeepeningCheckBox->setChecked(blackAI.getIterativeDeepening());

    QObject::connect(whiteAIIterativeDeepeningCheckBox, &QCheckBox::stateChanged, this, &MainWindow::whiteAIIterativeDeepeningStateChanged);
    QObject::connect(blackAIIterativeDeepeningCheckBox, &QCheckBox::stateChanged, this, &MainWindow::blackAIIterativeDeepeningStateChanged);

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

void MainWindow::whiteAIDepthValueChanged(int i)
{
    whiteAI.setDepth(i);
}

void MainWindow::blackAIDepthValueChanged(int i)
{
    blackAI.setDepth(i);
}

void MainWindow::whiteAIAlphaBetaPruningStateChanged(int state)
{
    if(state == Qt::Unchecked) {
        whiteAI.setAlphaBetaPruning(false);
    }
    else if (state == Qt::Checked) {
        whiteAI.setAlphaBetaPruning(true);
    }
}

void MainWindow::blackAIAlphaBetaPruningStateChanged(int state)
{
    if(state == Qt::Unchecked) {
        blackAI.setAlphaBetaPruning(false);
    }
    else if (state == Qt::Checked) {
        blackAI.setAlphaBetaPruning(true);
    }
}

void MainWindow::whiteAIIterativeDeepeningStateChanged(int state)
{
    if(state == Qt::Unchecked) {
        whiteAI.setIterativeDeepening(false);
    }
    else if (state == Qt::Checked) {
        whiteAI.setIterativeDeepening(true);
    }
}

void MainWindow::blackAIIterativeDeepeningStateChanged(int state)
{
    if(state == Qt::Unchecked) {
        blackAI.setIterativeDeepening(false);
    }
    else if (state == Qt::Checked) {
        blackAI.setIterativeDeepening(true);
    }
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



