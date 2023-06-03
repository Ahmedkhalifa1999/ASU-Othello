#include "AI.h"

double AI::getBoardScore(const Board &board)
{
    int cornerScore = corners(board, player) * cornerWeight;
    int squareScore = squareWeights(board, player) * squareWeight;

    return cornerScore + squareScore;
}

AI::AI(PlayerColor player, const Board& board)
    : QObject{nullptr}, player{player}, board{board}
{

}
int AI::corners(Board& board, PlayerColor player) {
    int corners[4][2] = {{0, 0}, {0, 7}, {7, 0}, {7, 7}};
    int blackCorners = 0;
    int whiteCorners = 0;

    for (int i = 0; i < 4; i++) {
        int row = corners[i][0];
        int col = corners[i][1];
        if (board[row][col] == BLACK_DISK) {
              blackCorners++;
        } else if (board[row][col] == WHITE_DISK) {
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

int AI::squareWeights(Board& board, PlayerColor player) {
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

    int score = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (board[row][col] == player) {
                score += squareWeights[row][col];
            } else if (board[row][col] == getOpponent(player)) {
                score -= squareWeights[row][col];
            }
        }
    }
    return score;
}


void AI::setDifficulty(Difficulty difficulty)
{

}

void AI::computeNextMove()
{

}
