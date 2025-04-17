#include "stockfish_service.hpp"
#include "serial_communication.hpp"
#include "chess-board.hpp"
#include "voiceCommand.hpp"
#include "Python.h"
    
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

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
    
    FENChar_chessboard = chess.chessBoardView();
    safesquares = chess.get_safeSquares();
    
    FILE* orig_stderr = stderr;
    freopen("/dev/null", "w", stderr);
    
    std::string gameDifficulty = "";
    std::string ELO = "";
    std::string search_depth = "";
    std::string command = "";
    
    bool rematch = false;
    std::string playerColor = "";
    std::string color;
    
    int run = initializeComm();
    
    Py_Initialize();
    
    bool replayGame = false;
    
    while(true){
        
        if(!rematch){
            gameDifficulty = voiceInput("get_game_mode");
            std::cout << "selected difficulty: " << gameDifficulty << std::endl;
            if(gameDifficulty == "Easy"){
                ELO = "1200";
                search_depth = "10";
            } else if (gameDifficulty == "Medium"){
                ELO = "1800";
                search_depth = "11";
            } else if (gameDifficulty == "Difficult"){
                ELO = "2400";
                search_depth = "12";
            } else if (gameDifficulty == "Insane"){
                ELO = "3200";
                search_depth = "13";
            } else {
                ELO = "1200";
                search_depth = "10";
            }
            
            //end of ELO setting functionality
            std::cout << "ELO: " << ELO << std::endl;
         
            std::cout << "Enter which colour you wish to play as (white or black): \n";
            color = voiceInput("get_player_color");
            chess.set_Colors(color);
            
            if(chess.get_UserColor() == White){
                playerColor = "white";
            } else {
                playerColor = "black";    
            }
            //end of getting player colour functionality
        }
        
        std::cout << "Player playing as: " << playerColor << "\n";
        std::cout << "game difficulty : " << ELO << std::endl;
        
        if(replayGame){
            int i = 30;
            //30 second countdown timer
            while(i >= 0){
                std::cout << "Countdown to replay: ";
                std::cout << i << " ";
                sleep(1);
                i--;
            }
            std::cout << "\n";
        }

        chess.reset_board();
        
        while (true) {
            // Initialize the Python Interpreter
            FENBoard = chess.get_FENBoard();
            FENChar_chessboard = chess.chessBoardView();
            safesquares = chess.get_safeSquares();
        
            //printing the chessboard
            for (int i = chess.getChessBoardSize(); i > 0; i--){
                for (int j = 0; j < chess.getChessBoardSize(); j++){
                    std::string str(1, FENChar_chessboard[i-1][j]);
                    std::cout << str << " ";
                }
                std::cout << i << "\n";
            }
        
            std::cout << "A B C D E F G H";
            std::cout << "\n";
        
            //getting the move from either the user or stockfish
            if (!chess.get_isGameOver() && (chess.get_playerColor() == chess.get_UserColor())){
                /*command = voiceInput("get_command");
                if(command == "View"){
                    sleep(15);
                }*/
                std::cout << "Enter Move: ";
                
                move = moveInput();
                startCoords = voiceCommandProcessing(move.startSquare);
                endCoords = voiceCommandProcessing(move.endSquare);

                std::string dig1 = std::to_string(startCoords.y);
                std::string dig2 = std::to_string(startCoords.x);
                std::string dig3 = std::to_string(endCoords.y);
                std::string dig4 = std::to_string(endCoords.x);
                
                chess.set_moveCoords("");
                chess.set_moveCoords(dig2 + dig1 + dig4 + dig3);
                chess.move(startCoords.y-1, startCoords.x-1, endCoords.y-1, endCoords.x-1);
                
                std::cout << startCoords.y-1 << ", " <<  startCoords.x-1 << ", " << endCoords.y-1 << ", " << endCoords.x-1;
                std::cout << "\n";
            } 
            else if (!chess.get_isGameOver() && (chess.get_playerColor() == chess.get_EngineColor())) {
                std::vector<Coords> engineMove;
                std::string fenboard = chess.get_FENBoard();
                char promotedPiece = 'q';
            
                engineMove = get_bestMove(fenboard, ELO, search_depth, &promotedPiece);
                
                std::string dig1 = std::to_string(engineMove[0].x+1);
                std::string dig2 = std::to_string(engineMove[0].y+1);
                std::string dig3 = std::to_string(engineMove[1].x+1);
                std::string dig4 = std::to_string(engineMove[1].y+1);
                
                chess.set_moveCoords("");
                chess.set_moveCoords(dig1 + dig2 + dig3 + dig4);
                
                chess.set_enginePromotedPiece(promotedPiece);
                chess.move(engineMove[0].y, engineMove[0].x, engineMove[1].y, engineMove[1].x);
                
            }
            else {
                break;
            }
        }
        std::cout << chess.get_gameOverMessage() << "\n";
        
        std::string rematchOption = voiceInput("get_reset_condition");
        
        if(rematchOption == "Rematch"){
            rematch = true;
            if(playerColor == "white") {
                chess.set_Colors("Black");
                playerColor = "black";
            }
            else {
                chess.set_Colors("White");
                playerColor = "white";
            }
            std::cout << "Game rematch initiated!\n";
            
        } else {
            rematch = false;
            std::cout << "Returning to game menu!\n";
        }
        
        replayGame = true;
        
    }//end outer while loop
    
    Py_Finalize();
    
    return 0;
}
