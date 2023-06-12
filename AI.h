#ifndef AI_H
#define AI_H

#include <QObject>

#include <Board.h>
#include <Node.h>

typedef struct {
    int depth;
    int cornerWeight;
    int squareWeight;
    bool alphaBetaPruning;
    bool iterativeDeepening;
    int timeSearchLimit;
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
     * @brief reference to the main board
     */
    const Board &board;

    /**
     * @brief search algorith parameters
     */
    AlgorithmParameters parameters;

    /**
     * @brief calculates the score for the passed board using heuristics
     * @param board: the board for calcualting score
     * @return the score for the passed board
     */
    int getBoardScore(const Board &board);

    int corners(const Board& board, PlayerColor player);

    PlayerColor getOpponent(PlayerColor player);

    int squareWeights(const Board& board, PlayerColor player);

    int cornerWeight;

    int squareWeight;

    void treeConstruct(Node* currentNode,int depth);


public:
    /**
     * @brief AI Constructor
     * @param player: AI player color
     * @param board: reference to the main board
     */
    AI(PlayerColor player, const Board &board);

    /**
     * @brief sets the difficulty level by using a pre-defined set of parameters for each level
     * @param desired difficulty level
     */
    void setDifficulty(Difficulty difficulty);

    /* Implement setters and getters for all the parameters */
    unsigned int getDepth();
    void setDepth(unsigned int depth);

    bool getAlphaBetaPruning();
    void setAlphaBetaPruning(bool alphaBetaPruning);

    bool getIterativeDeepening();
    void setIterativeDeepening(bool iterativeDeepening);

public slots:
    /**
     * @brief starts computing the next move
     */
    void computeNextMove();
    void deleteTree(Node* node);
    int minimax(Node *currentNode,bool Max,int depth);

signals:
    /**
     * @brief used to signal back that next move computation finished
     * @param next move
     */
    void nextMoveComputed(Move move);


};

#endif // AI_H
