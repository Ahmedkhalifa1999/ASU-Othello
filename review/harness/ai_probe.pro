# Review-only probe: compiles the repository's AI/Board/Node against a patched copy in ./src (see README.md).
QT += core
QT -= gui
CONFIG += console c++17
CONFIG -= app_bundle
INCLUDEPATH += $$PWD/src
SOURCES += ai_probe.cpp src/AI.cpp src/Board.cpp src/Node.cpp
HEADERS += src/AI.h src/Board.h src/Node.h
QMAKE_CXXFLAGS += -O2 -g
