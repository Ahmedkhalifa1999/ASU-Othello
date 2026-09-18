# Review harness

Scaffolding used to produce the numbers in `../REVIEW.md`. It is **not** part of the
application build and is deliberately hacky (it exposes private members so the search
internals can be inspected without editing the real sources).

## board_fuzz.cpp

Fuzz-compares `Board.cpp` against an independent reference Othello implementation.
Needs only a compiler; `Board.h` includes `<QObject>` but never uses it, so a stub is enough:

```sh
mkdir -p fakeqt && printf '#include <vector>\n' > fakeqt/QObject
g++ -std=c++17 -O1 -Ifakeqt -I../.. board_fuzz.cpp ../../Board.cpp -o board_fuzz && ./board_fuzz
```

## ai_probe.cpp

Drives `AI`/`Node`/`Board` directly. It needs a patched copy of the sources
(`<climits>` so the code compiles at all, and `public:` on `Board` so the probe can set
up positions):

```sh
mkdir -p src && cp ../../*.cpp ../../*.h src/
sed -i '1i #include <climits>' src/Node.h
perl -0pi -e 's/class Board\n\{/class Board\n{\npublic:/' src/Board.h
qmake6 ai_probe.pro && make
./ai_probe            # sections 1-7
./ai_probe crash      # calls the real computeNextMove() on a position with no legal move
./ai_probe cost 5     # tree size / time / RSS for one move at depth 5
./ai_probe ref        # correct reference minimax vs random, and vs the repo's UNBEATABLE
./ai_probe real       # calls the real slot three times (use with an ASan build for leak checking)
```

## gui_drive.py

Scripted GUI session under Xvfb (`xdotool` + ImageMagick): plays a human move, switches
white to AI, then switches black to AI and samples the board pixels once per second while
recording the process exit status. Expects the built binary at `$SP/build-warn/ASU-Othello`
and a running `Xvfb :99`.
