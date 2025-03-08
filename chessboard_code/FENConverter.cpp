#include "chess-board.hpp"

std::string castlingPossibilitiesFunc(std::vector<std::vector<Piece*>> board, Color color) {
    std::string castlingAvailability = "";
    int kingPositionX = color == White ? 0 : 7;
    
    Piece* king = board[kingPositionX][4];

    if(king && (*king).get_pieceType() == "king" && !(*king).get_hasMoved()){
        int rookPositionX = kingPositionX;
        Piece* kingSideRook = board[rookPositionX][7];
        Piece* queenSideRook = board[rookPositionX][0];

        if(kingSideRook && (*kingSideRook).get_pieceType() == "rook" && !(*kingSideRook).get_hasMoved()){
            if(color == White) castlingAvailability+="K";
            else castlingAvailability+="k";
        }

        if(queenSideRook && (*queenSideRook).get_pieceType() == "rook" && !(*queenSideRook).get_hasMoved()){
            if(color == White) castlingAvailability+="Q";
            else castlingAvailability += "q";
        }

    }

    return castlingAvailability;
}

std::string castlingAvailability(std::vector<std::vector<Piece*>> board){
    std::string castlingPossibilities = castlingPossibilitiesFunc(board, White) + castlingPossibilitiesFunc(board, White);
    return (castlingPossibilities != "") ? castlingPossibilities : "-";
}

std::string enPassantPossibility(LastMove* lastMove, Color color){
    if(!lastMove) return "-";
    Piece* piece = (*lastMove).piece;
    int newX, newY, prevX, prevY;

    newX = (*lastMove).currX;
    newY = (*lastMove).currY;
    prevX = (*lastMove).prevX;
    prevY = (*lastMove).prevY;

    if(piece && (*piece).get_pieceType() == "pawn" && abs(newX - prevX) == 2){
        int row = color == White ? 6 : 3;
        return columns[prevY] + std::to_string(row);
    }
    return "-";
}

std::string convertBoardToFEN(
    std::vector<std::vector<Piece*>> board, Color playerColor, LastMove* lastMove, int fiftyMoveRuleCounter,
    int numberOfFullMoves
) {
    std::string FEN = "";
    std::string consecutiveEmptySquaresCounterString;

    for(int i = 7; i >= 0; i--){
        std::string FENRow = "";
        int consecutiveEmptySquaresCounter = 0;

        for(Piece* piece : board[i]){
            if(!piece) {
                consecutiveEmptySquaresCounter++;
                continue;
            }

            if(consecutiveEmptySquaresCounter != 0){
                std::string str(1, consecutiveEmptySquaresCounter);
                consecutiveEmptySquaresCounterString = std::to_string(consecutiveEmptySquaresCounter);
                FENRow += consecutiveEmptySquaresCounterString;
            }

            consecutiveEmptySquaresCounter = 0;
            FENRow += (*piece).get_FENChar();
        }
        consecutiveEmptySquaresCounterString = std::to_string(consecutiveEmptySquaresCounter);
        if(consecutiveEmptySquaresCounter != 0) FENRow += consecutiveEmptySquaresCounterString;
        FEN += (i==0) ? FENRow : FENRow + "/";
    }

    std::string player = playerColor == White ? "w" : "b";

    FEN += " " + player;
    FEN += " " + castlingAvailability(board);
    FEN += " " + enPassantPossibility(lastMove, playerColor);
    FEN += " " + std::to_string(fiftyMoveRuleCounter);
    FEN += " " + std::to_string(numberOfFullMoves);

    return FEN;
}
