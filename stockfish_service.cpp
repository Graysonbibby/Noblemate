 #include "stockfish_service.hpp"

std::string send_command(bp::opstream &engineInput, bp::ipstream &engineOutput,
	const std::string &cmd, const std::string &waitFor){
	
	engineInput << cmd << std::endl;
	std::string line;
	std::ostringstream response;
	
	while(std::getline(engineOutput, line)){
		response << line << "\n";
		if(line.find(waitFor) != std::string::npos) break;
	}
	
	return response.str();
}

void setoption_command(bp::opstream &engineInput, bp::ipstream &engineOutput,
	const std::string &cmd, const std::string &value){
			
	std::string command = "setoption name " + cmd + " value " + value;
	engineInput << command << std::endl;
	
	engineInput << "isready" << std::endl;
	std::string line;
	
	while(std::getline(engineOutput, line)){
		if(line.find("readyok") != std::string::npos) break;
	}
	
}

void setposition_command(bp::opstream &engineInput, const std::string fen){
	std::string command = "position fen " + fen;
	engineInput << command << std::endl;
}

int letter_converter(char letter){	
	if(letter > 57){
		if(letter == 'a') return 0;
		else if(letter == 'b') return 1;
		else if(letter == 'c') return 2;
		else if(letter == 'd') return 3;
		else if(letter == 'e') return 4;
		else if(letter == 'f') return 5;
		else if(letter == 'g') return 6;
		else if(letter == 'h') return 7;
	} else {
		return letter - '1';
	}
}

std::vector<Coords> convert_stockfishMove(std::string bestMove, char* promotedPiece){
	std::vector<Coords> bestMove_coords;
	int bestMoveLength = 0;
	for(int i = 0; bestMove[i] != '\0'; i++){
		bestMoveLength++;
	}
	
	if(bestMoveLength == 5){
		int startSquareX = letter_converter(bestMove[0]);
		int startSquareY = letter_converter(bestMove[1]);
		int endSquareX = letter_converter(bestMove[2]);
		int endSquareY = letter_converter(bestMove[3]);
		
		bestMove_coords.push_back({startSquareX, startSquareY});
		bestMove_coords.push_back({endSquareX, endSquareY});
		
		*promotedPiece = bestMove[4];
	} else if (bestMoveLength == 4){
		int startSquareX = letter_converter(bestMove[0]);
		int startSquareY = letter_converter(bestMove[1]);
		int endSquareX = letter_converter(bestMove[2]);
		int endSquareY = letter_converter(bestMove[3]);
		
		bestMove_coords.push_back({startSquareX, startSquareY});
		bestMove_coords.push_back({endSquareX, endSquareY});
	} else {
		return {{0, 0}, {0, 0}};
	}
	
	return bestMove_coords;
}

std::vector<Coords> get_bestMove(const std::string fen, std::string elo_rating,
	std::string search_depth, char* promotedPiece){
	
	std::vector<Coords> stockfish_move;
	bp::ipstream engineOutput;
	bp::opstream engineInput;
	
	bp::child stockfish("Stockfish/src/stockfish", bp::std_out > engineOutput, bp::std_in < engineInput);
	
	std::string uciResponse = send_command(engineInput, engineOutput, "uci", "uciok");
	std::cout << "UCI Ready?: " << uciResponse;
	std::string readyResponse = send_command(engineInput, engineOutput, "isready", "readyok");
	std::cout << "Engine ready?: " << readyResponse;
	
	setoption_command(engineInput, engineOutput, "UCI_Elo", elo_rating);
	setoption_command(engineInput, engineOutput, "UCI_LimitStrength", "true");
	setoption_command(engineInput, engineOutput, "Threads", "4");
	
	setposition_command(engineInput, fen);
	
	engineInput << "go depth " << search_depth << std::endl;
	
	std::string line, bestMove;
	while(std::getline(engineOutput, line)){
		std::cout << line << "\n";
		if(line.find("bestmove") != std::string::npos){
			std::istringstream iss(line);
			std::string token;
			iss >> token;
			iss >> bestMove;
			break;
		}
	}
	
	stockfish_move = convert_stockfishMove(bestMove, promotedPiece);
	
	return stockfish_move;
}

