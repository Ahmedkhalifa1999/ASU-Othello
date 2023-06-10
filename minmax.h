#include <iostream>
#include <algorithm>
#include "Board.h"
#include "Node.h"
#include "AI.h"
#define ScoreStub 30

void treeConstruct(Node* currentNode,int depth);

Move minimax(Node currentNode,bool Max,int alpha,int beta,int depth);
