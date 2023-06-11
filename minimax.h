#ifndef MINIMAX_H
#define MINIMAX_H

#include "Node.h"
void treeConstruct (Node* currentNode,int depth);
int minimax (Node currentNode,bool Max,int alpha,int beta,int depth);

#endif // MINIMAX_H
