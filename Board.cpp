#include "Board.h"

Board::Board()
{
    //Initialize with 4 disks (2 white & 2 black)
    board[3][3] = WHITE_DISK;
    board[3][4] = BLACK_DISK;
    board[4][3] = BLACK_DISK;
    board[4][4] = WHITE_DISK;
}

std::vector<Move> Board::getValidMoves(PlayerColor player)
{

}

const std::vector<std::vector<BoardSquareState>> &Board::getBoard() const
{
    return board;
}

bool Board::doMove(Move move)
{

}
