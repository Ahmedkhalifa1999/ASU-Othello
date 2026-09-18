// Review harness: drives the repo's AI/Board code directly. Private members are exposed
// via a #define so that the search internals can be inspected without editing the sources.
#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QElapsedTimer>
#include <QDebug>
#include <climits>
#include <vector>
#define private public
#include "AI.h"
#undef private
#include <cstdio>
#include <cstring>
#include <set>
#include <map>
#include <random>
#include <chrono>
#include <sys/resource.h>

static const char* pc(PlayerColor c){ return c==BLACK_PLAYER?"BLACK":"WHITE"; }
static PlayerColor opp(PlayerColor c){ return c==BLACK_PLAYER?WHITE_PLAYER:BLACK_PLAYER; }
static size_t countNodes(Node* n){ size_t c=1; for(auto ch:n->children) c+=countNodes(ch); return c; }
static std::set<std::pair<int,int>> uniqMoves(const std::vector<Move>& v){ std::set<std::pair<int,int>> s; for(auto&m:v) s.insert({m.row,m.column}); return s; }

// Exact replica of AI::computeNextMove's move selection (lines 120-137), minus the 1 s sleep.
struct Pick { Move move{-1,-1}; size_t nodes=0; bool noChildren=false; std::vector<std::pair<Move,int>> childScores; };
static Pick pick(AI& ai){
    const Board& b = ai.board;
    Node* root = new Node(b, b.getCurrentPlayer(), 0,0, INT_MIN, INT_MAX, 0);
    ai.treeConstruct(root, ai.parameters.depth);
    ai.minimax(root, true, ai.parameters.depth);
    Pick p; p.nodes = countNodes(root); p.noChildren = root->children.empty();
    if(!p.noChildren){
        int bestScore = root->children[0]->Score;
        for(auto child: root->children){
            p.childScores.push_back({child->moveDone, child->Score});
            if(child->Score >= bestScore){ bestScore = child->Score; p.move = child->moveDone; }
        }
    }
    ai.deleteTree(root);
    return p;
}

// Replica of AI::minimax with instrumentation, to measure how often the alpha-beta cutoff fires.
static long g_visits=0, g_cutoffs=0;
static int minimaxInstr(AI& ai, Node* n, bool Max, int depth){
    g_visits++;
    if(depth==0) return ai.getBoardScore(n->State);
    if(Max){ int maxValue=INT_MIN; for(auto ch:n->children){ int e=minimaxInstr(ai,ch,false,depth-1); maxValue=std::max(maxValue,e); n->alpha=std::max(n->alpha,e); n->Score=std::max(n->alpha,e); if(n->beta<=n->alpha){g_cutoffs++;break;} } return maxValue; }
    else   { int minValue=INT_MAX; for(auto ch:n->children){ int e=minimaxInstr(ai,ch,true,depth-1);  minValue=std::min(minValue,e); n->beta=std::min(n->beta,e);   n->Score=std::min(n->beta,e);   if(n->beta<=n->alpha){g_cutoffs++;break;} } return minValue; }
}

static void randomPlay(Board& b, std::mt19937& rng, int nMoves){
    PlayerColor p=BLACK_PLAYER;
    for(int i=0;i<nMoves;i++){
        auto u=uniqMoves(b.getValidMoves(p)); if(u.empty()){ p=opp(p); u=uniqMoves(b.getValidMoves(p)); if(u.empty()) return; b.currentPlayer=p; }
        std::vector<std::pair<int,int>> v(u.begin(),u.end()); auto m=v[rng()%v.size()]; b.doMove({m.first,m.second},p); p=opp(p);
    }
}

static void printBoard(const Board& b){
    for(int r=0;r<8;r++){ for(int c=0;c<8;c++){ auto s=b.getBoard()[r][c]; putchar(s==EMPTY_SQUARE?'.':s==BLACK_DISK?'B':'W'); } putchar('\n'); }
}


// ---- Reference: a textbook alternating-player negamax with alpha-beta and the AI's own heuristic, corrected. ----
static int refEval(Board& b, PlayerColor me){
    const int W[8][8]={{100,-20,30,10,10,30,-20,100},{-20,-50,-10,-5,-5,-10,-50,-20},{30,-10,10,5,5,10,-10,30},{10,-5,5,0,0,5,-5,10},{10,-5,5,0,0,5,-5,10},{30,-10,10,5,5,10,-10,30},{-20,-50,-10,-5,-5,-10,-50,-20},{100,-20,30,10,10,30,-20,100}};
    BoardSquareState my = me==BLACK_PLAYER?BLACK_DISK:WHITE_DISK, op = me==BLACK_PLAYER?WHITE_DISK:BLACK_DISK;
    int s=0; const auto& g=b.getBoard();
    for(int r=0;r<8;r++)for(int c=0;c<8;c++){ if(g[r][c]==my) s+=W[r][c]; else if(g[r][c]==op) s-=W[r][c]; }
    int mc=0,oc=0; for(auto [r,c]:{std::pair{0,0},std::pair{0,7},std::pair{7,0},std::pair{7,7}}){ if(g[r][c]==my) mc++; else if(g[r][c]==op) oc++; }
    return s + 100*(mc-oc)/(mc+oc+1);
}
static int refSearch(Board& b, PlayerColor toMove, PlayerColor me, int depth, int alpha, int beta){
    auto moves=uniqMoves(b.getValidMoves(toMove));
    if(depth==0) return refEval(b,me);
    if(moves.empty()){ if(uniqMoves(b.getValidMoves(opp(toMove))).empty()){ int d=b.countDisks(me==BLACK_PLAYER?BLACK_DISK:WHITE_DISK)-b.countDisks(me==BLACK_PLAYER?WHITE_DISK:BLACK_DISK); return d>0?100000:d<0?-100000:0; } return refSearch(b,opp(toMove),me,depth-1,alpha,beta); }
    bool maxi = toMove==me; int best = maxi?INT_MIN:INT_MAX;
    for(auto [r,c]:moves){ Board nb=b; nb.doMove({r,c},toMove); int v=refSearch(nb,opp(toMove),me,depth-1,alpha,beta);
        if(maxi){ best=std::max(best,v); alpha=std::max(alpha,v); } else { best=std::min(best,v); beta=std::min(beta,v); }
        if(beta<=alpha) break; }
    return best;
}
static Move refPick(Board& b, PlayerColor me, int depth){
    auto moves=uniqMoves(b.getValidMoves(me)); Move best{-1,-1}; int bestV=INT_MIN;
    for(auto [r,c]:moves){ Board nb=b; nb.doMove({r,c},me); int v=refSearch(nb,opp(me),me,depth-1,INT_MIN,INT_MAX); if(v>bestV){bestV=v;best={r,c};} }
    return best;
}
// Plays one game; mover functions receive the board and colour and return a move.
template<class FA,class FB> static int playGame(FA blackMover, FB whiteMover){
    Board b; PlayerColor p=BLACK_PLAYER;
    while(true){
        auto u=uniqMoves(b.getValidMoves(p));
        if(u.empty()){ if(uniqMoves(b.getValidMoves(opp(p))).empty()) break; p=opp(p); b.currentPlayer=p; continue; }
        Move m = p==BLACK_PLAYER? blackMover(b,p) : whiteMover(b,p);
        if(!b.doMove(m,p)){ printf("ILLEGAL MOVE by %s\n",pc(p)); return 0; }
        p=opp(p);
    }
    return b.countDisks(BLACK_DISK)-b.countDisks(WHITE_DISK); // >0 black wins
}

int main(int argc, char** argv){
    QCoreApplication app(argc, argv);
    std::mt19937 rng(7);


    if(argc>2 && !strcmp(argv[1],"cost")){
        int depth=atoi(argv[2]); std::mt19937 r(3); Board b; randomPlay(b,r,20);
        AI ai(b.getCurrentPlayer(),b); ai.parameters.depth=depth;
        auto t0=std::chrono::steady_clock::now(); Pick p=pick(ai);
        double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t0).count();
        struct rusage ru; getrusage(RUSAGE_SELF,&ru);
        printf("depth %d: %10zu nodes  %9.0f ms  peak RSS %6ld MB\n",depth,p.nodes,ms,ru.ru_maxrss/1024);
        return 0;
    }
    if(argc>1 && !strcmp(argv[1],"real")){
        // call the real slot a few times on a normal position (includes its 1 s sleep) so ASan can leak-check it
        std::mt19937 r(5); Board b; randomPlay(b,r,16); AI ai(b.getCurrentPlayer(),b); ai.setDifficulty(HARD);
        QObject::connect(&ai,&AI::nextMoveComputed,[&](Move m){ printf("real computeNextMove -> (%d,%d)\n",m.row,m.column); });
        for(int i=0;i<3;i++) ai.computeNextMove();
        return 0;
    }
    if(argc>1 && !strcmp(argv[1],"ref")){
        auto rnd=[&](Board& b,PlayerColor p){ auto u=uniqMoves(b.getValidMoves(p)); std::vector<std::pair<int,int>> v(u.begin(),u.end()); auto x=v[rng()%v.size()]; return Move{x.first,x.second}; };
        for(int d: {1,2,3}){
            int w=0,l=0,dr=0;
            for(int g=0; g<100; g++){
                auto refM=[&](Board& b,PlayerColor p){ return refPick(b,p,d); };
                int res = (g%2==0)? playGame(refM,rnd) : -playGame(rnd,refM);
                if(res>0) w++; else if(res<0) l++; else dr++;
            }
            printf("reference minimax depth %d vs random: %3d W / %3d L / %2d D\n",d,w,l,dr);
        }
        // head to head: repo UNBEATABLE (depth 4) vs reference depth 1 and 2
        for(int d: {1,2}){
            int w=0,l=0,dr=0;
            for(int g=0; g<20; g++){
                auto repoM=[&](Board& b,PlayerColor p){ AI ai(p,b); ai.setDifficulty(UNBEATABLE); return pick(ai).move; };
                auto refM=[&](Board& b,PlayerColor p){ return refPick(b,p,d); };
                // add variety: first 2 plies random for both
                int res;
                if(g%2==0) res = playGame([&](Board& b,PlayerColor p){ return b.countDisks(BLACK_DISK)+b.countDisks(WHITE_DISK)<6? rnd(b,p): repoM(b,p); },
                                          [&](Board& b,PlayerColor p){ return b.countDisks(BLACK_DISK)+b.countDisks(WHITE_DISK)<6? rnd(b,p): refM(b,p); });
                else        res = -playGame([&](Board& b,PlayerColor p){ return b.countDisks(BLACK_DISK)+b.countDisks(WHITE_DISK)<6? rnd(b,p): refM(b,p); },
                                            [&](Board& b,PlayerColor p){ return b.countDisks(BLACK_DISK)+b.countDisks(WHITE_DISK)<6? rnd(b,p): repoM(b,p); });
                if(res>0) w++; else if(res<0) l++; else dr++;
            }
            printf("repo UNBEATABLE(d4) vs reference depth %d: %2d W / %2d L / %2d D (20 games, colours alternated)\n",d,w,l,dr);
        }
        return 0;
    }
    if(argc>1 && !strcmp(argv[1],"crash")){
        // Find a position where the side to move has no legal move, and call the REAL computeNextMove().
        for(int seed=0;;seed++){
            std::mt19937 r(seed); Board b; PlayerColor p=BLACK_PLAYER; bool found=false;
            for(int i=0;i<60;i++){
                auto u=uniqMoves(b.getValidMoves(p));
                if(u.empty()){ if(!uniqMoves(b.getValidMoves(opp(p))).empty()){ found=true; } break; }
                std::vector<std::pair<int,int>> v(u.begin(),u.end()); auto m=v[r()%v.size()]; b.doMove({m.first,m.second},p); p=opp(p);
            }
            if(found){
                printf("Position (seed %d) where %s must PASS (0 legal moves); currentPlayer=%s\n",seed,pc(p),pc(b.getCurrentPlayer())); printBoard(b);
                AI ai(p,b); ai.setDifficulty(EASY);
                QObject::connect(&ai,&AI::nextMoveComputed,[](Move m){ printf("nextMoveComputed(%d,%d)\n",m.row,m.column); });
                printf("Calling AI::computeNextMove() ...\n"); fflush(stdout);
                ai.computeNextMove();
                printf("returned normally\n");
                return 0;
            }
        }
    }

    printf("=== 1. squareWeights(): opponent disks are never subtracted ===\n");
    {
        Board b; for(auto&row:b.board) for(auto&c:row) c=EMPTY_SQUARE;
        b.board[0][0]=b.board[0][7]=b.board[7][0]=b.board[7][7]=WHITE_DISK; // opponent owns all 4 corners (+100 each)
        b.board[3][3]=BLACK_DISK;                                            // AI owns one centre square (0)
        AI ai(BLACK_PLAYER,b);
        printf("board: WHITE on all 4 corners, BLACK on d4 only\n");
        printf("squareWeights(board, BLACK) = %d   (expected -400: 0 - 4*100)\n", ai.squareWeights(b,BLACK_PLAYER));
        printf("squareWeights(board, WHITE) = %d   (expected +400)\n", ai.squareWeights(b,WHITE_PLAYER));
        printf("corners(board, BLACK)       = %d\n", ai.corners(b,BLACK_PLAYER));
        printf("cause: AI.cpp:64-65 -> playerDisk==opponentDisk for both colours (copy-paste in the ternaries)\n\n");
    }

    printf("=== 2. treeConstruct(): every ply is generated for the SAME colour ===\n");
    {
        Board b; AI ai(BLACK_PLAYER,b); ai.parameters.depth=3;
        Node* root=new Node(b,b.getCurrentPlayer(),0,0,INT_MIN,INT_MAX,0);
        ai.treeConstruct(root,3);
        printf("root color=%s (board says %s to move)\n",pc(root->color),pc(root->State.getCurrentPlayer()));
        Node* n=root; int ply=0;
        while(!n->children.empty()){
            n=n->children[0]; ply++;
            printf("ply%d node color=%s  moveDone=(%d,%d)  its board says %s to move  B=%d W=%d  children=%zu\n",ply,pc(n->color),n->moveDone.row,n->moveDone.column,pc(n->State.getCurrentPlayer()),n->State.countDisks(BLACK_DISK),n->State.countDisks(WHITE_DISK),n->children.size());
        }
        printf("deepest board reached along first branch:\n"); printBoard(n->State);
        // count how many depth-3 leaves have wiped the opponent out entirely
        size_t leaves=0, wiped=0; std::vector<std::pair<Node*,int>> st{{root,0}};
        while(!st.empty()){ auto [x,d]=st.back(); st.pop_back(); if(d==3){ leaves++; if(x->State.countDisks(WHITE_DISK)==0) wiped++; } for(auto ch:x->children) st.push_back({ch,d+1}); }
        printf("depth-3 leaves=%zu, of which white has 0 disks in %zu (impossible in a real game after 3 plies)\n",leaves,wiped);
        printf("=> the 'minimising' plies are black's own moves; the opponent never moves in the search tree.\n\n");
        ai.deleteTree(root);
    }

    printf("=== 3. EASY (depth 1): the evaluation is computed and then ignored ===\n");
    {
        int shown=0;
        for(int seed=1; shown<3; seed++){
            std::mt19937 r(seed); Board b; randomPlay(b,r,12);
            AI ai(b.getCurrentPlayer(),b); ai.setDifficulty(EASY);
            Pick p=pick(ai);
            if(p.childScores.size()<3) continue;
            // heuristic best move per the AI's own evaluation function
            int bestH=INT_MIN; Move bestM{-1,-1}; std::set<int> distinctH;
            for(auto& cs: p.childScores){ Board t=b; t.doMove(cs.first,ai.player); int h=ai.getBoardScore(t); distinctH.insert(h); if(h>bestH){bestH=h;bestM=cs.first;} }
            if(distinctH.size()<2) continue;
            shown++;
            printf("position #%d (%s to move, %zu legal moves incl. duplicates)\n",shown,pc(ai.player),p.childScores.size());
            printf("  child Node.Score values seen by the selection loop: ");
            for(auto& cs:p.childScores) printf("%d ",cs.second); printf("\n");
            printf("  heuristic value of each move:                         ");
            for(auto& cs:p.childScores){ Board t=b; t.doMove(cs.first,ai.player); printf("%d ",ai.getBoardScore(t)); } printf("\n");
            printf("  chosen move: (%d,%d) = LAST entry;  best by heuristic: (%d,%d) score %d\n",p.move.row,p.move.column,bestM.row,bestM.column,bestH);
        }
        printf("=> minimax() returns the leaf score but never stores it in child->Score, so all children keep Score=0 and '>=' picks the last one.\n\n");
    }

    printf("=== 4. Alpha-beta pruning: cutoff frequency ===\n");
    {
        for(int depth=2; depth<=4; depth++){
            long visits=0,cut=0,total=0; int positions=0;
            for(int seed=1; seed<=20; seed++){
                std::mt19937 r(seed); Board b; randomPlay(b,r,10+seed%20);
                AI ai(b.getCurrentPlayer(),b); ai.parameters.depth=depth;
                Node* root=new Node(b,b.getCurrentPlayer(),0,0,INT_MIN,INT_MAX,0);
                ai.treeConstruct(root,depth);
                // sanity: replica must produce identical Scores to the original
                Node* root2=new Node(b,b.getCurrentPlayer(),0,0,INT_MIN,INT_MAX,0); ai.treeConstruct(root2,depth); ai.minimax(root2,true,depth);
                g_visits=g_cutoffs=0; minimaxInstr(ai,root,true,depth);
                bool same=true; std::vector<Node*> s1{root},s2{root2};
                while(!s1.empty()){ Node*a=s1.back();Node*bb=s2.back();s1.pop_back();s2.pop_back(); if(a->Score!=bb->Score||a->alpha!=bb->alpha||a->beta!=bb->beta) same=false; for(size_t i=0;i<a->children.size();i++){s1.push_back(a->children[i]);s2.push_back(bb->children[i]);} }
                if(!same) printf("  (replica mismatch at seed %d!)\n",seed);
                visits+=g_visits; cut+=g_cutoffs; total+=countNodes(root); positions++;
                ai.deleteTree(root); ai.deleteTree(root2);
            }
            printf("depth %d: %d positions, tree nodes built=%ld, nodes visited by minimax=%ld (%.1f%%), cutoffs=%ld\n",depth,positions,total,visits,100.0*visits/total,cut);
        }
        printf("=> alpha/beta live on each Node and are never passed to children, so a real cutoff cannot occur; the whole tree is materialised up-front anyway.\n\n");
    }

    printf("=== 5. Cost per move vs. depth (initial position; depth spin box has no maximum) ===\n");
    {
        for(int depth=1; depth<=7; depth++){
            Board b; AI ai(BLACK_PLAYER,b); ai.parameters.depth=depth;
            auto t0=std::chrono::steady_clock::now();
            Pick p=pick(ai);
            double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t0).count();
            struct rusage ru; getrusage(RUSAGE_SELF,&ru);
            printf("depth %d: %9zu nodes  %8.0f ms  peak RSS %6ld MB\n",depth,p.nodes,ms,ru.ru_maxrss/1024);
            if(ms>20000) break;
        }
        printf("\n");
    }

    printf("=== 6. Playing strength: AI vs uniformly random mover (pass handled by the harness) ===\n");
    {
        const char* names[]={"EASY(d1)","MEDIUM(d2)","HARD(d3)","UNBEATABLE(d4)"};
        for(int d=0; d<4; d++){
            int win=0,loss=0,draw=0; int games=100;
            for(int g=0; g<games; g++){
                Board b; PlayerColor aiColor = (g%2==0)?BLACK_PLAYER:WHITE_PLAYER;
                AI ai(aiColor,b); ai.setDifficulty((Difficulty)d);
                PlayerColor p=BLACK_PLAYER;
                while(true){
                    auto u=uniqMoves(b.getValidMoves(p));
                    if(u.empty()){ if(uniqMoves(b.getValidMoves(opp(p))).empty()) break; p=opp(p); b.currentPlayer=p; continue; }
                    Move m;
                    if(p==aiColor){ Pick pk=pick(ai); m=pk.move; }
                    else { std::vector<std::pair<int,int>> v(u.begin(),u.end()); auto x=v[rng()%v.size()]; m={x.first,x.second}; }
                    if(!b.doMove(m,p)){ printf("  AI produced illegal move (%d,%d)!\n",m.row,m.column); break; }
                    p=opp(p);
                }
                int bc=b.countDisks(BLACK_DISK), wc=b.countDisks(WHITE_DISK);
                int aiC = aiColor==BLACK_PLAYER?bc:wc, opC = aiColor==BLACK_PLAYER?wc:bc;
                if(aiC>opC) win++; else if(aiC<opC) loss++; else draw++;
            }
            printf("%-15s vs random: %3d W / %3d L / %2d D  (%d games, colours alternated)\n",names[d],win,loss,draw,games);
        }
        printf("\n");
    }

    printf("=== 7. Duplicate legal moves inflate the search tree ===\n");
    {
        std::mt19937 r(3); Board b; randomPlay(b,r,20);
        auto v=b.getValidMoves(b.getCurrentPlayer()); auto u=uniqMoves(v);
        AI ai(b.getCurrentPlayer(),b); ai.parameters.depth=3; Pick p=pick(ai);
        printf("mid-game position: getValidMoves() returned %zu entries, %zu unique -> root has %zu children; depth-3 tree = %zu nodes\n",v.size(),u.size(),v.size(),p.nodes);
    }
    return 0;
}
