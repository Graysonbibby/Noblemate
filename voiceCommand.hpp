#ifndef VOICE_COMMAND
#define VOICE_COMMAND

#include "chess-board.hpp"
#include "Python.h"

#ifndef MOVE
#define MOVE
    struct Move {
        std::string piece;
        std::string startSquare;
        std::string endSquare;
    };
#endif


Move moveInput();
Coords voiceCommandProcessing(std::string square);
std::string voiceInput(const char* pythonModuleName);

#endif
