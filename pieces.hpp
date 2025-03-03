#ifndef PIECES_HPP
#define PIECES_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>
#include <sstream>
#include <cmath>

struct Coords{
    int x;
    int y;
};

struct SafeSquares{
    Coords square;
    std::vector<Coords> SafeSquare_coords;
};


enum Color{
    Black,
    White
};

enum FENChar{
    WhiteRook = 'R',
    WhiteKnight = 'N',
    WhiteBishop = 'B',
    WhiteQueen = 'Q',
    WhiteKing = 'K',
    WhitePawn = 'P',
    BlackRook = 'r',
    BlackKnight = 'n',
    BlackBishop = 'b',
    BlackQueen = 'q',
    BlackKing = 'k',
    BlackPawn = 'p',
    None = 'x'
};



class Piece {
    protected:
        Color _color;
        FENChar _FENChar;
        std::vector<Coords> _directions;
        bool _hasMoved;

    public:
        Piece(Color _color):
            _color(_color)
        {
        }

        FENChar get_FENChar(){
            return _FENChar;
        }

        std::vector<Coords> get_directions(){
            return _directions;
        }

        Color get_color(){
            return _color;
        }

        virtual bool get_hasMoved(){
            return _hasMoved;
        }

        virtual void set_hasMoved(){
            _hasMoved = true;
        }

        virtual std::string get_pieceType(){
            return "piece";
        }
};

class Bishop : public Piece {
    public:
        Bishop(Color pieceColor):
            Piece(pieceColor)
        {
            _directions = {
                {1, 1},
                {1, -1},
                {-1, 1},
                {-1, -1}
            };
            _FENChar  = (pieceColor == White) ? WhiteBishop : BlackBishop;
        }

        std::string get_pieceType() override {
            return "bishop";
        }
};

class Knight : public Piece {
    public:
        Knight(Color pieceColor):
            Piece(pieceColor)
        {
            _directions = {
                {1, 2},
                {1, -2},
                {-1, 2},
                {-1, -2},
                {2, 1},
                {2, -1},
                {-2, 1},
                {-2, -1}
            };
            _FENChar  = (pieceColor == White) ? WhiteKnight : BlackKnight;
        }

        std::string get_pieceType() override {
            return "knight";
        }
};

class Queen : public Piece {
    public:
        Queen(Color piececolor):
            Piece(piececolor)
        {
            _directions = {
                {0, 1},
                {0, -1},
                {1, 0},
                {1, -1},
                {1, 1},
                {-1, 0},
                {-1, 1},
                {-1, -1}
            };
            _FENChar = (piececolor == White) ? WhiteQueen : BlackQueen;
        }

        std::string get_pieceType() override {
            return "queen";
        }
};

class Rook : public Piece {
    private:
        bool _hasMoved;

    public:
        Rook(Color piececolor):
            Piece(piececolor)
        {   
            _directions = {
                {0, 1},
                {0, -1},
                {1, 0},
                {-1, 0}
            };
            _FENChar = (piececolor == White) ? WhiteRook : BlackRook;
        }

        bool get_hasMoved() override {
            return _hasMoved;
        }

        void set_hasMoved() override {
            _hasMoved = true;
        }

        std::string get_pieceType() override {
            return "rook";
        }
};

class King : public Piece {
    private: 
        bool _hasMoved = false;

    public:
        King(Color piececolor):
            Piece(piececolor)
        {
            _directions = {
                {0, 1},
                {0, -1},
                {1, 0},
                {1, -1},
                {1, 1},
                {-1, 0},
                {-1, 1},
                {-1, -1}
            };
            _FENChar = (piececolor == White) ? WhiteKing : BlackKing;
        }

        bool get_hasMoved() override {
            return _hasMoved;
        }

        void set_hasMoved() override {
            _hasMoved = true;
        }

        std::string get_pieceType() override {
            return "king";
        }
};

class Pawn : public Piece {
    private:
        bool _hasMoved = false;
    public:
        Pawn(Color piececolor):
            Piece(piececolor)
        {   
            _directions = {
                {1, 0},
                {2, 0},
                {1, 1},
                {1, -1}
            };
            if(piececolor == Black){
                for(int i = 0; i < 4; i++){
                    _directions[i].x = -1*_directions[i].x;
                }
            }
            _FENChar = (piececolor == White) ? WhitePawn : BlackPawn;

        }

        bool get_hasMoved() override {
            return _hasMoved;
        }

        void set_hasMoved() override {
            _hasMoved = true;
            _directions = {
                {1, 0},
                {1, 1},
                {1, -1}
            };
            if(_color == Black){
                for(int i = 0; i < 3; i++){
                    _directions[i].x = -1*_directions[i].x;
                }
            }
        }

        std::string get_pieceType() override {
            return "pawn";
        }
};

#endif
