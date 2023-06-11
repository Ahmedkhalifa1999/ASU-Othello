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
        int alpha;
        int beta;
        double Score = 0;

        Node(Board State,PlayerColor color,int alpha,int beta,double Score);
}; 
