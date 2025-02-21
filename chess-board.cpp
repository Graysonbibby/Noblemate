#ifndef PIECES
#include "pieces.cpp"
#endif

#include <memory>
#include <typeinfo>

class chessBoard {
    private:
        std::vector<std::vector<Piece*>> chessboard;
        Color _playerColor = White;
        int chessBoardSize = 8;

        bool areCoordsValid(int x, int y){
            return x >= 0 && y >= 0 && x < chessBoardSize && y < chessBoardSize;
        }
    public:
        //Constrcutor for the chessboard (using pointers and must dereference them when the pieces move)
        chessBoard(){
            chessboard = {
                {
                    new Rook(White), new Knight(White), new Bishop(White), new King(White),
                    new Queen(White), new Bishop(White), new Knight(White), new Rook(White)
                },
                {
                    new Pawn(White), new Pawn(White), new Pawn(White), new Pawn(White),
                    new Pawn(White), new Pawn(White), new Pawn(White), new Pawn(White)
                },
                {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
                {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
                {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
                {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
                {
                    new Pawn(Black), new Pawn(Black), new Pawn(Black), new Pawn(Black),
                    new Pawn(Black), new Pawn(Black), new Pawn(Black), new Pawn(Black)
                },
                {
                    new Rook(Black), new Knight(Black), new Bishop(Black), new King(Black),
                    new Queen(Black), new Bishop(Black), new Knight(Black), new Rook(Black)
                },
            };
        }//end of chessBoard() constructor

        //method to get the current player's color (whose move it is)
        Color get_playerColor(){
            return _playerColor;
        }//end get_playerColor()

        //Returns FENChar vector of board position based off of chessboard
        std::vector<std::vector<FENChar>> chessBoardView(){
            std::vector<std::vector<FENChar>> FENChar_chessboard(chessBoardSize, std::vector<FENChar>(chessBoardSize));
            for (int i = 0; i < chessBoardSize; i++){
                for (int j = 0; j < chessBoardSize; j++){
                    if (chessboard [i][j] == NULL){
                        FENChar_chessboard[i][j] = None;
                    } else {
                        FENChar_chessboard[i][j] = (*chessboard[i][j]).get_FENChar();
                    }
                }
            }
            return FENChar_chessboard;
        }//end FENChar chessBoardView()

        //Getter and setter for chessBoardSize variable
        int getChessBoardSize(){
            return chessBoardSize;
        }
        void setChessBoardSize(int size){
            chessBoardSize = size;
        }//end chessBoardSize getter and setter

        //Method to check if the king of the current playerColor is in check
        bool isInCheck(Color playerColor){
            for (int i = 0; i < chessBoardSize; i++){
                for (int j = 0; j < chessBoardSize; j++){
                    Piece* piece = chessboard[i][j];
                    if (piece == NULL || (*piece).get_color() == playerColor) continue;

                    for(Coords k : (*piece).get_directions()){
                        int newX = i + k.x;
                        int newY = j + k.y;

                        if (!areCoordsValid(newX, newY))continue;

                        if ((*piece).get_pieceType()){

                        }
                    }
                }
            }

            return false;
        }//end isIncheck
};

int main(){
    chessBoard chess = chessBoard();
    std::vector<std::vector<FENChar>> FENChar_chessboard;
    FENChar_chessboard = chess.chessBoardView();
    char character_chessboard[8][8];

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            std::cout << FENChar_chessboard[i][j];
        }
        std::cout << "\n";
    }
    return 0;
}