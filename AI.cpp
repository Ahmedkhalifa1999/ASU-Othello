#include "AI.h"

double AI::getBoardScore(const Board &board)
{
    int cornerScore = AI::corners(board, player) * cornerWeight;
    int squareScore = AI::squareWeights(board, player) * squareWeight;

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

    int score = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (boardVector[row][col] == player) {
                score += squareWeights[row][col];
            } else if (boardVector[row][col] == getOpponent(player)) {
                score -= squareWeights[row][col];
            }
        }
    }
    return score;
}


void AI::setDifficulty(Difficulty difficulty)
{

}

unsigned int AI::getDepth()
{

}

void AI::setDepth(unsigned int depth)
{

}

bool AI::getAlphaBetaPruning()
{

}

void AI::setAlphaBetaPruning(bool alphaBetaPruning)
{

}

bool AI::getIterativeDeepening()
{

}

void AI::setIterativeDeepening(bool iterativeDeepening)
{

}

void AI::computeNextMove()
{
    Move bestMove;

    // construct tree

    //call minmax

    //get the best move

    //delete tree

    emit nextMoveComputed(bestMove);
}

void AI::treeConstruct(Node* currentNode,int depth){
    if(depth == 0)
        return;
    vector<Move> validMoves = currentNode->State.getValidMoves(currentNode->color);

    for(auto move: validMoves){
        Board StateCpy = currentNode->State;
        StateCpy.doMove(move,currentNode->color);
        currentNode->children.push_back(new Node(StateCpy,currentNode->color,0,0,currentNode->Score));
        treeConstruct(currentNode->children.back(),depth - 1);
    }
}

int AI::minimax(Node *currentNode,bool Max,int alpha,int beta,int depth){
    if(depth == 0)
        return currentNode->Score;

    if(Max)
    {
        int maxValue = -INFINITY;
        for(auto child: currentNode->children)
        {
            int eval = minimax(child,false,alpha,beta,depth-1);
            maxValue = max(maxValue,eval);
            alpha = max(alpha,eval);
            if(beta<=alpha){
                break;
            }
        }
        return maxValue;
    }
    else
    {
        int minValue = INFINITY;
        for(auto child: currentNode->children)
        {
            int eval = minimax(child,true,alpha,beta,depth-1);
            minValue = min(minValue,eval);
            beta = min(beta,eval);
            if(beta<=alpha){
                break;
            }
        }
        return minValue;
    }
}
