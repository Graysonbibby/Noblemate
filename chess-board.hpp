#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP

#define _CRT_SECURE_NO_WARNINGS

#include "pieces.hpp"
#include "serial_communication.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <iostream>
#include "voiceCommand.hpp"

    
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

int initializeComm();
void sendData(int serialPort, const char* sent_string);
std::string receiveData(int serialPort);

// Supporting structs for the chess board.
struct LastMove {
    Piece* piece = nullptr;
    int prevX;
    int prevY;
    int currX;
    int currY;
    
    LastMove(Piece* p, int prevX, int prevY, int currX, int currY)
        : piece(p), prevX(prevX), prevY(prevY), currX(currX), currY(currY) {}
        
    LastMove() = default;
};

struct KingChecked {
    int x = 10;
    int y = 10;
    
    KingChecked(int x, int y) : x(x), y(y) {}
    
    KingChecked() = default;
};

struct AllPieces {
    Piece* piece;
    int x;
    int y;
};

#ifndef MOVE
#define MOVE
    struct Move {
        std::string piece;
        std::string startSquare;
        std::string endSquare;
    };
#endif

struct MoveDict {
    std::vector<std::string> FENWhite;
    std::vector<std::string> FENBlack;
};

// Function to convert a character to the FENChar enum.
FENChar character_to_FENChar(char c);

// Declaration of the chessBoard class.
class chessBoard {
private:
    std::vector<std::vector<Piece*>> chessboard;
    Color _playerColor = White;
    int chessBoardSize = 8;

    std::vector<SafeSquares> _safeSquares;
    LastMove _lastMove;
    bool _isInCheck = false;
    KingChecked _checkState;
    bool _pawnPromoted = false;
    std::string _moveTypeFlag = "";
    std::string _moveCoords = "";
    std::string _checkmateFlag = "";
    std::string _promotionFlag = "";
    
    Color userColor = White;
    Color engineColor = Black;
    char enginePromotedPiece = 'q';

    bool _isGameOver = false;
    std::string _gameOverMessage;

    double fiftyMoveRuleCounter = 0;
    int fullNumberOfMoves = 1;

    std::unordered_map<std::string, int> repetitionCounts;
    bool threeFoldRepetitionFlag = false;

    std::string _boardAsFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


    // Private helper functions.
    bool areCoordsValid(int x, int y);
    bool isPositionSafeAfterMove(Piece* piece, int prevX, int prevY, int newX, int newY);
    std::vector<SafeSquares> findSafeSquares();
    bool canCastle(Piece* king, bool kingSideCastle);
    void handlingSpecialMoves(Piece* piece, int prevX, int prevY, int newX, int newY);
    bool canCaptureEnPassant(Piece* pawn, int pawnX, int pawnY);
    bool isGameFinished();
    bool playerHasOnlyTwoKnightsAndKing(std::vector<AllPieces> pieces);
    bool playerHasOnlyBishopsWithSameColorAndKing(std::vector<AllPieces> pieces);
    bool insufficientMaterial();
    void updateThreeFoldRepetitionDictionary(std::string FEN);
    void clearThreeFoldRepetitionDictionary();
public:
    chessBoard();
    Color get_playerColor();
    std::vector<std::vector<FENChar>> chessBoardView();
    std::vector<SafeSquares> get_safeSquares();
    void reset_board();
    int getChessBoardSize();
    void setChessBoardSize(int size);
    LastMove get_lastMove();
    bool get_isInCheck();
    KingChecked get_checkState();
    bool get_pawnPromoted();
    void set_pawnPromoted();
    std::string get_gameOverMessage();
    bool get_isGameOver();
    std::string get_FENBoard();
    void set_Colors(std::string color);
    Color get_EngineColor();
    Color get_UserColor();
    void set_moveTypeFlag(std::string moveType);
    void set_moveCoords(std::string moveCoords);
    void set_checkmateFlag(std::string checkmateFlag);
    void set_enginePromotedPiece(char promotedPiece);
    bool isSquareDark(int x, int y);
    bool isInCheck(Color playerColor, bool checkingCurrentPosition);
    void move(int prevX, int prevY, int newX, int newY);
    bool isSquareLastMove(int x, int y);
    bool isSquareChecked(int x, int y);
};

//defining FENConverter functions
std::string castlingPossibilitiesFunc(std::vector<std::vector<Piece*>> board, Color color);
std::string castlingAvailability(std::vector<std::vector<Piece*>> board);
std::string enPassantPossibility(LastMove* lastMove, Color color);
std::string convertBoardToFEN(std::vector<std::vector<Piece*>> board, Color playerColor, LastMove* lastMove, int fiftyMoveRuleCounter,int numberOfFullMoves);

//voice input stuff
Move voiceCommandInput();
Coords voiceCommandProcessing(std::string square);

#endif // CHESS_BOARD_HPP
