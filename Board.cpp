#include "Board.h"

std::vector<Move> Board::getValidMovesAroundDisk(unsigned int row, unsigned int column)
{
    std::vector<Move> validMoves;

    const BoardSquareState opponentDisk = board[row][column];
    const BoardSquareState playerDisk = (opponentDisk == BLACK_DISK)? WHITE_DISK:BLACK_DISK;

    //Upper-Left
    if (row > 0 && column > 0 && board[row - 1][column - 1] == EMPTY_SQUARE) {
        for (int offset = 1; row + offset < 8 && column + offset < 8; offset++) {
            if (board[row + offset][column + offset] == EMPTY_SQUARE) break;
            if (board[row + offset][column + offset] == opponentDisk) {
                continue;
            }
            if (board[row + offset][column + offset] == playerDisk) {
                validMoves.push_back({row - 1, column - 1});
                break;
            }
        }
    }

    //Upper
    if (row > 0 && board[row - 1][column] == EMPTY_SQUARE) {
        for (int offset = 1; row + offset < 8; offset++) {
            if (board[row + offset][column] == EMPTY_SQUARE) break;
            if (board[row + offset][column] == opponentDisk) {
                continue;
            }
            if (board[row + offset][column] == playerDisk) {
                validMoves.push_back({row - 1, column});
                break;
            }
        }
    }

    //Upper-Right
    if (row > 0 && column < 7 && board[row - 1][column + 1] == EMPTY_SQUARE) {
        for (int offset = 1; row + offset < 8 && column - offset >= 0; offset++) {
            if (board[row + offset][column - offset] == EMPTY_SQUARE) break;
            if (board[row + offset][column - offset] == opponentDisk) {
                continue;
            }
            if (board[row + offset][column - offset] == playerDisk) {
                validMoves.push_back({row - 1, column + 1});
                break;
            }
        }
    }

    //Left
    if (column > 0 && board[row][column - 1] == EMPTY_SQUARE) {
        for (int offset = 1; column + offset < 8; offset++) {
            if (board[row][column + offset] == EMPTY_SQUARE) break;
            if (board[row][column + offset] == opponentDisk) {
                continue;
            }
            if (board[row][column + offset] == playerDisk) {
                validMoves.push_back({row, column - 1});
                break;
            }
        }
    }

    //Right
    if (column < 7 && board[row][column + 1] == EMPTY_SQUARE) {
        for (int offset = 1; column - offset >= 0; offset++) {
            if (board[row][column - offset] == EMPTY_SQUARE) break;
            if (board[row][column - offset] == opponentDisk) {
                continue;
            }
            if (board[row][column - offset] == playerDisk) {
                validMoves.push_back({row, column + 1});
                break;
            }
        }
    }

    //Lower-Left
    if (row < 7 && column > 0 && board[row + 1][column - 1] == EMPTY_SQUARE) {
        for (int offset = 1; row - offset >= 0 && column + offset < 8; offset++) {
            if (board[row - offset][column + offset] == EMPTY_SQUARE) break;
            if (board[row - offset][column + offset] == opponentDisk) {
                continue;
            }
            if (board[row - offset][column + offset] == playerDisk) {
                validMoves.push_back({row + 1, column - 1});
                break;
            }
        }
    }

    //Lower
    if (row < 7 && board[row + 1][column] == EMPTY_SQUARE) {
        for (int offset = 1; row - offset >= 0; offset++) {
            if (board[row - offset][column] == EMPTY_SQUARE) break;
            if (board[row - offset][column] == opponentDisk) {
                continue;
            }
            if (board[row - offset][column] == playerDisk) {
                validMoves.push_back({row + 1, column});
                break;
            }
        }
    }

    //Lower-Right
    if (row < 7 && column < 7 && board[row + 1][column + 1] == EMPTY_SQUARE) {
        for (int offset = 1; row - offset >= 0 && column - offset >= 0; offset++) {
            if (board[row - offset][column - offset] == EMPTY_SQUARE) break;
            if (board[row - offset][column - offset] == opponentDisk) {
                continue;
            }
            if (board[row - offset][column - offset] == playerDisk) {
                validMoves.push_back({row + 1, column + 1});
                break;
            }
        }
    }

    return validMoves;
}

bool Board::isValidMove(unsigned int row, unsigned int column, PlayerColor player)
{
    return false;
}

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
    BoardSquareState targetDisk = (player == BLACK_PLAYER)? WHITE_DISK:BLACK_DISK;

    std::vector<Move> validMoves;

    for (unsigned int i = 0; i < 8; i++) {
        for (unsigned int j = 0; j < 8; j++) {
//            if (isValidMove(i, j, player)) {
//                validMoves.push_back({i, j});
//            }

            if (board[i][j] == targetDisk) {
                std::vector<Move> validMovesAroundDisk = getValidMovesAroundDisk(i, j);
                validMoves.insert(validMoves.end(), validMovesAroundDisk.begin(), validMovesAroundDisk.end());
            }
        }
    }

    return validMoves;
}

const std::vector<std::vector<BoardSquareState>> &Board::getBoard() const
{
    return board;
}

bool Board::doMove(Move move, PlayerColor player)
{
    std::vector<Move> validMoves = getValidMoves(player);

    bool valid = false;

    for (auto const &validMove: validMoves) {
        if (validMove.row == move.row && validMove.column == move.column) {
            valid = true;
            break;
        }
    }

    const BoardSquareState playerDisk = (player == BLACK_PLAYER)? BLACK_DISK:WHITE_DISK;
    const BoardSquareState opponentDisk = (player == BLACK_PLAYER)? WHITE_DISK:BLACK_DISK;

    if (valid) {
        unsigned int row = move.row;
        unsigned int column = move.column;

        board[row][column] = playerDisk;

        bool flip;

        //Up
        flip = false;
        for (int offset = 1; row - offset < 8; offset++) {
            if (board[row - offset][column] == EMPTY_SQUARE) break;
            if (board[row - offset][column] == opponentDisk) {
                continue;
            }
            if (board[row - offset][column] == playerDisk) {
                flip = true;
                break;
            }
        }
        if (flip) {
            for (int offset = 1; row - offset < 8; offset++) {
                if (board[row - offset][column] == EMPTY_SQUARE) break;
                if (board[row - offset][column] == opponentDisk) {
                    board[row - offset][column] = playerDisk;
                    continue;
                }
                if (board[row - offset][column] == playerDisk) {
                    break;
                }
            }
        }

        //Up-Right
        flip = false;
        for (int offset = 1; row - offset < 8 && column + offset < 8; offset++) {
            if (board[row - offset][column + offset] == EMPTY_SQUARE) break;
            if (board[row - offset][column + offset] == opponentDisk) {
                continue;
            }
            if (board[row - offset][column + offset] == playerDisk) {
                flip = true;
                break;
            }
        }
        if (flip) {
            for (int offset = 1; row - offset < 8 && column + offset < 8; offset++) {
                if (board[row - offset][column + offset] == EMPTY_SQUARE) break;
                if (board[row - offset][column + offset] == opponentDisk) {
                    board[row - offset][column + offset] = playerDisk;
                    continue;
                }
                if (board[row - offset][column + offset] == playerDisk) {
                    break;
                }
            }
        }

        //Right
        flip = false;
        for (int offset = 1; column + offset < 8; offset++) {
            if (board[row][column + offset] == EMPTY_SQUARE) break;
            if (board[row][column + offset] == opponentDisk) {
                continue;
            }
            if (board[row][column + offset] == playerDisk) {
                flip = true;
                break;
            }
        }
        if (flip) {
            for (int offset = 1; column + offset < 8; offset++) {
                if (board[row][column + offset] == EMPTY_SQUARE) break;
                if (board[row][column + offset] == opponentDisk) {
                    board[row][column + offset] = playerDisk;
                    continue;
                }
                if (board[row][column + offset] == playerDisk) {
                    break;
                }
            }
        }

        //Down-Right
        flip = false;
        for (int offset = 1; row + offset < 8 && column + offset < 8; offset++) {
            if (board[row + offset][column + offset] == EMPTY_SQUARE) break;
            if (board[row + offset][column + offset] == opponentDisk) {
                continue;
            }
            if (board[row + offset][column + offset] == playerDisk) {
                flip = true;
                break;
            }
        }
        if (flip) {
            for (int offset = 1; row + offset < 8 && column + offset < 8; offset++) {
                if (board[row + offset][column + offset] == EMPTY_SQUARE) break;
                if (board[row + offset][column + offset] == opponentDisk) {
                    board[row + offset][column + offset] = playerDisk;
                    continue;
                }
                if (board[row + offset][column + offset] == playerDisk) {
                    break;
                }
            }
        }

        //Down
        flip = false;
        for (int offset = 1; row + offset < 8; offset++) {
            if (board[row + offset][column] == EMPTY_SQUARE) break;
            if (board[row + offset][column] == opponentDisk) {
                continue;
            }
            if (board[row + offset][column] == playerDisk) {
                flip = true;
                break;
            }
        }
        if (flip) {
            for (int offset = 1; row + offset < 8; offset++) {
                if (board[row + offset][column] == EMPTY_SQUARE) break;
                if (board[row + offset][column] == opponentDisk) {
                    board[row + offset][column] = playerDisk;
                    continue;
                }
                if (board[row + offset][column] == playerDisk) {
                    break;
                }
            }
        }

        //Down-Left
        flip = false;
        for (int offset = 1; row + offset < 8 && column - offset < 8; offset++) {
            if (board[row + offset][column - offset] == EMPTY_SQUARE) break;
            if (board[row + offset][column - offset] == opponentDisk) {
                continue;
            }
            if (board[row + offset][column - offset] == playerDisk) {
                flip = true;
                break;
            }
        }
        if (flip) {
            for (int offset = 1; row + offset < 8 && column - offset < 8; offset++) {
                if (board[row + offset][column - offset] == EMPTY_SQUARE) break;
                if (board[row + offset][column - offset] == opponentDisk) {
                    board[row + offset][column - offset] = playerDisk;
                    continue;
                }
                if (board[row + offset][column - offset] == playerDisk) {
                    break;
                }
            }
        }

        //Left
        flip = false;
        for (int offset = 1; column - offset < 8; offset++) {
            if (board[row][column - offset] == EMPTY_SQUARE) break;
            if (board[row][column - offset] == opponentDisk) {
                continue;
            }
            if (board[row][column - offset] == playerDisk) {
                flip = true;
                break;
            }
        }
        if (flip) {
            for (int offset = 1; column - offset < 8; offset++) {
                if (board[row][column - offset] == EMPTY_SQUARE) break;
                if (board[row][column - offset] == opponentDisk) {
                    board[row][column - offset] = playerDisk;
                    continue;
                }
                if (board[row][column - offset] == playerDisk) {
                    break;
                }
            }
        }

        //Up-Left
        flip = false;
        for (int offset = 1; row - offset < 8 && column - offset < 8; offset++) {
            if (board[row - offset][column - offset] == EMPTY_SQUARE) break;
            if (board[row - offset][column - offset] == opponentDisk) {
                continue;
            }
            if (board[row - offset][column - offset] == playerDisk) {
                flip = true;
                break;
            }
        }
        if (flip) {
            for (int offset = 1; row + offset < 8 && column - offset < 8; offset++) {
                if (board[row - offset][column - offset] == EMPTY_SQUARE) break;
                if (board[row - offset][column - offset] == opponentDisk) {
                    board[row - offset][column - offset] = playerDisk;
                    continue;
                }
                if (board[row - offset][column - offset] == playerDisk) {
                    break;
                }
            }
        }
    }

    return valid;
}
