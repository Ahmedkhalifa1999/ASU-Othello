#include "AI.h"

int AI::getBoardScore(const Board &board)
{
    int cornerScore = AI::corners(board, player) * parameters.cornerWeight;
    int squareScore = AI::squareWeights(board, player) * parameters.squareWeight;

    return cornerScore + squareScore;
}

AI::AI(PlayerColor player, const Board& board)
    : QObject{nullptr}, player{player}, board{board}
{

}
int AI::corners(const Board& board, PlayerColor player) {
    int corners[4][2] = {{0, 0}, {0, 7}, {7, 0}, {7, 7}};
    int blackCorners = 0;
    int whiteCorners = 0;

    auto boardVector = board.getBoard();

    for (int i = 0; i < 4; i++) {
        int row = corners[i][0];
        int col = corners[i][1];
        if (boardVector[row][col] == BLACK_DISK) {
              blackCorners++;
        } else if (boardVector[row][col] == WHITE_DISK) {
        whiteCorners++;
    }
}
    if (player == BLACK_PLAYER) {
        return 100 * (blackCorners - whiteCorners)
            / (blackCorners + whiteCorners + 1);
    }
    else {
        return 100 * (whiteCorners - blackCorners)
            / (blackCorners + whiteCorners + 1);
    }
}

PlayerColor AI::getOpponent(PlayerColor player) {
    if (player == BLACK_PLAYER) {
        return WHITE_PLAYER;
    } else {
        return BLACK_PLAYER;
    }
}

int AI::squareWeights(const Board& board, PlayerColor player) {
    const int squareWeights[8][8] = {
        { 100, -20,  30,  10,  10,  30, -20, 100 },
        { -20, -50, -10,  -5,  -5, -10, -50, -20 },
        {  30, -10,  10,   5,   5,  10, -10,  30 },
        {  10,  -5,   5,   0,   0,   5,  -5,  10 },
        {  10,  -5,   5,   0,   0,   5,  -5,  10 },
        {  30, -10,  10,   5,   5,  10, -10,  30 },
        { -20, -50, -10,  -5,  -5, -10, -50, -20 },
        { 100, -20,  30,  10,  10,  30, -20, 100 }
    };

    auto boardVector = board.getBoard();

    BoardSquareState playerDisk = (player == WHITE_PLAYER)? WHITE_DISK:BLACK_DISK;
    BoardSquareState opponentDisk = (player == BLACK_PLAYER)? BLACK_DISK:WHITE_DISK;

    int score = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (boardVector[row][col] == playerDisk) {
                score += squareWeights[row][col];
            } else if (boardVector[row][col] == opponentDisk) {
                score -= squareWeights[row][col];
            }
        }
    }
    return score;
}


void AI::setDifficulty(Difficulty difficulty)
{
    switch(difficulty) {
    case EASY:
        parameters.depth = 1;
        break;
    case MEDIUM:
        parameters.depth = 2;
        break;
    case HARD:
        parameters.depth = 3;
        break;
    case UNBEATABLE:
        parameters.depth = 4;
        break;
    }
    parameters.cornerWeight = 1;
    parameters.squareWeight = 1;
}

void AI::deleteTree(Node* node)
{
    if (node == NULL) return;

    for(auto child: node->children)
    {
        deleteTree(child);
    }

    delete node;
}

void AI::computeNextMove()
{
    QElapsedTimer timer;
    timer.start();

    // construct tree
    //Board State, PlayerColor color, int row, int column, int alpha, int beta, double Score
    Node* parentNode = new Node(board,board.getCurrentPlayer(),0,0, INT_MIN, INT_MAX, 0);
    //Node* parentNode = new Node();
    treeConstruct(parentNode,parameters.depth);
    //call minmax
    minimax(parentNode,true,parameters.depth);
    //get the best move
    Move nextMove = {-1, -1};
    int bestScore = parentNode->children[0]->Score;
    for(auto child : parentNode->children)
    {
        if(child->Score >= bestScore)
     	{
     	   bestScore = child->Score;
           nextMove = child->moveDone;
     	}   
    }
    //delete tree
    deleteTree(parentNode);

    int elapsed = timer.elapsed();

    if (elapsed < 1000) {
        QThread::msleep(1000-elapsed);
    }

    emit nextMoveComputed(nextMove);
}

void AI::treeConstruct(Node* currentNode,int depth){
    if(depth == 0)
        return;
    vector<Move> validMoves = currentNode->State.getValidMoves(currentNode->color);

    for(auto move: validMoves){
        Board StateCpy = currentNode->State;
        StateCpy.doMove(move,currentNode->color);
        currentNode->children.push_back(new Node(StateCpy,currentNode->color,move.row,move.column,INT_MIN,INT_MAX,currentNode->Score));
        //Board State, PlayerColor color, int row, int column, int alpha, int beta, double Score
        treeConstruct(currentNode->children.back(),depth - 1);
    }
}

int AI::minimax(Node *currentNode,bool Max,int depth){
    if(depth == 0)
        return getBoardScore(currentNode->State);

    if(Max)
    {
        int maxValue = INT_MIN;
        for(auto child: currentNode->children)
        {
            int eval = minimax(child,false,depth-1);
            maxValue = max(maxValue,eval);
            currentNode->alpha = max(currentNode->alpha,eval);
            currentNode->Score = max(currentNode->alpha,eval);
            if(currentNode->beta<=currentNode->alpha){
                break;
            }
        }
        return maxValue;
    }
    else
    {
        int minValue = INT_MAX;
        for(auto child: currentNode->children)
        {
            int eval = minimax(child,true,depth-1);
            minValue = min(minValue,eval);
            currentNode->beta = min(currentNode->beta,eval);
            currentNode->Score = min(currentNode->beta,eval);
            if(currentNode->beta<=currentNode->alpha){
                break;
            }
        }
        return minValue;
    }
}
