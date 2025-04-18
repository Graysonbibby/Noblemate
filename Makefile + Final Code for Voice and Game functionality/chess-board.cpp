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
        _moveTypeFlag = "4";
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
            _moveTypeFlag = "3";
        }
    }
}//end handlingSpecialMoves()

//determines if enPassant capture can occur
bool chessBoard::canCaptureEnPassant(Piece* pawn, int pawnX, int pawnY){
    if(!pawn) return false;
    if((*pawn).get_pieceType() != "pawn") return false;
    if(!(_lastMove.piece)) return false;

    Piece* piece = _lastMove.piece;

    int prevX, prevY, currX, currY;
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
            _checkmateFlag = "2";
        } else {
            _gameOverMessage = "Stalemate";
            _checkmateFlag = "3";
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
                                             !isSquareDark((*whiteBishop).x, (*whiteBishop).y) && !isSquareDark((*blackBishop).x, (*blackBishop).y);`   
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

std::vector<SafeSquares> chessBoard::get_safeSquares(){
    return _safeSquares;
}

void chessBoard::reset_board(){
    for(int i = 0; i < chessBoardSize; i++){
        for(int j = 0; j < chessBoardSize; j++){
            if(!chessboard[i][j]) continue;
            else {
                Piece* piece = chessboard[i][j];
                chessboard[i][j] = nullptr;
                delete piece;
            }
        }
    }
    
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
    _playerColor = White;
    _safeSquares = findSafeSquares();
    _isInCheck = false;
    _pawnPromoted = false;
    _isGameOver = false;
    fiftyMoveRuleCounter = 0;
    fullNumberOfMoves = 1;
    clearThreeFoldRepetitionDictionary();
    threeFoldRepetitionFlag = false;
     
    _boardAsFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
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

//getters and setters for user and engine colours
void chessBoard::set_Colors(std::string color){
    if(color == "White"){
        userColor = White;
    } else {
        userColor = Black;
    }
    
    engineColor = userColor == White ? Black : White;
}

Color chessBoard::get_UserColor(){
    return userColor;
}

Color chessBoard::get_EngineColor(){
    return engineColor;
}

void chessBoard::set_enginePromotedPiece(char promotedPiece){
    enginePromotedPiece = promotedPiece;
}

void chessBoard::set_moveTypeFlag(std::string moveType){
    _moveTypeFlag = moveType;
}

void chessBoard::set_moveCoords(std::string moveCoords){
    _moveCoords = moveCoords;
}

void chessBoard::set_checkmateFlag(std::string checkmateFlag){
    _checkmateFlag = checkmateFlag;
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
                        if ( (*piece).get_pieceType() == "pawn" && ((newY - j) == 0)) continue;//POSSIBLE ERROR HERE

                        //verifies that the current player moving is in check
                        Piece* attacked_piece = chessboard[newX][newY];
                        if (attacked_piece == nullptr) continue;

                        if((*attacked_piece).get_pieceType() == "king" && (*attacked_piece).get_color() == playerColor){
                            if(checkingCurrentPosition) {
                                _isInCheck = true;
                                _checkState = KingChecked(newX, newY);
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
                                _checkState = KingChecked(newX, newY);
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
    _moveTypeFlag = "0";
    _promotionFlag = "0";
    _checkmateFlag = "0";
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
        _moveTypeFlag = "1";
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

                char characterPiece;
                std::string characterString;
                FENChar promotedPiece;
                if(userColor == _playerColor){
                    std::cout << "Enter promoted piece: ";
                    characterString = voiceInput("get_promotion_piece");
                    if(characterString == "Knight") {
                        characterPiece = 'n';
                        _promotionFlag = "2";
                    } else {
                        characterPiece = 'q';
                        _promotionFlag = "1";
                    }
                } else {
                    characterPiece = enginePromotedPiece;
                }
                promotedPiece = character_to_FENChar(characterPiece);
                
                //only promotion options are queen or knight
                if(promotedPiece == BlackKnight || promotedPiece == WhiteKnight)
                    chessboard[newX][newY] = new Knight(_playerColor);
                else chessboard[newX][newY] = new Queen(_playerColor);
                piece = chessboard[newX][newY];
                delete oldPawn;
                _checkmateFlag = "1";
            }
        }
    }

    //updating variables for the next player's move
    _lastMove = LastMove(piece, prevX, prevY, newX, newY);
    _playerColor = _playerColor == White ? Black : White;
    isInCheck(_playerColor, true);
    _safeSquares = findSafeSquares();

    if(_playerColor == White) fullNumberOfMoves++;

    _boardAsFEN = get_FENBoard();
    updateThreeFoldRepetitionDictionary(_boardAsFEN);

    _isGameOver = isGameFinished();
    
    if(_isGameOver) _checkmateFlag = "2";
    
    //sending data to arduino
    std::string serialData = _moveCoords + _moveTypeFlag + _checkmateFlag + _promotionFlag + "\n";
    std::cout << "Serial Data Input: " << serialData << std::endl;
    
    const char* serialDataStr = serialData.c_str();
    int serialPort;
    std::string moveCompleted = "nc";
    
    serialPort = initializeComm();
    if(serialPort != -1){
        sendData(serialPort, serialDataStr);
    }
}//end move()

//gets user input from user's chess move then determines if that move was the most recent move made
bool chessBoard::isSquareLastMove(int x, int y){ 
    if(!(_lastMove.piece)) return false;
    return ((x == _lastMove.prevX) && (y == _lastMove.prevY)) || ((x == _lastMove.currX) && (y == _lastMove.currY));
}

bool chessBoard::isSquareChecked(int x, int y){
    return _isInCheck && _checkState.x == x && _checkState.y == y;
}
