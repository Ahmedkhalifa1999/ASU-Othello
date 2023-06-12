#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), whiteAI{WHITE_PLAYER, mainBoard}, blackAI{BLACK_PLAYER, mainBoard}, boardWidget{mainBoard}
{
    //AI Objects
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

    QComboBox *whiteDifficultyComboBox = new QComboBox(whitePlayerSettingsWidget);
    QComboBox *blackDifficultyComboBox = new QComboBox(blackPlayerSettingsWidget);

    whiteDifficultyComboBox->addItems({"Easy", "Medium", "Hard", "Unbeatable"});
    blackDifficultyComboBox->addItems({"Easy", "Medium", "Hard", "Unbeatable"});

    whitePlayerSettingsWidgetLayout->addWidget(whiteDifficultyComboBox);
    blackPlayerSettingsWidgetLayout->addWidget(blackDifficultyComboBox);

    QObject::connect(whiteDifficultyComboBox, &QComboBox::currentTextChanged, this, &MainWindow::whiteAIDifficulyChanged);
    QObject::connect(blackDifficultyComboBox, &QComboBox::currentTextChanged, this, &MainWindow::blackAIDifficulyChanged);

    whiteDifficultyComboBox->setCurrentText("Easy");
    blackDifficultyComboBox->setCurrentText("Easy");

    whiteAI.setDifficulty(EASY);
    blackAI.setDifficulty(EASY);

    QHBoxLayout *whiteAIDepthLayout = new QHBoxLayout();
    QHBoxLayout *blackAIDepthLayout = new QHBoxLayout();

    whiteAIDepthLayout->addWidget(new QLabel("Depth", whitePlayerSettingsWidget));
    whiteAIDepthSpinBox.setParent(whitePlayerSettingsWidget);
    whiteAIDepthSpinBox.setMinimum(1);
    whiteAIDepthSpinBox.setValue(whiteAI.parameters.depth);
    whiteAIDepthLayout->addWidget(&whiteAIDepthSpinBox);

    blackAIDepthLayout->addWidget(new QLabel("Depth", blackPlayerSettingsWidget));
    blackAIDepthSpinBox.setParent(blackPlayerSettingsWidget);
    blackAIDepthSpinBox.setMinimum(1);
    blackAIDepthSpinBox.setValue(blackAI.parameters.depth);
    blackAIDepthLayout->addWidget(&blackAIDepthSpinBox);

    QObject::connect(&whiteAIDepthSpinBox, &QSpinBox::valueChanged, this, &MainWindow::whiteAIDepthValueChanged);
    QObject::connect(&blackAIDepthSpinBox, &QSpinBox::valueChanged, this, &MainWindow::blackAIDepthValueChanged);

    whitePlayerSettingsWidgetLayout->addLayout(whiteAIDepthLayout);
    blackPlayerSettingsWidgetLayout->addLayout(blackAIDepthLayout);

    QHBoxLayout *whiteAICornerWeightLayout = new QHBoxLayout();
    QHBoxLayout *blackAICornerWeightLayout = new QHBoxLayout();

    whiteAICornerWeightLayout->addWidget(new QLabel("Corner Weight", whitePlayerSettingsWidget));
    whiteAICornerWeightSpinBox.setParent(whitePlayerSettingsWidget);
    whiteAICornerWeightSpinBox.setMinimum(1);
    whiteAICornerWeightSpinBox.setValue(whiteAI.parameters.cornerWeight);
    whiteAICornerWeightLayout->addWidget(&whiteAICornerWeightSpinBox);

    blackAICornerWeightLayout->addWidget(new QLabel("Corner Weight", blackPlayerSettingsWidget));
    blackAICornerWeightSpinBox.setParent(blackPlayerSettingsWidget);
    blackAICornerWeightSpinBox.setMinimum(1);
    blackAICornerWeightSpinBox.setValue(blackAI.parameters.cornerWeight);
    blackAICornerWeightLayout->addWidget(&blackAICornerWeightSpinBox);

    QObject::connect(&whiteAICornerWeightSpinBox, &QSpinBox::valueChanged, this, &MainWindow::whiteAICornerWeightValueChanged);
    QObject::connect(&blackAICornerWeightSpinBox, &QSpinBox::valueChanged, this, &MainWindow::blackAICornerWeightValueChanged);

    whitePlayerSettingsWidgetLayout->addLayout(whiteAICornerWeightLayout);
    blackPlayerSettingsWidgetLayout->addLayout(blackAICornerWeightLayout);

    QHBoxLayout *whiteAISquareWeightLayout = new QHBoxLayout();
    QHBoxLayout *blackAISquareWeightLayout = new QHBoxLayout();

    whiteAISquareWeightLayout->addWidget(new QLabel("Square Weight", whitePlayerSettingsWidget));
    whiteAISquareWeightSpinBox.setParent(whitePlayerSettingsWidget);
    whiteAISquareWeightSpinBox.setMinimum(1);
    whiteAISquareWeightSpinBox.setValue(whiteAI.parameters.squareWeight);
    whiteAISquareWeightLayout->addWidget(&whiteAISquareWeightSpinBox);

    blackAISquareWeightLayout->addWidget(new QLabel("Square Weight", blackPlayerSettingsWidget));
    blackAISquareWeightSpinBox.setParent(blackPlayerSettingsWidget);
    blackAISquareWeightSpinBox.setMinimum(1);
    blackAISquareWeightSpinBox.setValue(blackAI.parameters.squareWeight);
    blackAISquareWeightLayout->addWidget(&blackAISquareWeightSpinBox);

    QObject::connect(&whiteAISquareWeightSpinBox, &QSpinBox::valueChanged, this, &MainWindow::whiteAISquareWeightValueChanged);
    QObject::connect(&blackAISquareWeightSpinBox, &QSpinBox::valueChanged, this, &MainWindow::blackAISquareWeightValueChanged);

    whitePlayerSettingsWidgetLayout->addLayout(whiteAISquareWeightLayout);
    blackPlayerSettingsWidgetLayout->addLayout(blackAISquareWeightLayout);

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
    if(!mainBoard.doMove(move, WHITE_PLAYER)) {
        qDebug() << "White AI: Invalid Move";
    }
    whiteScore.setText("White Score: " + QString::number(mainBoard.countDisks(WHITE_DISK)));
    blackScore.setText("Black Score: " + QString::number(mainBoard.countDisks(BLACK_DISK)));
    boardWidget.update();
    if (isBlackAI) {
        emit blackComputeNextMove();
    }
}

void MainWindow::blackNextMoveComputed(Move move)
{
    if(!mainBoard.doMove(move, BLACK_PLAYER)) {
        qDebug() << "White AI: Invalid Move";
    }
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

void MainWindow::whiteAIDifficulyChanged(QString difficulty)
{
    if (difficulty == "Easy") {
        whiteAI.setDifficulty(EASY);
    }
    else if (difficulty == "Medium") {
        whiteAI.setDifficulty(MEDIUM);
    }
    else if (difficulty == "Hard") {
        whiteAI.setDifficulty(HARD);
    }
    else if (difficulty == "Unbeatable") {
        whiteAI.setDifficulty(UNBEATABLE);
    }
    whiteAIDepthSpinBox.setValue(whiteAI.parameters.depth);
    whiteAICornerWeightSpinBox.setValue(whiteAI.parameters.cornerWeight);
    whiteAISquareWeightSpinBox.setValue(whiteAI.parameters.squareWeight);
}

void MainWindow::blackAIDifficulyChanged(QString difficulty)
{
    if (difficulty == "Easy") {
        blackAI.setDifficulty(EASY);
    }
    else if (difficulty == "Medium") {
        blackAI.setDifficulty(MEDIUM);
    }
    else if (difficulty == "Hard") {
        blackAI.setDifficulty(HARD);
    }
    else if (difficulty == "Unbeatable") {
        blackAI.setDifficulty(UNBEATABLE);
    }
    blackAIDepthSpinBox.setValue(blackAI.parameters.depth);
    blackAICornerWeightSpinBox.setValue(blackAI.parameters.cornerWeight);
    blackAISquareWeightSpinBox.setValue(blackAI.parameters.squareWeight);
}

void MainWindow::whiteAIDepthValueChanged(int depth)
{
    whiteAI.parameters.depth = depth;
}

void MainWindow::blackAIDepthValueChanged(int depth)
{
    blackAI.parameters.depth = depth;
}

void MainWindow::whiteAICornerWeightValueChanged(int cornerWeight)
{
    whiteAI.parameters.cornerWeight = cornerWeight;
}

void MainWindow::blackAICornerWeightValueChanged(int cornerWeight)
{
    blackAI.parameters.cornerWeight = cornerWeight;
}

void MainWindow::whiteAISquareWeightValueChanged(int squareWeight)
{
    whiteAI.parameters.squareWeight = squareWeight;
}

void MainWindow::blackAISquareWeightValueChanged(int squareWeight)
{
    blackAI.parameters.squareWeight = squareWeight;
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

    if ((currentPlayer == WHITE_PLAYER && !isWhiteAI) || ((currentPlayer == BLACK_PLAYER) && !isBlackAI)) {
        mainBoard.doMove(move, currentPlayer);

        whiteScore.setText("White Score: " + QString::number(mainBoard.countDisks(WHITE_DISK)));
        blackScore.setText("Black Score: " + QString::number(mainBoard.countDisks(BLACK_DISK)));
        boardWidget.repaint();
    }

    PlayerColor nextPlayer = mainBoard.getCurrentPlayer();

    if (nextPlayer == WHITE_PLAYER && isWhiteAI) {
        emit whiteComputeNextMove();
    }
    else if (nextPlayer == BLACK_PLAYER && isBlackAI) {
        emit blackComputeNextMove();
    }
}



