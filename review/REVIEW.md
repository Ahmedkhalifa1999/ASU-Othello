# ASU-Othello: code review

Reviewed at commit `31da9f8` ("Final Commit 8"), branch `main`. The project is a Qt Widgets Othello
(Reversi) game with a minimax AI, written by a student team of about eight people over roughly three weeks in
May and June 2023 (11 git identities, several of them the same person). This review covers the build, the rules engine, the AI, the GUI, and code quality.
Every claim below that is marked **[verified]** was reproduced in a fresh environment; the exact
commands and the probe programs used are in `harness/`. The remediation plan is in
[`FIX_PLAN.md`](FIX_PLAN.md).

## How the review was done

| Step | Result |
|---|---|
| Ubuntu 24.04, GCC 13.3, Qt 6.4.2, `qmake6` + `make` on the unmodified tree | **Does not compile** (missing `<climits>`) |
| Same tree, Qt 5.15.13 | Does not compile (Qt 6 only API usage, missing `<QDebug>`) |
| One-line fix (`#include <climits>` in `Node.h`) in a scratch copy, `-Wall -Wextra -Wshadow` | Builds; 15 shadowing / narrowing warnings in project code |
| `cppcheck --enable=all` | Self-assignment in `Node::Node()`, uninitialised member, pass-by-value, non-explicit ctor |
| Ran the GUI under Xvfb, drove it with `xdotool`, screenshots + per-second pixel sampling | Human vs AI works; AI vs AI freezes the window and ends in **SIGSEGV** |
| Fuzz test: 2 000 random games (118 095 moves) of `Board` against an independent reference | Flip logic correct; duplicates and pass handling broken |
| Probe program driving `AI` internals; 100-game matches vs a random mover and vs a correct minimax | See section 3 |
| AddressSanitizer on the real `AI::computeNextMove()` | No leaks; null-page read when the side to move has no legal move |

## Verdict

The rules engine (`Board`) is correct: over 118 095 fuzzed moves it never disagreed with a reference
implementation on legal moves or on which disks flip. Everything built on top of it has serious
problems. In order of impact:

1. **The project does not build as committed** on a current compiler (missing `<climits>`), and it
   only builds against Qt 6, which nothing documents.
2. **Every game against the AI ends in a crash.** When the side to move has no legal move (which
   always happens at the end of a game, and in 37% of random games somewhere in the middle), the AI
   dereferences the first element of an empty vector.
3. **The AI's search is wrong in three independent ways** (it never lets the opponent move, its
   heuristic never counts opponent disks, and on Easy it throws away the evaluation entirely). The
   result: "Unbeatable" beats a random mover only 76% of the time and **loses 20-0** to a correct
   one-ply search using the same heuristic.
4. **The game has no concept of a pass or of game over.** A player with no legal move simply leaves
   the game stuck until Restart.
5. **The AI runs on the GUI thread** with a deliberate one-second `msleep`, so the window freezes
   for every AI move and never repaints during an AI-vs-AI game.

Beneath those are a large amount of copy-paste duplication (the eight board directions are written
out eight times, twice; every white-player widget is duplicated for black), string-based dispatch of
UI events, and a repository with no README, licence, tests or CI, and commit messages such as
"Final Commit 8", "." and ",".

## Findings by severity

| # | Severity | Where | Finding |
|---|---|---|---|
| 1 | Critical | `AI.cpp:127` | `parentNode->children[0]` on an empty vector when there is no legal move: SIGSEGV **[verified]** |
| 2 | Critical | `Node.h:25-26`, `AI.cpp` | `INT_MIN`/`INT_MAX` used without `<climits>`: build fails on GCC 13 / Qt 6.4 **[verified]** |
| 3 | High | `AI.cpp:156` | Child nodes are created with the parent's colour, so the opponent never moves in the search tree **[verified]** |
| 4 | High | `AI.cpp:64-65` | `opponentDisk` is computed as the player's own disk; opponent disks are never subtracted from the score **[verified]** |
| 5 | High | `AI.cpp:127-135`, `AI.cpp:163-164` | Leaf scores are returned but never stored, so at depth 1 every child has `Score == 0` and `>=` selects the last legal move **[verified]** |
| 6 | High | `Board.cpp:381-382`, `MainWindow.cpp` | No pass handling and no game-over detection: game deadlocks when a player has no move **[verified]** |
| 7 | High | `MainWindow.cpp:7-10`, `AI.cpp:141-143` | AI slot executes synchronously on the GUI thread, including a `QThread::msleep(1000)`; AI-vs-AI game runs entirely inside one mouse event with zero repaints **[verified]** |
| 8 | Medium | `AI.cpp:173-175`, `188-190` | Alpha-beta cutoff can never fire (alpha/beta are per-node and never inherited): 64 cutoffs in 930 712 visited nodes **[verified]** |
| 9 | Medium | `AI.cpp:148-160`, `MainWindow.cpp:97` | Whole tree materialised eagerly, one `Board` copy per node; depth spin box has no maximum: depth 6 = 709 119 nodes / 482 MB / 1.5 s per move, growing about 5x per ply **[verified]** |
| 10 | Medium | `Board.cpp:134-150` | `getValidMoves()` returns the same square once per flanking line: 25.5% of entries are duplicates, up to 17 extras in one list; inflates the search tree and stacks the semi-transparent hint circles **[verified]** |
| 11 | Medium | `Board.cpp:157-168`, `381` | `doMove()` does not check that `player` is the side to move, and toggles `currentPlayer` regardless of who moved **[verified]** |
| 12 | Medium | `MainWindow.cpp:298-318` | Switching a colour to AI, and AI-vs-AI, only start when the user clicks the board (undocumented); Restart never kicks a black AI **[verified]** |
| 13 | Medium | `MainWindow.cpp:71-72`, `83-84`, `202-233` | UI events dispatched by comparing widget display strings (`"AI"`, `"Easy"`) |
| 14 | Medium | `ASU-Othello.pro:17,24` | Lists `mainwindow.cpp/.h`; files are `MainWindow.cpp/.h`. qmake6 silently case-folds, CMake or a stricter tool would not |
| 15 | Medium | `MainWindow.cpp:71-72,107-150,177,190` | Qt 5 build fails: ambiguous `QSpinBox::valueChanged`, `QButtonGroup::buttonClicked`, and `QDebug` used without its header **[verified]** |
| 16 | Low | `MainWindow.cpp:165-166` | Initial labels read "White Player: 0" and "Black Score: 0" (should be 2 / 2) **[verified]** |
| 17 | Low | `MainWindow.cpp:190` | Black AI's invalid-move message says "White AI" |
| 18 | Low | `Node.cpp:3-11` | Default constructor self-assigns uninitialised members |
| 19 | Low | `Node.h:1-3` | No include guard; `using namespace std;` in a header; `#include <Board.h>` with angle brackets |
| 20 | Low | `BoardWidget.cpp:6`, `92-169` | Board hard-coded to 801x801 px (not resizable, not HiDPI aware); 16-branch if/else ladder to compute `x / 100` |
| 21 | Low | `BoardWidget.h:45`, `AI.h:122` | `Move` is passed through signals but not registered with `Q_DECLARE_METATYPE`; the first attempt to move the AI to a thread (which `AI.h` clearly intends) will fail at runtime |
| 22 | Low | `.vscode/tasks.json` | Windows-only "compile the active file with `C:\MinGW\bin\g++.exe`" task; cannot build a Qt project |

## 1. Build and repository

**Does not compile.** `Node.h:25-26` and `AI.cpp` use `INT_MIN`/`INT_MAX` without including
`<climits>`. This compiled for the authors because their Qt headers happened to include it
transitively; with Qt 6.4 on GCC 13 it fails:

```
Node.h:25:21: error: 'INT_MIN' was not declared in this scope
AI.cpp:120:69: error: 'INT_MIN' was not declared in this scope
```

The same pattern appears with `std::max`/`std::min` (`AI.cpp:172`, reached through the
`using namespace std;` in `Node.h` and a transitive `<algorithm>`), `QElapsedTimer` (`AI.cpp:115`),
`QDebug` (`MainWindow.cpp:177`) and `std::vector` (`Board.h:27`), none of which are included where
they are used. Include what you use.

**Qt 6 only, undocumented.** With Qt 5.15 the build fails on `&QSpinBox::valueChanged` and
`&QButtonGroup::buttonClicked` (both overloaded in Qt 5) and on the missing `<QDebug>`. There is no
README stating the Qt version, the build steps, or how to play.

**Case-sensitive file names.** The `.pro` file lists `mainwindow.cpp` and `mainwindow.h`, but the
files on disk are `MainWindow.cpp` and `MainWindow.h` (git history shows the project moved from
CMake to qmake and the `.ui` file was deleted along the way). qmake6 happened to case-fold the
lookup here; the mismatch is a trap for anyone moving to CMake or another generator.

**Project headers included with angle brackets** (`#include <Board.h>`, `<Node.h>`, `<AI.h>`).
That works only because qmake adds the project directory to the include path. Use quotes for local
headers.

**Repository hygiene.** No README, no LICENSE, no tests, no CI, no `.clang-format`. The committed
`.vscode/tasks.json` is a machine-specific Windows task that can never build this project. Commit
history: "Final Commit 1" through "Final Commit 8", ".", ",", "c", "yarab". Nobody can bisect that.

## 2. Rules engine (`Board`)

**What is right.** Move generation and flipping are correct in all eight directions. Fuzzing 2 000
random games against an independent implementation produced 0 mismatches over 118 095 moves.

**No pass, no game over.** `doMove()` (`Board.cpp:381-382`) unconditionally hands the turn to the
other colour. Othello requires the turn to return to the mover when the opponent has no legal move,
and the game to end when neither side can move. Neither exists anywhere in the code:

- 742 of 2 000 random games (37%) reached a position where the side to move had no legal move but
  the opponent did. In every one, `Board` left `currentPlayer` on the stuck side.
- In the GUI a stuck human can only press Restart; a stuck AI crashes (finding 1).
- There is no winner display; the score labels just count disks.

**Duplicate legal moves.** `getValidMoves()` scans every opponent disk and, for each, walks the
eight neighbours; a square that flanks along two or three lines is pushed two or three times.
Across the fuzz run 25.5% of returned entries were duplicates, and one list contained 17 extras.
Downstream, every duplicate becomes an extra subtree in the AI search and an extra alpha-blended
circle in `BoardWidget::paintEvent`, so some hints are drawn darker than others for no reason.

**Turn order is not enforced.** `doMove(move, player)` validates geometry but not that `player`
is the side to move, and then toggles `currentPlayer` whoever moved. This is what lets the AI's
search tree play one colour four times in a row (finding 3) without any error.

**Design.**
- `getValidMovesAroundDisk()` is 120 lines and `doMove()` is 230 lines of the same code pasted
  eight times with signs changed. A direction table (`{dr, dc}` x 8) reduces each to about 20 lines
  and removes the copy-paste risk that already bit the AI (finding 4).
- `doMove()` validates one move by regenerating **all** legal moves; `treeConstruct()` then calls
  it once per child after having already called `getValidMoves()` for the same position, so every
  node costs two full-board move generations.
- The board is `std::vector<std::vector<BoardSquareState>>`: nine heap allocations per copy, and
  the AI copies one per node. `std::array<std::array<...,8>,8>` (or a bitboard) is trivially
  copyable and cache-friendly.
- `getValidMoves()` is not `const`, so nothing can hold a `const Board&` and ask it anything useful
  (`BoardWidget` takes a mutable reference because of this).
- The `row - offset < 8` loop guards in `doMove()` (`Board.cpp:183` etc.) rely on `unsigned`
  wrap-around to stop at the board edge. It works, but it reads like an off-by-one bug and the
  neighbouring function uses explicit `>= 0` checks instead.
- C-style `typedef enum` in the global namespace (`EMPTY_SQUARE`, `EASY`, ...); `Move` has no
  equality operator, so callers compare fields by hand; the `PlayerColor` to `BoardSquareState`
  conversion is re-derived with a ternary in at least six places.

## 3. AI

This is where most of the effort went and where most of the defects are. The probe program
(`harness/ai_probe.cpp`) exercises the real `AI`, `Node` and `Board` code.

### 3.1 Crash when there is no legal move (`AI.cpp:127`)

`computeNextMove()` builds the tree, runs `minimax()`, then reads
`parentNode->children[0]->Score`. If the side to move has no legal move the root has no children
and that is a read through the empty vector's null data pointer:

```
==2924==ERROR: AddressSanitizer: SEGV on unknown address 0x000000000000
    #0 in AI::computeNextMove() AI.cpp:127
```

This is not an edge case. At the end of every game the last player to move leaves the other side
without a move, so **every game against the AI ends with the process dying**. In the scripted GUI
session the AI-vs-AI game ran for 64 s and exited with signal 11.

### 3.2 The search tree never lets the opponent move (`AI.cpp:156`)

`treeConstruct()` creates each child with `currentNode->color`, the same colour as the parent,
rather than the colour of the side to move in the child position. Consequently the "minimising"
plies of the minimax are the AI's own second and third moves, and the opponent is modelled as
passing forever. From the opening position the tree is: black plays, black plays again, white has
zero disks, tree ends. Observed:

```
ply1 node color=BLACK  moveDone=(2,3)  its board says WHITE to move  B=4 W=1
ply2 node color=BLACK  moveDone=(4,5)  its board says BLACK to move  B=6 W=0  children=0
```

A side effect: from the opening, depth 3, 4, 5 and 6 all produce the same 17-node tree, so
"Hard" and "Unbeatable" are literally the same player until the board fills up.

### 3.3 The heuristic never counts the opponent (`AI.cpp:64-65`)

```cpp
BoardSquareState playerDisk   = (player == WHITE_PLAYER)? WHITE_DISK:BLACK_DISK;
BoardSquareState opponentDisk = (player == BLACK_PLAYER)? BLACK_DISK:WHITE_DISK;  // same as playerDisk
```

Both ternaries resolve to the player's own disk, so the `else if (== opponentDisk)` branch is dead.
With white on all four corners (each worth 100) and black on one centre square,
`squareWeights(board, BLACK)` returns 0 instead of -400.

### 3.4 Easy ignores its own evaluation (`AI.cpp:127-135`, `163-164`)

`minimax()` returns the leaf evaluation but never writes it into `child->Score`; `Score` is only
assigned at internal nodes. At depth 1 every child is a leaf, so all `Score`s stay at the value
copied from the root (0), and the selection loop's `>=` picks the **last** legal move in scan
order, whatever the evaluation said:

```
child Node.Score values seen by the selection loop: 0 0 0 0 0 0 0 0 0 0
heuristic value of each move:                       -5 20 20 40 5 20 20 20 10 30
chosen move: (7,5) = LAST entry;  best by heuristic: (5,0) score 40
```

At depth >= 2, `Score` and the return value disagree whenever a node has no children: the function
returns `INT_MIN` / `INT_MAX` but `Score` keeps the parent's value, and the `INT_MAX` return then
triggers the only "cutoffs" the pruning ever produces (finding 8), which stops the root from
evaluating its remaining children at all.

### 3.5 Alpha-beta pruning is a no-op (`AI.cpp:173-175`, `188-190`)

`alpha` and `beta` are members of each `Node`, initialised to `INT_MIN`/`INT_MAX` in the
constructor, and never passed from parent to child. At a Max node `beta` is therefore always
`INT_MAX`, so `beta <= alpha` can only be true when a child returns `INT_MAX`; symmetrically for
Min nodes. Measured over 20 mid-game positions:

| depth | nodes built | nodes visited | cutoffs |
|---|---|---|---|
| 2 | 4 085 | 4 085 | 0 |
| 3 | 61 555 | 61 549 | 4 |
| 4 | 930 712 | 930 071 | 64 |

Even a working cutoff would save nothing here, because `treeConstruct()` has already built and
stored every node (with a full `Board` copy in each) before `minimax()` starts. The `Node` class,
the tree, `deleteTree()` and the alpha/beta members are all unnecessary: a standard recursive
negamax needs none of them.

### 3.6 Unbounded cost

The depth spin box has a minimum of 1 and no maximum. Measured from a mid-game position, one move:

| depth | nodes | time | peak RSS |
|---|---|---|---|
| 3 | 2 579 | 6 ms | 10 MB |
| 4 | 22 684 | 49 ms | 23 MB |
| 5 | 148 604 | 315 ms | 107 MB |
| 6 | 709 119 | 1 478 ms | 482 MB |

Growth is about 5x per ply, so depth 8 would need on the order of 10 GB, all allocated on the GUI
thread while the window is frozen. (Note these trees are already smaller than a correct search
would produce, because of 3.2.)

### 3.7 Net effect on playing strength

100 games per row against a uniformly random mover, colours alternated, passes handled by the
harness (the game itself cannot handle them):

| player | W | L | D |
|---|---|---|---|
| repo EASY (depth 1) | 56 | 43 | 1 |
| repo MEDIUM (depth 2) | 77 | 20 | 3 |
| repo HARD (depth 3) | 76 | 16 | 8 |
| repo UNBEATABLE (depth 4) | 76 | 21 | 3 |
| correct minimax, depth 1, same heuristic with 3.3 fixed | 85 | 13 | 2 |
| correct minimax, depth 3, same heuristic with 3.3 fixed | 86 | 12 | 2 |

Easy is a coin flip against random play. Medium, Hard and Unbeatable are indistinguishable from
one another. Head-to-head, 20 games each with the first two plies randomised:

| match | repo wins | reference wins |
|---|---|---|
| repo UNBEATABLE (depth 4) vs correct depth 1 | 0 | 20 |
| repo UNBEATABLE (depth 4) vs correct depth 2 | 1 | 19 |

The reference used in these rows is the textbook algorithm with the repo's own square weights and
corner term; the only differences are alternating colours, a correct opponent-disk sign, and
returning the evaluation instead of discarding it.

### 3.8 Smaller AI issues

- `computeNextMove()` roots the search on `board.getCurrentPlayer()` (`AI.cpp:120`) while
  evaluating from `player`'s perspective (`AI.cpp:5-6`), and `MainWindow` then applies the result as
  a fixed colour. All three agree today only because nothing enforces it (finding 11).
- `corners()` and `squareWeights()` copy the whole 8x8 vector-of-vectors on every call
  (`auto boardVector = board.getBoard();`, `AI.cpp:21,62`); `const auto&` is what was meant. The
  local arrays in those functions shadow the member functions of the same name.
- `Node::Node()` (`Node.cpp:3-11`) assigns every member to itself, reading uninitialised `color`
  and `Score`. It is dead code, but cppcheck and `-Wshadow` both flag it.
- `Node(..., double Score)` narrows to `int Score` (`Node.cpp:22`).
- `Node` owns raw `Node*` children and relies on `deleteTree()`; a `std::bad_alloc` at depth 7
  would leak the partial tree. (ASan found no leaks on the normal path.)
- `QThread::msleep(1000 - elapsed)` (`AI.cpp:141-143`) is a UX delay implemented as a hard block of
  the GUI thread. If a minimum think time is wanted, arm a `QTimer` and emit from its slot.

## 4. GUI

**Everything runs on the GUI thread.** `AI` is a `QObject` with signals and slots and `AI.h`
includes `<QThread>`, which suggests a worker-thread design was intended, but the objects are
never moved to a thread, so every `emit whiteComputeNextMove()` is a direct call. In the scripted
session, after switching both sides to AI and clicking the board, the board pixels were sampled
once per second for 40 s: one distinct frame. The whole game executed inside a single
`mousePressEvent`, `update()` requests queued up unprocessed, and the process then crashed
(finding 1). Human-vs-AI "works" only because `humanPlayed()` calls `repaint()` synchronously
before invoking the AI, and the user experiences a one-second freeze per move.

**Hidden start conditions.** Nothing triggers an AI move except the tail of `humanPlayed()` or
the tail of the other AI's move. So: an AI-vs-AI game starts only when the user clicks somewhere
on the board; switching White from Human to AI when it is White's turn does nothing until the board
is clicked; and Restart with Black set to AI shows a board that never moves. None of this is
documented or indicated.

**Dispatch by display string.** `whitePlayerSelectionButtonClicked()` compares `button->text()`
to `"AI"`; the difficulty slots compare the combo text to `"Easy"`, `"Medium"`, and so on. Any
label change or translation silently breaks the logic. Use `QButtonGroup::idClicked` with ids, and
`currentIndexChanged` with `itemData`, or just separate slots.

**Two of everything.** `MainWindow` has seven white/black member pairs, six white/black slot pairs,
a signal pair, and a 170-line constructor in which nearly every statement appears twice with the
colour name changed. A `PlayerPanel`
widget (radio buttons, difficulty, three spin boxes, emitting typed signals) instantiated twice, and
a single `aiMoveComputed(PlayerColor, Move)` slot, would halve the file and remove the copy-paste
bug in finding 17.

**Rendering.** The board is `setFixedSize(801, 801)` with a hard-coded `scale(100, 100)`, so it
cannot be resized and ignores display scaling. `paintEvent` copies the board vector each frame and
draws the grid as 14 hand-written `drawLine` calls. `mousePressEvent` converts a pixel to a cell
with a 16-branch if/else ladder instead of `p.x() / 100`, ignores which mouse button was pressed,
and emits a `Move{8, 8}` for out-of-range clicks that only `doMove()`'s validation catches.

**Small things.** Initial labels are "White Player: 0" / "Black Score: 0" rather than 2 / 2 (only
fixed after the first move). Depth / weight controls stay enabled when the player is Human. Corner
and square weights have a minimum of 1, so the two heuristic terms cannot be switched off for
experimentation. `Move` is used as a signal argument without `Q_DECLARE_METATYPE`; it works only
because every connection is direct, and will fail the moment the AI is moved to a thread. Slot
parameters in `MainWindow.h:80-86` are named `depth` for the weight setters.

## 5. Code quality

- **Duplication** is the dominant smell: `Board.cpp` is 400 lines that should be about 100;
  `MainWindow.cpp` is 320 lines that should be about 150; `BoardWidget::mousePressEvent` is 75
  lines for two divisions.
- **Comments.** Most Doxygen blocks are Qt Creator stubs (`@brief corners`, `@param board`,
  `@return`) with no content; three different functions in `AI.h` carry the identical
  `@brief corners` block.
- **Naming and style.** Members `State`, `Score`, parameter `Max`; `whiteAIDifficulyChanged`;
  mixed tabs and spaces (`AI.cpp:131-134`); a mis-indented closing brace in `corners()`; typos in
  identifiers and comments ("reprenting", "polayer", "choosen", "calcualting", "algorith").
- **Headers.** `Node.h` has no include guard and a `using namespace std;` that leaks into every
  translation unit including `AI.h` and `MainWindow.h`. `Board.h` includes `<QObject>` for a class
  that is not a `QObject` and omits `<vector>` for the one it uses.
- **Warnings.** `-Wshadow` flags every constructor and the two heuristic functions;
  `-Wconversion` flags `Node.cpp:22` (double to int) and `AI.cpp:139` (qint64 to int). None of
  these are enabled in the `.pro`.
- **Tests.** None. The fuzz test in `harness/board_fuzz.cpp` is about 80 lines and found three of
  the findings above on its first run.

## 6. Recommendations, in order

1. **Make it build.** Add the missing includes (`<climits>`, `<algorithm>`, `<vector>`,
   `<QElapsedTimer>`, `<QDebug>`), fix the file-name case in the `.pro`, delete `.vscode/`, add a
   README with the Qt 6 requirement and build steps, and turn on `-Wall -Wextra -Wshadow`.
2. **Give `Board` the missing rules.** `hasLegalMove(color)`, `pass()`, `isGameOver()`,
   `winner()`; make `doMove` reject a move by the wrong colour; dedupe `getValidMoves` (or generate
   by scanning empty squares instead of opponent disks, which produces each move once); make the
   query methods `const`; replace the 16 pasted direction blocks with a direction table; consider
   `std::array`.
3. **Rewrite the search.** A recursive negamax with alpha-beta over a `Board` value, alternating
   the colour each ply, handling "no legal move" as a pass node and "neither side can move" as a
   terminal node. No `Node` class, no materialised tree, no per-node alpha/beta. Fix the
   `opponentDisk` ternary. Cap depth in the UI (6 is already 1.5 s per move). This is under 60 lines
   and, per section 3.7, turns a player that loses 20-0 to one-ply greedy into one that beats it.
4. **Get the AI off the GUI thread.** `QtConcurrent::run` or a `QThread` worker with
   `Q_DECLARE_METATYPE(Move)`; disable board input while a move is being computed; replace
   `msleep` with a `QTimer` if a visible delay is wanted; drive AI-vs-AI from the "move computed"
   slot rather than from mouse clicks.
5. **Fix the game flow.** Show whose turn it is, announce passes, show the winner, and auto-start
   the AI on Restart and on Human-to-AI switches.
6. **Collapse the duplication in the GUI**: a `PlayerPanel` widget and colour-parameterised slots;
   id-based radio and combo handling; `x / cellSize` in `mousePressEvent`; a resizable board that
   derives the cell size from `width()`.
7. **Keep the tests.** `board_fuzz.cpp` is a ready-made regression test; add a QtTest or Catch2
   target and a GitHub Actions workflow that installs Qt 6 and runs it. Then a build break like
   finding 2 can never reach `main` again.

