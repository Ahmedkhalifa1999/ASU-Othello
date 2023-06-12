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
        int alpha = -INFINITY;
        int beta = INFINITY;
        double Score;

        Node(Board State,PlayerColor color,int alpha,int beta,double Score);
}; 
