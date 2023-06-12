#include <vector>
#include <Board.h>
using namespace std;
class Node{
    public:
        /**
        * @brief board
        */
        Board State;
        /**
        * @brief player color
        */
        PlayerColor color;
        /**
        * @brief tree children
        */
        vector<Node*> children;
        /**
        * @brief move of the node
        */
        Move moveDone;
        /**
        * @brief alpha and beta values
        */
        int alpha = INT_MIN;
        int beta = INT_MAX;
        /**
        * @brief score of the node
        */
        double Score;

        /**
         * @brief node default constructor
         */
        Node();

        /**
         * @brief node constructor with parameters
         * @param board
         * @param player color
         * @param the node position of column and row
         * @param alpha and beta values
         * @param score of the tree
         * @return
         */
        Node(Board State, PlayerColor color, int row, int column, int alpha, int beta, double Score);
}; 
