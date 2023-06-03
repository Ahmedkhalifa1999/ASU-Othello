#include <iostream>
#include <algorithm>
#include "Board.h"
#include "Node.hpp"
#include "AI.h"
#define ScoreStub 30
using namespace std;


void treeConstruct(Node* currentNode,int depth){
	if(depth == 0)
		return; 
	vector<Move> validMoves = currentNode->State.getValidMoves(currentNode->color);
	
	for(auto Move: validMoves){
		Board StateCpy = currentNode->State;
		StateCpy.doMove(Move,currentNode->color);
		currentNode->children.push_back(new Node(StateCpy,currentNode->color,0,0,ScoreStub));
		treeConstruct(currentNode->children.back(),depth - 1);
	}
}

Move minimax(Node currentNode,bool Max,int alpha,int beta,int depth){
	Move move;
	if(depth == 0)
		return move;
	
	if(Max)
	{
		
		
	}
	else
	{

	}


}
