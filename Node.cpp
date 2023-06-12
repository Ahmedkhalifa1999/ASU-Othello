#include "Node.h"

Node::Node(Board State, PlayerColor color, int alpha, int beta, double Score)
{

    this->State = State;
    this->color = color;
    this->alpha = alpha;
    this->beta = beta;
    this->Score = Score;
}
