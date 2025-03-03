#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP

#include "pieces.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

// Supporting structs for the chess board.
struct LastMove {
    Piece* piece = nullptr;
    int prevX;
    int prevY;
    int currX;
    int currY;
};

struct KingChecked {
    int x = 10;
    int y = 10;
};

struct AllPieces {
    Piece* piece;
    int x;
    int y;
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
    bool _isGameOver = false;
    std::string _gameOverMessage;
    double fiftyMoveRuleCounter = 0;
    int fullNumberOfMoves = 1;

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
public:
    chessBoard();
    Color get_playerColor();
    std::vector<std::vector<FENChar>> chessBoardView();
    std::vector<SafeSquares> get_safeSquares();
    int getChessBoardSize();
    void setChessBoardSize(int size);
    LastMove get_lastMove();
    bool get_isInCheck();
    KingChecked get_checkState();
    bool get_pawnPromoted();
    void set_pawnPromoted();
    std::string get_gameOverMessage();
    bool get_isGameOver();
    bool isSquareDark(int x, int y);
    bool isInCheck(Color playerColor, bool checkingCurrentPosition);
    void move(int prevX, int prevY, int newX, int newY);
    bool isSquareLastMove(int x, int y);
    bool isSquareChecked(int x, int y);
};

#endif // CHESS_BOARD_HPP
