#include "chess-board.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
    //FUNCTIONS
// Implementation of the helper function.
FENChar character_to_FENChar(char c) {
    if(c == 'n') return BlackKnight;
    else if(c == 'N') return WhiteKnight;
    else if(c == 'r') return BlackRook;
    else if(c == 'R') return WhiteRook;
    else if(c == 'b') return BlackBishop;
    else if(c == 'B') return WhiteBishop;
    else if(c == 'q') return BlackQueen;
    else if(c == 'Q') return WhiteQueen;
    else if(c == 'k') return BlackKing;
    else if(c == 'K') return WhiteKing;
    else if(c == 'p') return BlackPawn;
    else if(c == 'P') return WhitePawn;
    else return None;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
    //PRIVATE chessBoard FUNCTIONS
bool chessBoard::areCoordsValid(int x, int y) {
    return x >= 0 && y >= 0 && x < chessBoardSize && y < chessBoardSize;
}

//to implement would probably loop through the board to check (probably used in capture or move function)
bool chessBoard::isPositionSafeAfterMove(Piece* piece, int prevX, int prevY, int newX, int newY){
    //moving some piece to newX and newY
    Piece* newPiece = chessboard[newX][newY];

    //checks if the piece is moving in the same square as an existing piece of the same color
    if(newPiece && (*newPiece).get_color() == (*piece).get_color()) return false;

    //simulate position
    Piece* originalPrev = chessboard[prevX][prevY];
    Piece* originalNew = chessboard[newX][newY];

    chessboard[prevX][prevY] = nullptr;
    chessboard[newX][newY] = piece;

    //checks if the player is in check after their move
    bool isPositionSafe = !isInCheck((*piece).get_color(), false);

    //restore position
    chessboard[prevX][prevY] = originalPrev;
    chessboard[newX][newY] = originalNew;

    return isPositionSafe;
}//end isPositionSafeAfterMove

std::vector<SafeSquares> chessBoard::findSafeSquares(){
    std::vector<SafeSquares> safe_squares;

    for (int i = 0; i < chessBoardSize; i++){//iterates through rows
        for (int j = 0; j < chessBoardSize; j++){//iterates through columns
            Piece* piece = chessboard[i][j];
            if(!piece || (*piece).get_color() != _playerColor) continue;

            std::vector<Coords> pieceSafeSquares;

            for(Coords k : (*piece).get_directions()){
                int dx = k.x;
                int dy = k.y;
                
                int newX = i + dx;
                int newY = j + dy;

                if(!areCoordsValid(newX, newY)) continue;

                Piece* newPiece = chessboard[newX][newY];

                if(newPiece && (*newPiece).get_color() == (*piece).get_color()) continue;

                //restricting pawn moves in certain directions
                if((*piece).get_pieceType() == "pawn"){
                    //can't move pawn two squares ahead if there is a piece obstructing it
                    if(dx == 2 || dx == -2){
                        if(newPiece) continue;
                        
                        int checkX = newX + ((dx == 2) ? -1 : 1);
                        if (!areCoordsValid(checkX, newY)) continue;
                        if(chessboard[checkX][newY]) continue;
                    }

                    //can't move pawn one square straight if piece is in front
                    if((dx == 1 || dx == -1) && dy == 0 && newPiece) continue;

                    //can't move pawn diagnoally if there is no piece diaganol to it or piece diagonal to it has the same color
                    if((dy == 1 || dy == -1) && (!newPiece || (*piece).get_color() == (*newPiece).get_color())) continue;
                }

                if ((*piece).get_pieceType() == "pawn" || (*piece).get_pieceType() == "knight" 
                    || (*piece).get_pieceType() == "king"){
                    
                    if(isPositionSafeAfterMove(piece, i, j, newX, newY)){
                        pieceSafeSquares.push_back({newX, newY});
                    }
                } else {
                    while(areCoordsValid(newX, newY)){
                        newPiece = chessboard[newX][newY];
                        //breaks once the queen/rook/bishop gets to another piece OR once it gets to the 
                        //end of the board (areCoordsValid condition breaks)
                        if(newPiece && (*newPiece).get_color() == (*piece).get_color()) break;
                        if(isPositionSafeAfterMove(piece, i, j, newX, newY)){
                            pieceSafeSquares.push_back({newX, newY});
                        }

                        //breaks once the newPiece reaches another piece of a different color
                        if (newPiece != nullptr) break;

                        newX += dx;
                        newY += dy;
                    }
                }
            }
            if((*piece).get_pieceType() == "king"){
                if(canCastle(piece, true))
                    pieceSafeSquares.push_back({i, 6});

                if(canCastle(piece, false))
                    pieceSafeSquares.push_back({i, 2});
            } 
            
            if((*piece).get_pieceType() == "pawn"){
                if(canCaptureEnPassant(piece, i, j)){
                    int pawnNewPositionX = i + (((*piece).get_color()) == White ? 1 : -1);
                    pieceSafeSquares.push_back({pawnNewPositionX, _lastMove.prevY});
                    int len_piecesafesquares = pieceSafeSquares.size();
                    std::cout << "En passant move: ";
                    std::cout << pieceSafeSquares[len_piecesafesquares-1].y+1 << ", " << pieceSafeSquares[len_piecesafesquares-1].x+1 << "\n";
                }
            }

            if(pieceSafeSquares.size()){ 
                Coords result;
                result.x = i;
                result.y = j;
                safe_squares.push_back({result, pieceSafeSquares});
            }
        }
    }

    return safe_squares;
}//end safe_squares

//determines if castling is possible
bool chessBoard::canCastle(Piece* king, bool kingSideCastle){
    if((*king).get_pieceType() != "king") return false;
    if((*king).get_hasMoved()) return false;
    
    int kingPositionX = (*king).get_color() == White ? 0 : 7;
    int kingPositionY = 4;
    int rookPositionX = kingPositionX;
    int rookPositionY = kingSideCastle ? 7 : 0;
    Piece* rook = chessboard[rookPositionX][rookPositionY];

    if(!rook || (*rook).get_pieceType() != "rook") return false;

    if((*rook).get_hasMoved() || _isInCheck) return false;

    //this way we can verify that both positions are not threatened in any manner
    int firstNextKingPositionY = kingPositionY + (kingSideCastle ? 1:-1);
    int secondNextKingPositionY = kingPositionY + (kingSideCastle ? 2:-2);
    
    //making sure no piece is blocking the path
    if(chessboard[kingPositionX][firstNextKingPositionY] || chessboard[kingPositionX][secondNextKingPositionY]) return false;
    if(!kingSideCastle && chessboard[kingPositionX][1]) return false;
    //making sure both squares are safe
    return isPositionSafeAfterMove(king, kingPositionX, kingPositionY, kingPositionX, firstNextKingPositionY) && isPositionSafeAfterMove(king, kingPositionX, kingPositionY, kingPositionX, secondNextKingPositionY);
}//end canCastle

//handles special moves
void chessBoard::handlingSpecialMoves(Piece* piece, int prevX, int prevY, int newX, int newY){
    if(!piece) return;

    if((*piece).get_pieceType() == "king" && abs(newY - prevY) == 2){
        //newY > prevY == kingside castle
        int rookPositionX = prevX;
        int rookPositionY = (newY > prevY) ? 7 : 0;
        Piece* rook = chessboard[rookPositionX][rookPositionY];
        int rookNewPositionY = (newY > prevY) ? 5:3;
        chessboard[rookPositionX][rookPositionY] = nullptr;
        chessboard[rookPositionX][rookNewPositionY] = rook;
        (*rook).set_hasMoved();
    }
    
    if((*piece).get_pieceType() == "pawn" && _lastMove.piece){
        if((*_lastMove.piece).get_pieceType() == "pawn"
            && abs(_lastMove.currX - _lastMove.prevX) == 2
            && prevX == _lastMove.currX
            && newY == _lastMove.currY){
            
            Piece* capturedPawn = chessboard[_lastMove.currX][_lastMove.currY];
            if(capturedPawn){
                chessboard[_lastMove.currX][_lastMove.currY] = nullptr;
                delete capturedPawn;
            }
        }
    }
}//end handlingSpecialMoves()

//determines if enPassant capture can occur
bool chessBoard::canCaptureEnPassant(Piece* pawn, int pawnX, int pawnY){
    if(!pawn) return false;
    if((*pawn).get_pieceType() != "pawn") return false;
    if(!(_lastMove.piece)) return false;

    Piece* piece = _lastMove.piece;

    int prevX, prevY, currX, currY;///START DEBUGGING FROM HERE!!!!
    prevX = _lastMove.prevX; 
    prevY = _lastMove.prevY;
    currX = _lastMove.currX;
    currY = _lastMove.currY;

    if((*piece).get_pieceType() != "pawn" || (*pawn).get_color() != _playerColor || abs(currX-prevX) != 2
        || pawnX != currX || abs(pawnY - currY) != 1){
            
        return false;
    }
    
    int pawnNewPositionX = pawnX + ((*pawn).get_color() == White ? 1 : -1);//pawn direction depends on color
    int pawnNewPositionY = currY;//pawn moves into the same rank as the pawn it is capturing

    Piece* originalPrev = chessboard[currX][currY];
    chessboard[currX][currY] = nullptr;
    bool isPositionSafe = isPositionSafeAfterMove(pawn, pawnX, pawnY, pawnNewPositionX, pawnNewPositionY);
    chessboard[currX][currY] = originalPrev;

    return isPositionSafe;
}//end canCaptureEnPassant

//isGameFinished method
bool chessBoard::isGameFinished(){
    if(insufficientMaterial()){
        _gameOverMessage = "Draw due to insufficient material";
        return true;
    }

    if(!_safeSquares.size()){
        if(_isInCheck){
            std::string prev_player = _playerColor == White ? "Black" : "White";
            _gameOverMessage = prev_player + " won by checkmate!";
        } else {
            _gameOverMessage = "Stalemate";
        }
        return true;
    }

    if(threeFoldRepetitionFlag){
        _gameOverMessage = "Draw due to three move repetition!";
        return true;
    }

    if(fiftyMoveRuleCounter == 50){
        _gameOverMessage = "Draw due to fifty move rule!";
        return true;
    }
    return false;
}//end isGameFinished method

bool chessBoard::playerHasOnlyTwoKnightsAndKing(std::vector<AllPieces> piece){
    int numKnights = 0;
    for(int i = 0; i < piece.size(); i++){
        if((*piece[i].piece).get_pieceType() == "knight"){
            numKnights++;
        }
    }
    return (numKnights == 2) ? true : false;
}

bool chessBoard::playerHasOnlyBishopsWithSameColorAndKing(std::vector<AllPieces> piece){
    std::vector<AllPieces> bishops;
    bool allBishopsSameColor = true;
    for(int i = 0; i < piece.size(); i++){
        if((*piece[i].piece).get_pieceType() == "bishop") bishops.push_back(piece[i]);
    }

    for(int i = 0; i < bishops.size()-1; i++){
        if(isSquareDark(bishops[i].x, bishops[i].y) != isSquareDark(bishops[i+1].x, bishops[i+1].y)) return false;
    }

    return (bishops.size() == piece.size()-1) ? true : false;
}

//checking for insufficient material
bool chessBoard::insufficientMaterial() {
    std::vector<AllPieces> whitePieces;
    std::vector<AllPieces> blackPieces;

    //checking what pieces remain
    for(int x = 0; x < chessBoardSize; x++){
        for(int y = 0; y < chessBoardSize; y++){
            Piece* piece = chessboard[x][y];
            if(!piece) continue;

            if((*piece).get_color() == White) whitePieces.push_back({piece, x, y});
            else blackPieces.push_back({piece, x, y});
        }
    }

    //king vs king
    if(whitePieces.size() == 1 && blackPieces.size() == 1) return true;

    //king and minor piece vs king
    if(whitePieces.size() == 1 && blackPieces.size() == 2){
        for(int i = 0; i < blackPieces.size(); i++){
            if((*blackPieces[i].piece).get_pieceType() == "bishop" || (*blackPieces[i].piece).get_pieceType() == "knight")
                return true;
        }
    }
    else if(whitePieces.size() == 2 && blackPieces.size() == 1){
        for(int i = 0; i < whitePieces.size(); i++){
            if((*whitePieces[i].piece).get_pieceType() == "bishop" || (*whitePieces[i].piece).get_pieceType() == "knight")
                return true;
        }
    }
    
    //both sides have bishop of same color
    else if(whitePieces.size() == 2 && blackPieces.size() == 2){
        AllPieces* whiteBishop = nullptr;
        AllPieces* blackBishop = nullptr;
        for(int i = 0; i < whitePieces.size(); i++){ 
            if((*whitePieces[i].piece).get_pieceType() == "bishop") whiteBishop = &(whitePieces[i]);
        }

        for(int i = 0; i < blackPieces.size(); i++){
            if((*blackPieces[i].piece).get_pieceType() == "bishop") blackBishop = &(blackPieces[i]);
        }

        if(whiteBishop && blackBishop) {
            bool areBishopsOfSameColor = isSquareDark((*whiteBishop).x, (*whiteBishop).y) && isSquareDark((*blackBishop).x, (*blackBishop).y) ||
                                             !isSquareDark((*whiteBishop).x, (*whiteBishop).y) && !isSquareDark((*blackBishop).x, (*blackBishop).y);
                
            return areBishopsOfSameColor;
        }
    }

    //one king and two knights
    if(whitePieces.size() == 3 && blackPieces.size() == 1 && playerHasOnlyTwoKnightsAndKing(whitePieces) ||
       whitePieces.size() == 1 && blackPieces.size() == 3 && playerHasOnlyTwoKnightsAndKing(blackPieces)
      ) return true;

    //one king and many bishops of same color
    if(whitePieces.size() >= 3 && blackPieces.size() == 1 && playerHasOnlyBishopsWithSameColorAndKing(whitePieces) ||
       whitePieces.size() == 1 && blackPieces.size() >= 3 && playerHasOnlyBishopsWithSameColorAndKing(blackPieces)
      ) return true;
    
    return false;    
        
}//end insufficientMaterial

void chessBoard::updateThreeFoldRepetitionDictionary(std::string FEN){
    std::vector<std::string> threeFoldString;
    std::string threeFoldRepetitionFENKey = "";

    int start_index = 0;
    for(int i = 0; FEN[i] != '\0'; i++){
        if(FEN[i] == ' '){
            threeFoldString.push_back(FEN.substr(start_index, i-start_index));
            start_index = i+1;
        }
    }

    for(int i = 0; i < 4; i++){
        threeFoldRepetitionFENKey += threeFoldString[i];
    }

    if(repetitionCounts.find(threeFoldRepetitionFENKey) == repetitionCounts.end()){
        repetitionCounts[threeFoldRepetitionFENKey] = 1;
    } else {
        repetitionCounts[threeFoldRepetitionFENKey] += 1;
    }

    if(repetitionCounts[threeFoldRepetitionFENKey] >= 3){
        threeFoldRepetitionFlag = true;
    }
}

void chessBoard::clearThreeFoldRepetitionDictionary(){
    repetitionCounts.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
    //PUBLIC chessBoard FUNCTIONS
chessBoard::chessBoard(){
    chessboard = {
        {
            new Rook(White), new Knight(White), new Bishop(White), new Queen(White),
            new King(White), new Bishop(White), new Knight(White), new Rook(White)
        },
        {
            new Pawn(White), new Pawn(White), new Pawn(White), new Pawn(White),
            new Pawn(White), new Pawn(White), new Pawn(White), new Pawn(White)
        },
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
        {
            new Pawn(Black), new Pawn(Black), new Pawn(Black), new Pawn(Black),
            new Pawn(Black), new Pawn(Black), new Pawn(Black), new Pawn(Black)
        },
        {
            new Rook(Black), new Knight(Black), new Bishop(Black), new Queen(Black),
            new King(Black), new Bishop(Black), new Knight(Black), new Rook(Black)
        },
    };
    _safeSquares = findSafeSquares();
}//end of chessBoard() constructor

////BEGINNING OF GETTERS AND SETTERS METHODS

//method to get the current player's color (whose move it is)
Color chessBoard::get_playerColor(){
    return _playerColor;
}//end get_playerColor()

//Returns FENChar vector of board position based off of chessboard
std::vector<std::vector<FENChar>> chessBoard::chessBoardView(){
    std::vector<std::vector<FENChar>> FENChar_chessboard(chessBoardSize, std::vector<FENChar>(chessBoardSize));
    for (int i = 0; i < chessBoardSize; i++){//iterates through rows
        for (int j = 0; j < chessBoardSize; j++){//iterates through columns
            if (chessboard [i][j] == nullptr){
                FENChar_chessboard[i][j] = None;
            } else {
                FENChar_chessboard[i][j] = (*chessboard[i][j]).get_FENChar();
            }
        }
    }
    return FENChar_chessboard;
}//end FENChar chessBoardView()

//with this we can get all of the safe squares for all of the pieces of the current player's color
//that contains the x and y coordinates of the piece position. 
//To find where the piece can go, the player only needs to enter in the piece's coordinates (need a 
//board position to coordinate converter to map all e4, a2, etc. positions to actual coordinates on
//the board)
//From that, the player can receive all of the available corrdinates for that given piece (need a function
//to take those coordinates and output them as board position values and a function to link those values
//to LEDs on the board if that is something we are implementing)
std::vector<SafeSquares> chessBoard::get_safeSquares(){
    return _safeSquares;
}

//Getter and setter for chessBoardSize variable
int chessBoard::getChessBoardSize(){
    return chessBoardSize;
}
void chessBoard::setChessBoardSize(int size){
    chessBoardSize = size;
}//end chessBoardSize getter and setter

LastMove chessBoard::get_lastMove(){
    return _lastMove;
}//end get_lastMove()

//is in check getter
bool chessBoard::get_isInCheck(){
    return _isInCheck;
}//end is in check getter

// checkState getter
KingChecked chessBoard::get_checkState(){
    return _checkState;
}//end checkState getter

//pawn promotion getter and setters
bool chessBoard::get_pawnPromoted(){
    return _pawnPromoted;
}
void chessBoard::set_pawnPromoted(){
    _pawnPromoted = true;
}//end pawn promotion getter and setter

//getters for game over conditions
std::string chessBoard::get_gameOverMessage(){
    return _gameOverMessage;
}
bool chessBoard::get_isGameOver(){
    return _isGameOver;
}

//getter for FENString representation of board
std::string chessBoard::get_FENBoard(){
    return convertBoardToFEN(chessboard, _playerColor, (&_lastMove), fiftyMoveRuleCounter, fullNumberOfMoves);
}

////END OF GETTER AND SETTER METHODS

bool chessBoard::isSquareDark(int x, int y){    
    return x % 2 == 0 && y % 2 == 0 || x % 2 == 1 && y % 2 == 1;
} 

//Method to check if the king of the current playerColor is in check
bool chessBoard::isInCheck(Color playerColor, bool checkingCurrentPosition){
    for (int i = 0; i < chessBoardSize; i++){
        for (int j = 0; j < chessBoardSize; j++){
            Piece* piece = chessboard[i][j];
            if (piece == nullptr || (*piece).get_color() == playerColor) continue;

            for(Coords k : (*piece).get_directions()){
                int newX = i + k.x;
                int newY = j + k.y;

                if (!areCoordsValid(newX, newY))continue;

                //isolating pawn, king and knight because they can only attack adjacent squares
                //or all of their possible attacks are already defined in the _directions variable.
                if ((*piece).get_pieceType() == "pawn" || (*piece).get_pieceType() == "knight" 
                    || (*piece).get_pieceType() == "king"){
                        if ( (*piece).get_pieceType() == "pawn") continue;

                        //verifies that the current player moving is in check
                        Piece* attacked_piece = chessboard[newX][newY];
                        if (attacked_piece == nullptr) continue;

                        if((*attacked_piece).get_pieceType() == "king" && (*attacked_piece).get_color() == playerColor){
                            if(checkingCurrentPosition) {
                                _isInCheck = true;
                                _checkState = {newX, newY};
                            }
                            return true;
                        }
                }
                else {
                    while(areCoordsValid(newX, newY)){
                        //verifies that the current player moving is in check
                        Piece* attacked_piece = chessboard[newX][newY];
                        if(attacked_piece == nullptr) {
                            newX += k.x;
                            newY += k.y;
                            continue;
                        }

                        if((*attacked_piece).get_pieceType() == "king" && (*attacked_piece).get_color() == playerColor){
                            if(checkingCurrentPosition) {
                                _isInCheck = true;
                                _checkState = {newX, newY};
                            }
                            return true;
                        }

                        if(attacked_piece != nullptr) break;

                        newX += k.x;
                        newY += k.y;
                    }
                }
            }
        }
    }
    if(checkingCurrentPosition) _isInCheck = false;
    return false;
}//end isIncheck(Color playerColor)

//method to move pieces
void chessBoard::move(int prevX, int prevY, int newX, int newY){
    //if the coords are not valid then return
    if(!areCoordsValid(prevX, prevY) || !areCoordsValid(newX, newY)) return;

    //if the piece is not the same color as the current player then return
    Piece* piece = chessboard[prevX][prevY];
    if(!piece || (*piece).get_color() != _playerColor) return;

    std::vector<Coords> pieceSafeSquares;

    for(int i = 0; i < _safeSquares.size(); i++){
        if(prevX == _safeSquares[i].square.x && prevY == _safeSquares[i].square.y){
            pieceSafeSquares = _safeSquares[i].SafeSquare_coords;
        }
    }

    bool coordsMatch = false;
    if(!pieceSafeSquares.size()) return;
    for(int i = 0; i < pieceSafeSquares.size(); i++){
        if(pieceSafeSquares[i].x == newX && pieceSafeSquares[i].y == newY) coordsMatch = true;
    }

    if(!coordsMatch) {
        std::cout << "Square is not safe!\n";
        return;
    }

    if(((*piece).get_pieceType() == "pawn" || (*piece).get_pieceType() == "king" || 
    (*piece).get_pieceType() == "rook") && !(*piece).get_hasMoved()){
        (*piece).set_hasMoved();
    }

    //handling the fiftyMoveRuleCounter
    bool isPieceTaken = chessboard[newX][newY] != nullptr;
    if((*piece).get_pieceType() == "pawn" || isPieceTaken) fiftyMoveRuleCounter = 0;
    else fiftyMoveRuleCounter += 0.5;

    //special moves
    handlingSpecialMoves(piece, prevX, prevY, newX, newY);

    //deleting pieces that have been captured
    Piece* capturedPiece = chessboard[newX][newY];
    if(capturedPiece){
        chessboard[prevX][prevY] = nullptr;
        chessboard[newX][newY] = piece;
        delete capturedPiece;
        clearThreeFoldRepetitionDictionary();
    } else {
        chessboard[prevX][prevY] = nullptr;
        chessboard[newX][newY] = piece;
    }   

    
    //if a pawn moves forward then the board position is irreverable (clear dicitonary)
    if((*piece).get_pieceType() == "pawn"){
        clearThreeFoldRepetitionDictionary();
    }

    //handling pawn promotion
    Piece* oldPawn = nullptr;
    if((*piece).get_pieceType() == "pawn"){
        int promotionRank = (*piece).get_color() == White ? 7 : 0;
        if(newX == promotionRank){
            oldPawn = chessboard[newX][newY];
            if(oldPawn){
                chessboard[newX][newY] = nullptr;

                ////NOTE: HERE IS WHERE THE INPUT FROM BUTTONS WILL GO
                char characterPiece;
                FENChar promotedPiece;
                std::cout << "Enter promoted piece: ";
                std::cin >> characterPiece;
                promotedPiece = character_to_FENChar(characterPiece);

                if(promotedPiece == BlackKnight || promotedPiece == WhiteKnight)
                    chessboard[newX][newY] = new Knight(_playerColor);
                else if(promotedPiece == BlackBishop || promotedPiece == WhiteBishop)
                    chessboard[newX][newY] = new Bishop(_playerColor);
                else if(promotedPiece == BlackRook || promotedPiece == WhiteRook){
                    chessboard[newX][newY] = new Rook(_playerColor);
                    Piece* newRook = chessboard[newX][newY];
                    (*newRook).set_hasMoved();
                }
                else chessboard[newX][newY] = new Queen(_playerColor);
                piece = chessboard[newX][newY];
                delete oldPawn;
            }
        }
    }

    //updating variables for the next player's move
    _lastMove = {piece, prevX, prevY, newX, newY};
    _playerColor = _playerColor == White ? Black : White;
    isInCheck(_playerColor, true);
    _safeSquares = findSafeSquares();

    if(_playerColor == White) fullNumberOfMoves++;

    _boardAsFEN = get_FENBoard();
    updateThreeFoldRepetitionDictionary(_boardAsFEN);

    _isGameOver = isGameFinished();
}//end move()

//gets user input from user's chess move then determines if that move was the most recent move made
bool chessBoard::isSquareLastMove(int x, int y){ 
    if(!(_lastMove.piece)) return false;
    return ((x == _lastMove.prevX) && (y == _lastMove.prevY)) || ((x == _lastMove.currX) && (y == _lastMove.currY));
}

//to implement run a for loop that iterates through all the squares and lights and LED to 
//show that that square is checked
bool chessBoard::isSquareChecked(int x, int y){
    return _isInCheck && _checkState.x == x && _checkState.y == y;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main game loop.
int main() {
    chessBoard chess;
    std::vector<std::vector<FENChar>> FENChar_chessboard;
    std::vector<SafeSquares> safesquares;
    std::string FENBoard;

    Coords startCoords;
    Coords endCoords;
    Move move;

    //int prevX, prevY, newX, newY;

    FENChar_chessboard = chess.chessBoardView();
    safesquares = chess.get_safeSquares();

    Py_Initialize();
    while (true) {
        // Initialize the Python Interpreter
        

        FENBoard = chess.get_FENBoard();
        //std::cout << FENBoard << "\n";
        FENChar_chessboard = chess.chessBoardView();
        safesquares = chess.get_safeSquares();
        for (int i = chess.getChessBoardSize(); i > 0; i--){
            for (int j = 0; j < chess.getChessBoardSize(); j++){
                std::string str(1, FENChar_chessboard[i-1][j]);
                std::cout << str << " ";
            }
            std::cout << i << "\n";
        }
        /*for (int i = 0; i < chess.getChessBoardSize(); i++){
            std::cout << i+1 << " ";
        }*/
        std::cout << "A B C D E F G H";
        std::cout << "\n";
        if (!chess.get_isGameOver()){
            std::cout << "Enter Move: ";

            //std::cin >> prevX >> prevY >> newX >> newY;

            move = voiceCommandInput();
            startCoords = voiceCommandProcessing(move.startSquare);
            endCoords = voiceCommandProcessing(move.endSquare);
            chess.move(startCoords.y-1, startCoords.x-1, endCoords.y-1, endCoords.x-1);

            std::cout << startCoords.y-1 << ", " <<  startCoords.x-1 << ", " << endCoords.y-1 << ", " << endCoords.x-1;
            std::cout << "\n";
        } else {
            break;
        }
    }
    std::cout << chess.get_gameOverMessage();
    Py_Finalize();
    return 0;
}

Move voiceCommandInput(){
    // Add the current directory to the Python module search path
    PyRun_SimpleString("import sys; sys.path.insert(0, '.')");

    // Import the Python module (without the .py extension)
    PyObject* pName = PyUnicode_DecodeFSDefault("chess_voice");
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    Move moveResult;

    if (pModule != nullptr) {
        // Get the get_user_move() function from the module
        PyObject* pFunc = PyObject_GetAttrString(pModule, "get_user_move");
        if (pFunc && PyCallable_Check(pFunc)) {
            // Call the function with no arguments
            PyObject* pResult = PyObject_CallObject(pFunc, nullptr);
            if (pResult != nullptr) {
                // Ensure the result is a tuple with three elements
                if (PyTuple_Check(pResult) && PyTuple_Size(pResult) == 3) {
                    // Extract the three returned items (all expected as strings)
                    PyObject* pPiece = PyTuple_GetItem(pResult, 0);
                    PyObject* pStart = PyTuple_GetItem(pResult, 1);
                    PyObject* pEnd   = PyTuple_GetItem(pResult, 2);

                    // Convert Python strings to C++ strings using UTF-8 encoding
                    const char* piece_c = PyUnicode_AsUTF8(pPiece);
                    const char* start_c = PyUnicode_AsUTF8(pStart);
                    const char* end_c   = PyUnicode_AsUTF8(pEnd);

                    //std::string piece(piece_c);
                    //std::string startSquare(start_c);
                    //std::string endSquare(end_c);
                    moveResult.piece = piece_c;
                    moveResult.startSquare = start_c;
                    moveResult.endSquare = end_c;

                    // Print the results
                    //std::cout << "Piece: " << piece << std::endl;
                    //std::cout << "From Square: " << startSquare << std::endl;
                    //std::cout << "To Square: " << endSquare << std::endl;
                } else {
                    std::cerr << "Returned value is not a tuple of size 3." << std::endl;
                }
                Py_DECREF(pResult);
            } else {
                PyErr_Print();
                std::cerr << "Call to get_user_move() failed." << std::endl;
            }
            Py_XDECREF(pFunc);
        } else {
            if (PyErr_Occurred())
                PyErr_Print();
            std::cerr << "Cannot find function get_user_move." << std::endl;
        }
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        std::cerr << "Failed to load module 'chess_voice'." << std::endl;
    }
    
    return moveResult;
}

Coords voiceCommandProcessing(std::string square){
    return {(square)[0] - '0', (square)[1] - '0'};
}