#ifndef AI_H
#define AI_H

#include <QObject>
#include <QThread>

#include <Board.h>
#include <Node.h>

typedef struct AlgorithmParameters {
    int depth = 3;
    int cornerWeight = 1;
    int squareWeight = 1;
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
     * @brief calculates the score for the passed board using heuristics
     * @param board the board for calcualting score
     * @return the score for the passed board
     */
    int getBoardScore(const Board &board);

    /**
     * @brief corners
     * @param board
     * @param player
     * @return
     */
    PlayerColor getOpponent(PlayerColor player);

    /**
     * @brief corners
     * @param board
     * @param player
     * @return
     */
    int corners(const Board& board, PlayerColor player);

    /**
     * @brief corners
     * @param board
     * @param player
     * @return
     */
    int squareWeights(const Board& board, PlayerColor player);

    /**
     * @brief function used to construct a tree
     * @param node for constructing the tree
     * @param the depth of the tree
     * @return
     */
    void treeConstruct(Node* currentNode,int depth);

    /**
     * @brief function that do the minimax with alpha-beta pruning
     * @param node
     * @param boolean tells working with maximizer or minimizer
     * @param the depth of the tree
     * @return the score of the best move
     */
    int minimax(Node *currentNode,bool Max,int depth);


    /**
     * @brief function to delete the constructed tree
     * @param tree to be deleted
     */
    void deleteTree(Node* node);

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

    /**
     * @brief search algorith parameters
     */
    AlgorithmParameters parameters;

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
