#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <thread>


#include <Windows.h>



#include "pisqpipe.h"
#define USE_OPENMP
#include "gomoku.h"


using namespace std;
using std::string;

const char *infotext = "name=\"MCTS\", author=\"Splendor\", version=\"0.1\", country=\"China\", email=\"splendor.kill@gmail.com\"";

GomokuState *pState;
MCTS::ComputeOptions player1_options;

void brain_init()
{
	//player1_options.max_iterations = 10000000;
	player1_options.max_time = 30;
	player1_options.verbose = false;

	if (width < 5 || height < 5) {
		pipeOut("ERROR size of the board");
		return;
	}
	
	brain_restart();
}

void brain_restart()
{
	pState = new GomokuState(9);
	pipeOut("OK");
}


void brain_my(int x, int y)
{
	pState->do_move(std::make_pair(x, y));
}

void brain_opponents(int x, int y)
{
	pState->do_move(make_pair(x, y));
}

void brain_block(int x, int y)
{
}

int brain_takeback(int x, int y)
{
	return 2;
}

void brain_turn()
{
	GomokuState::Move move = MCTS::compute_move(*pState, player1_options);
	do_mymove(move.first, move.second);
}

void brain_end()
{
	delete pState;
}

