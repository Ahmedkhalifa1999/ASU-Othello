// Fuzz-compares Board.cpp against a straightforward reference Othello implementation.
#include "Board.h"
#include <cstdio>
#include <set>
#include <random>
#include <algorithm>

struct Ref {
    int b[8][8]{}; // 0 empty, 1 black, 2 white
    Ref() { b[3][3]=2; b[3][4]=1; b[4][3]=1; b[4][4]=2; }
    static int disk(PlayerColor p){ return p==BLACK_PLAYER?1:2; }
    std::vector<std::pair<int,int>> flips(int r,int c,PlayerColor p) const {
        std::vector<std::pair<int,int>> out;
        if (b[r][c]!=0) return out;
        int me=disk(p), op=3-me;
        for(int dr=-1;dr<=1;dr++) for(int dc=-1;dc<=1;dc++){
            if(!dr&&!dc) continue;
            std::vector<std::pair<int,int>> line;
            int rr=r+dr, cc=c+dc;
            while(rr>=0&&rr<8&&cc>=0&&cc<8&&b[rr][cc]==op){ line.push_back({rr,cc}); rr+=dr; cc+=dc; }
            if(!line.empty()&&rr>=0&&rr<8&&cc>=0&&cc<8&&b[rr][cc]==me) out.insert(out.end(),line.begin(),line.end());
        }
        return out;
    }
    std::set<std::pair<int,int>> valid(PlayerColor p) const {
        std::set<std::pair<int,int>> s;
        for(int r=0;r<8;r++)for(int c=0;c<8;c++) if(!flips(r,c,p).empty()) s.insert({r,c});
        return s;
    }
    void play(int r,int c,PlayerColor p){ auto f=flips(r,c,p); b[r][c]=disk(p); for(auto&x:f) b[x.first][x.second]=disk(p); }
};

static bool same(const Board& bd,const Ref& rf){
    auto& v=bd.getBoard();
    for(int r=0;r<8;r++)for(int c=0;c<8;c++){
        int x = v[r][c]==EMPTY_SQUARE?0: v[r][c]==BLACK_DISK?1:2;
        if(x!=rf.b[r][c]) return false;
    }
    return true;
}

int main(){
    std::mt19937 rng(12345);
    int games=0, moves=0, dupLists=0, maxDup=0, passSituations=0, stuckGames=0, mismatches=0;
    long totalListed=0, totalUnique=0;
    for(int g=0; g<2000; g++){
        Board bd; Ref rf; PlayerColor p=BLACK_PLAYER;
        games++;
        for(int turn=0; turn<200; turn++){
            auto listed = bd.getValidMoves(p);
            std::set<std::pair<int,int>> uniq; for(auto&m:listed) uniq.insert({m.row,m.column});
            auto ref = rf.valid(p);
            totalListed += listed.size(); totalUnique += uniq.size();
            if(listed.size()!=uniq.size()){ dupLists++; maxDup=std::max(maxDup,(int)(listed.size()-uniq.size())); }
            if(uniq!=ref){ mismatches++; printf("VALID-MOVE MISMATCH game %d turn %d\n",g,turn); break; }
            if(ref.empty()){
                PlayerColor o = p==BLACK_PLAYER?WHITE_PLAYER:BLACK_PLAYER;
                if(rf.valid(o).empty()) break; // game over
                // Othello pass: opponent should move. Does Board allow it?
                passSituations++;
                if(bd.getCurrentPlayer()==p){ stuckGames++; }
                break; // stop this game: Board has no pass concept
            }
            std::vector<std::pair<int,int>> choices(ref.begin(),ref.end());
            auto mv = choices[rng()%choices.size()];
            bool ok = bd.doMove({mv.first,mv.second},p);
            rf.play(mv.first,mv.second,p);
            moves++;
            if(!ok||!same(bd,rf)){ mismatches++; printf("DOMOVE MISMATCH game %d turn %d\n",g,turn); break; }
            p = p==BLACK_PLAYER?WHITE_PLAYER:BLACK_PLAYER;
            if(bd.getCurrentPlayer()!=p){ mismatches++; printf("CURRENT PLAYER MISMATCH\n"); break; }
        }
    }
    printf("games=%d moves=%d\n",games,moves);
    printf("flip/valid-move correctness mismatches vs reference: %d\n",mismatches);
    printf("valid-move lists containing duplicates: %d  (max extra entries in one list: %d)\n",dupLists,maxDup);
    printf("total listed entries=%ld  unique=%ld  (%.1f%% redundant)\n",totalListed,totalUnique,100.0*(totalListed-totalUnique)/totalListed);
    printf("games that reached a PASS situation (mover has no moves, opponent does): %d; Board left turn with stuck player in %d of them\n",passSituations,stuckGames);

    // Does doMove enforce turn order?
    Board bd; // black to move
    bool r = bd.doMove({2,4}, WHITE_PLAYER); // legal geometry for white? (2,4) is above black(3,4)... white at (4,4) flanks -> yes legal for white
    printf("doMove(WHITE) while currentPlayer==BLACK accepted: %s; currentPlayer now=%s\n", r?"YES":"no", bd.getCurrentPlayer()==BLACK_PLAYER?"BLACK":"WHITE");
    return 0;
}
