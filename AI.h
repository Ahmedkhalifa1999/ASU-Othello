#ifndef AI_H
#define AI_H

#include <QObject>

#include <Board.h>

typedef struct {

} AlgorithmParameters;

typedef enum {
    EASY,
    MEDIUM,
    HARD,
    UNBEATABLE
} Difficulty;

class AI : public QObject
{
    Q_OBJECT

    /**
     * @brief AI player color
     */
    PlayerColor player;

    /**
     * @brief pointer to the main board
     */
    Board *board;

    /**
     * @brief search algorith parameters
     */
    AlgorithmParameters parameters;

public:
    /**
     * @brief AI Constructor
     * @param player: AI player color
     * @param board: pointer to the main board
     */
    AI(PlayerColor player, Board *board);

    /**
     * @brief sets the difficulty level by using a pre-defined set of parameters for each level
     * @param desired difficulty level
     */
    void setDifficulty(Difficulty difficulty);

    /* Implement setters and getters for all the parameters */

public slots:
    /**
     * @brief starts computing the next move
     */
    void computeNextMove();

signals:
    /**
     * @brief used to signal back that next move computation finished
     * @param next move
     */
    void nextMoveComputed(Move move);
};

#endif // AI_H
