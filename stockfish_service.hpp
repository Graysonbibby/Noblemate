#ifndef STOCKFISH_SERVICE_H
#define STOCKFISH_SERVICE_H

#include "chess-board.hpp"
#include <boost/process.hpp>

namespace bp = boost::process;


std::string send_command(bp::opstream &engineInput, bp::ipstream &engineOutput,
	const std::string &cmd, const std::string &waitFor);
	
void setoption_command(bp::opstream &engineInput, bp::ipstream &engineOutput,
	const std::string &cmd, const std::string &value);
	
void setposition_command(bp::opstream &engineInput, const std::string fen);

int letter_converter(char letter);

std::vector<Coords> convert_stockfishMove(std::string bestMove, char* promotedPiece);

std::vector<Coords> get_bestMove(std::string fen, std::string elo_rating,
	std::string search_depth, char* promotedPiece);
#endif
