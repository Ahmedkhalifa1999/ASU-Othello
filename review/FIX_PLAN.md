# ASU-Othello: remediation plan

Companion to [`REVIEW.md`](REVIEW.md). Finding numbers below (F1 to F22) refer to the severity
table in that document.

## Strategy

Fix the problems in dependency order, one pull request per phase, and keep the build and tests
green after every PR:

| Phase | PR | What | Effort | Depends on |
|---|---|---|---|---|
| 0 | PR-0 | Make it build, add tests and CI | 0.5 to 1 day | nothing |
| 1 | PR-1 | Rules engine: pass, game over, turn enforcement, dedupe, direction table | 1 day | PR-0 |
| 2 | PR-2 | AI: rewrite the search, fix the heuristic, remove `Node` | 1 day | PR-1 |
| 3 | PR-3 | Game controller and worker thread; auto-start, pass and game-over flow | 1 to 2 days | PR-2 |
| 4 | PR-4 | GUI: `PlayerPanel`, resizable board, id-based dispatch, status | 1 to 2 days | PR-3 |
| 5 | PR-5 | Hygiene: formatting, naming, docs, licence | 0.5 day | any |

About one week for one developer; phases 4 and 5 can be split among several people once PR-3 is in.

Two rules for every PR:

1. **No behaviour change without a test.** Phases 1 to 3 each add a QtTest target; phase 0 puts
   the fuzz test from the review into CI so the rules engine can never silently regress again.
2. **Rewrite, don't patch, the search.** The three AI defects (F3, F4, F5) are in code whose
   structure (eager tree, per-node alpha/beta, `Node` class) is itself the problem. A 40-line
   negamax replaces `Node.h`, `Node.cpp`, `treeConstruct`, `minimax` and `deleteTree`.

If a working demo is needed before the plan can be executed, see **Hotfix option** at the end.

## Target layout

```
CMakeLists.txt
README.md
LICENSE
.clang-format
.github/workflows/ci.yml
src/
  Board.h Board.cpp            rules engine (no Qt dependency)
  AI.h AI.cpp                  evaluation + search (no Qt dependency)
  GameController.h/.cpp        QObject: owns the Board, player configs, AI worker
  MainWindow.h/.cpp            assembles widgets, connects to the controller
  BoardWidget.h/.cpp           paints, converts clicks to Move
  PlayerPanel.h/.cpp           one colour's settings widget, instantiated twice
  main.cpp
tests/
  reference_othello.h          the reference implementation from the fuzz test
  tst_board.cpp
  tst_ai.cpp
  tst_controller.cpp
```

`Board` and `AI` become a static library with no Qt dependency so the tests and any future
command-line tournament tool can link them without a GUI.

## Phase 0: make it build, add tests and CI (PR-0)

Fixes F2, F14, F15 (documented), F19, F22.

**Changes**
- Add the missing includes: `<climits>` and `<algorithm>` in `Node.h`/`AI.cpp`, `<vector>` in
  `Board.h`, `<QElapsedTimer>` in `AI.cpp`, `<QDebug>` in `MainWindow.cpp`. Remove `<QObject>`
  from `Board.h`.
- Add an include guard to `Node.h`; delete `using namespace std;` and qualify the four uses.
- Quote-style includes for project headers.
- Switch the build to CMake (the project started on CMake; Qt 6 recommends it; CTest comes free
  and there is no case-folding of file names). Keep the `.pro` until PR-4 lands if Qt Creator
  users prefer it, then delete it.
- Delete `.vscode/tasks.json`; add `.vscode/` and `build*/` to `.gitignore`.
- `README.md`: what it is, Qt 6.4+ requirement, build and test commands, how to play, the
  three-click way to start an AI-vs-AI game (until PR-3 removes the need).
- `tests/tst_board.cpp`: port `review/harness/board_fuzz.cpp` to QtTest (2 000 seeded games
  against `reference_othello.h`). It passes today for flips and fails today for duplicates; mark
  the duplicate and pass assertions `QEXPECT_FAIL` until PR-1.
- Warnings on: `-Wall -Wextra -Wshadow -Wconversion`; treat as errors in CI.

**CMake sketch**

```cmake
cmake_minimum_required(VERSION 3.16)
project(ASU-Othello LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent Test)

add_library(othello_core STATIC src/Board.cpp src/AI.cpp)
target_include_directories(othello_core PUBLIC src)

add_executable(ASU-Othello src/main.cpp src/MainWindow.cpp src/BoardWidget.cpp
                           src/PlayerPanel.cpp src/GameController.cpp)
target_link_libraries(ASU-Othello PRIVATE othello_core Qt6::Widgets Qt6::Concurrent)

enable_testing()
foreach(t tst_board tst_ai tst_controller)
  add_executable(${t} tests/${t}.cpp)
  target_link_libraries(${t} PRIVATE othello_core Qt6::Test Qt6::Concurrent)
  add_test(NAME ${t} COMMAND ${t})
endforeach()
```

**CI sketch** (`.github/workflows/ci.yml`)

```yaml
name: CI
on: [push, pull_request]
jobs:
  linux:
    runs-on: ubuntu-24.04
    steps:
      - uses: actions/checkout@v4
      - run: sudo apt-get update && sudo apt-get install -y qt6-base-dev cmake ninja-build
      - run: cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
             -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wshadow -Werror"
      - run: cmake --build build
      - run: QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

**Done when**: a fresh clone builds on Linux with zero warnings, `ctest` runs, CI is green, and
the README is enough for a new team member to build and play.

## Phase 1: rules engine (PR-1)

Fixes F6 (engine half), F10, F11; removes the `Board.cpp` duplication.

**New `Board` interface**

```cpp
enum class Square : uint8_t { Empty, Black, White };
enum class Color  : uint8_t { Black, White };
constexpr Color  opponent(Color c);
constexpr Square disk(Color c);

struct Move { int row, col; bool operator==(const Move&) const = default; };

enum class PlayResult { Illegal, Played, OpponentPassed, GameOver };

class Board {
public:
    using Grid = std::array<std::array<Square, 8>, 8>;
    Board();                                             // standard opening
    const Grid&        grid() const;
    Color              sideToMove() const;
    bool               isGameOver() const;
    std::optional<Color> winner() const;                 // nullopt while running or on a draw
    int                count(Square) const;
    std::vector<Move>  legalMoves(Color) const;          // each square at most once
    bool               hasLegalMove(Color) const;
    std::vector<Move>  flipsFor(Move, Color) const;      // empty => illegal
    PlayResult         play(Move);                       // only the side to move may play
private:
    Grid  grid_{};
    Color sideToMove_ = Color::Black;
    bool  gameOver_ = false;
};
```

**Implementation notes**
- One direction table replaces the sixteen pasted blocks:

  ```cpp
  constexpr std::array<std::pair<int,int>, 8> kDirections{{
      {-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}}};

  std::vector<Move> Board::flipsFor(Move m, Color c) const {
      std::vector<Move> out;
      if (!inBounds(m.row, m.col) || grid_[m.row][m.col] != Square::Empty) return out;
      for (auto [dr, dc] : kDirections) {
          std::vector<Move> line;
          int r = m.row + dr, k = m.col + dc;
          while (inBounds(r, k) && grid_[r][k] == disk(opponent(c))) { line.push_back({r, k}); r += dr; k += dc; }
          if (!line.empty() && inBounds(r, k) && grid_[r][k] == disk(c))
              out.insert(out.end(), line.begin(), line.end());
      }
      return out;
  }
  ```
  This is the reference implementation from the fuzz test, which has already survived 118 095
  moves of comparison, so the rewrite starts from known-good code.
- `legalMoves(c)` scans the 64 squares and keeps those with non-empty `flipsFor`; that yields
  each square once (F10).
- `play(m)` applies `flipsFor(m, sideToMove_)`, then advances the turn: opponent if it has a
  move, else the mover stays and the result is `OpponentPassed`, else `GameOver` (F6). It rejects a
  move when `flipsFor` is empty, which also covers "not your turn" because callers can no longer
  pass a colour (F11).
- `std::array` instead of `vector<vector>`: nine fewer allocations per copy, which matters in
  phase 2 where the search copies a board per node.

**Tests (`tst_board`)**
- Fuzz against the reference (already written): legal-move sets equal, resulting grids equal,
  and, new, `sideToMove` and `isGameOver` equal after every move including passes.
- Explicit cases: opening position has exactly four legal moves; a move by a square with no
  flips is `Illegal`; a position where the opponent must pass returns `OpponentPassed` and keeps
  the mover; double pass returns `GameOver`; a full board is `GameOver`; wiping the opponent out
  is `GameOver` with the right winner; `winner()` is `nullopt` on 32-32.

**Done when**: fuzz has 0 mismatches over 2 000 games, `legalMoves` never contains a duplicate,
and `BoardWidget`/`AI` compile against the new interface (temporarily via a thin adapter if PR-2
is not ready).

## Phase 2: AI rewrite (PR-2)

Fixes F1, F3, F4, F5, F8, F9, F18, F21 (by removing `Node`), and the smaller items in review
section 3.8.

**Delete** `Node.h`, `Node.cpp`, `AI::treeConstruct`, `AI::minimax`, `AI::deleteTree`, and the
`QObject` base of `AI`. The class becomes a value type with no Qt dependency; threading moves to
the controller in PR-3.

**New interface**

```cpp
struct AlgorithmParameters { int depth = 3; int cornerWeight = 1; int squareWeight = 1; };

class AI {
public:
    explicit AI(AlgorithmParameters p);
    std::optional<Move> chooseMove(const Board& b) const;   // nullopt: no legal move
    int  evaluate(const Board& b, Color me) const;          // public so tests can pin it
    mutable long nodesSearched = 0;                         // for tests and a debug label
private:
    int search(const Board& b, Color me, int depth, int alpha, int beta) const;
    AlgorithmParameters p_;
};
```

**Search**

```cpp
int AI::search(const Board& b, Color me, int depth, int alpha, int beta) const {
    ++nodesSearched;
    if (b.isGameOver()) return terminalValue(b, me);   // +-(10000 + disk margin), 0 for a draw
    if (depth == 0)     return evaluate(b, me);
    const bool maximising = (b.sideToMove() == me);
    int best = maximising ? INT_MIN : INT_MAX;
    for (Move m : cornersFirst(b.legalMoves(b.sideToMove()))) {
        Board next = b;
        next.play(m);                                   // advances to the next side that can move
        const int v = search(next, me, depth - 1, alpha, beta);
        if (maximising) { best = std::max(best, v); alpha = std::max(alpha, v); }
        else            { best = std::min(best, v); beta  = std::min(beta,  v); }
        if (beta <= alpha) break;
    }
    return best;
}

std::optional<Move> AI::chooseMove(const Board& b) const {
    const Color me = b.sideToMove();
    std::optional<Move> best; int bestValue = INT_MIN;
    for (Move m : cornersFirst(b.legalMoves(me))) {
        Board next = b; next.play(m);
        const int v = search(next, me, p_.depth - 1, INT_MIN, INT_MAX);
        if (!best || v > bestValue) { best = m; bestValue = v; }
    }
    return best;
}
```

Because `Board::play` advances to whichever side can move, passes fall out of the search for
free and the colour alternates correctly (F3). A root with no legal move returns `nullopt`
instead of crashing (F1). Alpha and beta are parameters, so pruning works (F8); the tree is never
stored (F9).

**Heuristic**
- `evaluate()` = `squareWeight * squareTerm + cornerWeight * cornerTerm`, with
  `opponentDisk = disk(opponent(me))` (F4). Keep the existing weight table.
- Add a cheap mobility term, `legalMoves(me).size() - legalMoves(opponent).size()`, weighted
  around 5; it is the single biggest strength gain available for the cost.
- `cornersFirst()` orders corner moves first so alpha-beta cuts more.

**Difficulty**
- Easy = depth 1, Medium = 2, Hard = 4, Unbeatable = 6. With pruning, depth 6 is well under a
  second mid-game. Cap the depth spin box at 8 in PR-4.
- Optional: break ties between equal-valued moves with a seeded random pick so Easy does not
  always answer the same way.

**Tests (`tst_ai`)**
- `evaluate(b, Black) == -evaluate(b, White)` for 100 random positions.
- White on four corners, black on d4: `evaluate(b, Black)` is negative and equals
  `-evaluate(b, White)`.
- `chooseMove` on a position where the side to move has no legal move returns `nullopt`.
- Depth 1 picks the argmax of `evaluate` over legal moves (the harness position from the review
  where the old code picked the last entry).
- Depth 2 declines a move that hands the opponent a corner when a safe alternative exists.
- Pruning: `nodesSearched` at depth 4 is below 25% of the unpruned count on 20 random positions
  (compute the unpruned count with alpha = INT_MIN, beta = INT_MAX pinned).
- Strength gates, seeded: depth 3 beats a random mover at least 90 of 100; depth 4 beats depth 1
  at least 65 of 100 with the first two plies randomised. 1 000 AI-vs-AI games at depth 2 finish
  with no exception and a `GameOver` result.

**Done when**: the tests above pass, `Node.*` is gone, and `AI` has no `#include <Q...>`.

## Phase 3: game controller and worker thread (PR-3)

Fixes F6 (flow half), F7, F12, F21, and the `msleep` and hidden-start items.

**`GameController` (QObject)** is the single owner of game state; `MainWindow` only forwards
user intent to it and paints what it emits.

```cpp
class GameController : public QObject {
    Q_OBJECT
public:
    const Board& board() const;
    void setPlayerKind(Color, PlayerKind);           // Human or AI
    void setParameters(Color, AlgorithmParameters);
public slots:
    void restart();
    void humanMove(Move);                             // ignored unless it is a human's turn
signals:
    void boardChanged();
    void turnChanged(Color);
    void passed(Color whoHadNoMove);
    void gameOver(std::optional<Color> winner, int black, int white);
    void aiThinking(bool);
private:
    void afterMove(PlayResult);
    void maybeStartAi();
    Board board_;
    std::array<PlayerKind, 2> kinds_;
    std::array<AlgorithmParameters, 2> params_;
    int generation_ = 0;                              // invalidates in-flight AI results
    bool thinking_ = false;
};
```

**Threading**
- `Q_DECLARE_METATYPE(Move)` and `qRegisterMetaType<Move>()` in `main()` (F21).
- Run the search with `QtConcurrent::run` on a snapshot of the board and watch it with a
  `QFutureWatcher`; the GUI thread never blocks:

  ```cpp
  void GameController::maybeStartAi() {
      if (board_.isGameOver() || kinds_[idx(board_.sideToMove())] != PlayerKind::AI || thinking_) return;
      thinking_ = true; emit aiThinking(true);
      const int generation = ++generation_;
      const Board snapshot = board_;
      const AI ai(params_[idx(board_.sideToMove())]);
      auto* watcher = new QFutureWatcher<std::optional<Move>>(this);
      QElapsedTimer started; started.start();
      connect(watcher, &QFutureWatcherBase::finished, this, [=]() mutable {
          watcher->deleteLater();
          if (generation != generation_) return;                        // restart happened meanwhile
          const int wait = std::max(0, kMinVisibleDelayMs - int(started.elapsed()));
          QTimer::singleShot(wait, this, [=] { thinking_ = false; emit aiThinking(false);
                                               applyAiMove(watcher->result()); });
      });
      watcher->setFuture(QtConcurrent::run([ai, snapshot] { return ai.chooseMove(snapshot); }));
  }
  ```
- `restart()`, `setPlayerKind()` and `setParameters()` bump `generation_` so a stale result is
  dropped, then call `maybeStartAi()`, which removes every hidden "click the board to start"
  path (F12).
- `afterMove()` emits `boardChanged`, then `passed` when the result was `OpponentPassed`, then
  `gameOver` or `turnChanged`, then `maybeStartAi()`. AI-vs-AI therefore runs from the event loop,
  one move per completed future, repainting between moves (F7).
- `humanMove()` is ignored while `thinking_` or when the side to move is an AI.

**Tests (`tst_controller`, `QCoreApplication`, `QSignalSpy`)**
- AI vs AI at depth 1 with a 0 ms minimum delay: `gameOver` is emitted with no user input, and
  `boardChanged` fired once per move.
- A position where the AI's reply forces the human to pass emits `passed(Human colour)` and the
  AI moves again by itself.
- `restart()` while an AI future is running: the stale result is not applied (board is the
  opening position after the future completes).
- `humanMove()` during `thinking_` is a no-op.
- Switching the side to move from Human to AI starts a move without a click.

**Done when**: the scripted session from the review (`review/harness/gui_drive.py`, adapted)
shows the board changing every move in AI-vs-AI mode and the process exiting normally.

## Phase 4: GUI (PR-4)

Fixes F13, F16, F17, F20, and the remaining section 4 items.

- **`PlayerPanel`**: radio group (ids, `idClicked`), difficulty combo (`currentIndexChanged`
  with `itemData` holding the `Difficulty` enum), depth spin box (1 to 8), corner and square
  weight spin boxes (0 to 10), emitting `kindChanged(PlayerKind)` and
  `parametersChanged(AlgorithmParameters)`. AI controls disabled while Human is selected.
  `MainWindow` creates two, tagged with their `Color`, and connects both to the controller. The
  twelve white/black slot pairs disappear.
- **`BoardWidget`**: takes `const Board&`; `cell = std::min(width(), height()) / 8` computed in
  `paintEvent`, grid drawn in a loop, disks inset by 10% of a cell, hints from `legalMoves`
  (now unique). `mousePressEvent`: left button only, `row = y / cell`, ignore clicks outside the
  8x8 area. `sizeHint` 640x640, expanding size policy; the fixed 801x801 goes away.
- **Status**: a `QLabel` in the bottom dock shows "Black to move" / "White thinking...";
  `passed` shows a transient message ("White has no legal move, Black plays again"); `gameOver`
  shows a `QMessageBox` with the winner and the disk counts and offers Restart.
- **Labels**: "White: 2" / "Black: 2" from the start, updated from `boardChanged`; the
  "White AI" copy-paste message goes away with the merged slot.
- Input is disabled on the board while `aiThinking(true)`.

**Done when**: a manual checklist passes on Linux and Windows: resize, HiDPI, human vs AI, AI vs
AI without clicking, pass shown, game-over dialog, restart mid-think.

## Phase 5: hygiene (PR-5)

- `.clang-format` (Qt style or LLVM with 4-space indent) and one formatting commit.
- Naming pass: `State` to `board`, `Score` to `score`, `Max` to `maximising`, fix `Difficuly`
  and the typos listed in the review.
- Replace the empty Doxygen stubs with either real text or nothing.
- `LICENSE` (the team's choice; MIT is the usual default for a student project).
- `CONTRIBUTING.md` with the branch and commit-message convention (imperative subject, why in
  the body) so the "Final Commit 8" history does not repeat.

## Finding to fix traceability

| Finding | Fixed in | How |
|---|---|---|
| F1 crash on no legal move | PR-2 | `chooseMove` returns `nullopt`; controller treats it as a pass |
| F2 missing `<climits>` | PR-0 | includes added; CI would catch a repeat |
| F3 same colour every ply | PR-2 | `Board::play` advances the side to move; search recurses on the new board |
| F4 opponent disks never subtracted | PR-2 | `disk(opponent(me))`; symmetry test |
| F5 Easy ignores evaluation | PR-2 | search returns values; no stored `Score` |
| F6 no pass / game over | PR-1, PR-3 | `PlayResult`, `isGameOver`, `winner`; controller signals |
| F7 AI on GUI thread | PR-3 | `QtConcurrent` + `QFutureWatcher`; `QTimer` for the visible delay |
| F8 pruning no-op | PR-2 | alpha/beta as parameters; node-count test |
| F9 eager tree, unbounded depth | PR-2, PR-4 | no tree; depth capped at 8 |
| F10 duplicate legal moves | PR-1 | generate by scanning empty squares |
| F11 turn not enforced | PR-1 | `play(Move)` has no colour parameter |
| F12 hidden start conditions | PR-3 | `maybeStartAi()` after every state change |
| F13 dispatch by display string | PR-4 | ids and `itemData` |
| F14 `.pro` file-name case | PR-0 | CMake; correct names |
| F15 Qt 5 build / missing headers | PR-0 | documented Qt 6 requirement; includes fixed |
| F16 initial labels | PR-4 | labels driven from `boardChanged` |
| F17 "White AI" message for black | PR-4 | single slot |
| F18 `Node()` self-assignment | PR-2 | `Node` deleted |
| F19 header hygiene | PR-0 | guard, no `using namespace`, quoted includes |
| F20 fixed-size board, if-ladder | PR-4 | derived cell size, division |
| F21 `Move` not a metatype | PR-3 | `Q_DECLARE_METATYPE` |
| F22 `.vscode/tasks.json` | PR-0 | deleted, ignored |

## Definition of done for the whole plan

- Fresh clone builds with CMake on Linux, macOS and Windows against Qt 6.4+ with zero warnings.
- `ctest` runs `tst_board`, `tst_ai`, `tst_controller` in CI on every push; all green.
- Unbeatable beats a random mover at least 95 of 100 seeded games and depth 1 at least 75 of 100.
- 1 000 headless AI-vs-AI games complete with no crash and a `gameOver` signal each.
- The GUI never blocks for more than a frame; AI-vs-AI plays itself from Restart with no clicks.
- Passes and game over are shown to the user.
- README explains build, play, and difficulty levels.

## Hotfix option (if a demo is needed before the plan is executed)

About 40 lines against the current code, in this order. It makes the game playable but keeps
the poor structure; do not let it replace the plan.

1. `Node.h`: `#include <climits>` (F2).
2. `AI.cpp:65`: `opponentDisk = (player == BLACK_PLAYER) ? WHITE_DISK : BLACK_DISK;` (F4).
3. `AI.cpp:156`: create children with `getOpponent(currentNode->color)` and, since the child's
   colour must be the side that can actually move, skip a level when that side has no moves (F3).
4. `AI.cpp:171` and `186`: after `int eval = minimax(...)`, add `child->Score = eval;` (F5).
5. `AI.cpp:127`: `if (parentNode->children.empty()) { deleteTree(parentNode); emit nextMoveComputed({-1, -1}); return; }`
   and make both `MainWindow::*NextMoveComputed` slots treat `{-1, -1}` as a pass (F1).
6. `Board.cpp:381`: after toggling `currentPlayer`, if the new player has no legal move and the
   old one does, toggle back; add `bool Board::isGameOver()` = neither side can move, and show a
   message box from `MainWindow` when it turns true (F6).
7. `Board::getValidMoves`: `std::sort` + `std::unique` on `(row, column)` before returning (F10).
8. `MainWindow.cpp:165-166`: correct the initial label text (F16).

Even with all eight, the AI still blocks the GUI thread and AI-vs-AI still needs a click to
start; those need PR-3.
