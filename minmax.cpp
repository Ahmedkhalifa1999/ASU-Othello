#include <minmax.h>
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
		int maxValue = -INFINITY;
		for(auto child: currentNode.children)
		{
			int eval = minimax(currentNode.children,false,alpha,beta,depth-1);
			maxValue = max(maxValue,eval);
			alpha = max(alpha,eval);
			if(beta<=alpha){
				break;
			}
		}
		return maxValue;
	}
	else
	{
		int minValue = INFINITY;
		for(auto child: currentNode.children)
		{
			int eval = minimax(currentNode.children,true,alpha,beta,depth-1);
			minValue = min(minValue,eval);
			beta = min(beta,eval);
			if(beta<=alpha){
				break;
			}
		}
		return minValue;
	}


}
