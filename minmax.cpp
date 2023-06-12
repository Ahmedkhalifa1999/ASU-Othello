
#include "minimax.h"
using namespace std;


void treeConstruct (Node* currentNode,int depth){
	if(depth == 0)
		return; 
	vector<Move> validMoves = currentNode->State.getValidMoves(currentNode->color);
	
	for(auto Move: validMoves){
		Board StateCpy = currentNode->State;
		StateCpy.doMove(Move,currentNode->color);
        currentNode->children.push_back(new Node(StateCpy,currentNode->color,0,0,currentNode->Score));
		treeConstruct(currentNode->children.back(),depth - 1);
	}
}

int minimax (Node* currentNode,bool Max,int depth){
    if(depth == 0)
        return currentNode->Score;

    if(Max)
    {
        int maxValue = -INFINITY;
        for(auto child: currentNode->children)
        {
            int eval = minimax(child,false,depth-1);
            maxValue = max(maxValue,eval);
            currentNode->Score = max(int(currentNode->Score),eval);
        }
        return maxValue;
    }
    else
    {
        int minValue = INFINITY;
        for(auto child: currentNode->children)
        {
            int eval = minimax(child,true,depth-1);
            minValue = min(minValue,eval);
            currentNode->Score = min(int(currentNode->Score),eval);
        }
        return minValue;
    }


}



int minimaxWithPruning (Node* currentNode,bool Max,int depth){
	if(depth == 0)
        return currentNode->Score;
	
	if(Max)
	{
        int maxValue = -INFINITY;
        for(auto child: currentNode->children)
		{
            int eval = minimax(child,false,depth-1);
			maxValue = max(maxValue,eval);
            currentNode->alpha = max(currentNode->alpha,eval);
            currentNode->Score = max(currentNode->alpha,eval);
            if(currentNode->beta<=currentNode->alpha){
				break;
			}
		}
		return maxValue;
	}
	else
	{
        int minValue = INFINITY;
        for(auto child: currentNode->children)
		{
            int eval = minimax(child,true,depth-1);
			minValue = min(minValue,eval);
            currentNode->beta = min(currentNode->beta,eval);
            currentNode->Score = min(currentNode->beta,eval);
            if(currentNode->beta<=currentNode->alpha){
				break;
			}
		}
		return minValue;
	}
}


