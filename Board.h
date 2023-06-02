#ifndef BOARD_H
#define BOARD_H

#include <QObject>

typedef enum {
    EMPTY_SQUARE,
    BLACK_DISK,
    WHITE_DISK
} BoardSquareState;

typedef enum {
    BLACK_PLAYER,
    WHITE_PLAYER
} PlayerColor;

typedef struct {
    unsigned int row;
    unsigned int column;
} Move;

class Board
{
    /**
     * @brief 8-by-8 matrix reprenting the board
     */
    std::vector<std::vector<BoardSquareState>> board = std::vector<std::vector<BoardSquareState>>(8, std::vector<BoardSquareState>(8, EMPTY_SQUARE));

    /**
     * @brief getValidMovesAroundDisk
     * @param row
     * @param column
     * @return
     */
    std::vector<Move> getValidMovesAroundDisk(int row, int column);

public:

    /**
     * @brief Board Constructur; constructs the starting board with 4 disks in the middle (2 White and 2 Black)
     */
    Board();

    /**
     * @brief calculates the valid moves that the given player can do
     * @param player: The color of the player whose valid moves are required
     * @return a vector of valid moves the given polayer can choose, an empty vector means no legal moves are available
     */
    std::vector<Move> getValidMoves(PlayerColor player);

    /**
     * @brief allows access to the board state
     * @return returns a constant reference to the 2D vector represnting the board
     */
    const std::vector<std::vector<BoardSquareState>>& getBoard() const;

    /**
     * @brief Mutates the board state according to passed move, if move is invalid, nothing happens
     * @param move: Move choosen from valid moves
     * @return return false if the passed move is invalid and true if it is
     */
    bool doMove(Move move);

};

#endif // BOARD_H
