#include "Node.h"

Node::Node()
{

}

Node::Node(Board State, PlayerColor color, int row, int column, int alpha, int beta, double Score)
{

    this->State = State;
    this->color = color;
    this->moveDone.row = row;
    this->moveDone.column = column;
    this->alpha = alpha;
    this->beta = beta;
    this->Score = Score;
}


