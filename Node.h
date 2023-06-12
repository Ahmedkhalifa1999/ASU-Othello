#include <vector>
#include <Board.h>
using namespace std;
class Node{
    public:
        Board State;
        int position;
        //bool isRoot;
        PlayerColor color;
        vector<Node*> children;
        Move moveDone;
        int alpha = INT_MIN;
        int beta = INT_MAX;
        double Score;

        Node();
        Node(Board State, PlayerColor color, int row, int column, int alpha, int beta, double Score);
}; 
