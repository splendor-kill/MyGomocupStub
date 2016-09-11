#include "pisqpipe.h"

const char *infotext = "name=\"Stub\", author=\"Splendor\", version=\"0.1\", country=\"China\", email=\"splendor.kill@gmail.com\"";

#define MAX_BOARD 100
int board[MAX_BOARD][MAX_BOARD];
static unsigned seed;

void brain_init()
{
	if (width < 5 || height < 5) {
		pipeOut("ERROR size of the board");
		return;
	}
	if (width > MAX_BOARD || height > MAX_BOARD) {
		pipeOut("ERROR Maximal board size is %d", MAX_BOARD);
		return;
	}

	pipeOut("OK");
}

void brain_restart()
{
	//int x, y;
	//for (x = 0; x < width; x++) {
	//	for (y = 0; y < height; y++) {
	//		board[x][y] = 0;
	//	}
	//}
	pipeOut("OK");
}

int isFree(int x, int y)
{
	return x >= 0 && y >= 0 && x < width && y < height && board[x][y] == 0;
}

void brain_my(int x, int y)
{
	if (isFree(x, y)) {
		board[x][y] = 1;
	}
	else {
		pipeOut("ERROR my move [%d,%d]", x, y);
	}
}

void brain_opponents(int x, int y)
{
	if (isFree(x, y)) {
		board[x][y] = 2;
	}
	else {
		pipeOut("ERROR opponents's move [%d,%d]", x, y);
	}
}

void brain_block(int x, int y)
{
	if (isFree(x, y)) {
		board[x][y] = 3;
	}
	else {
		pipeOut("ERROR winning move [%d,%d]", x, y);
	}
}

int brain_takeback(int x, int y)
{
	if (x >= 0 && y >= 0 && x < width && y < height && board[x][y] != 0) {
		board[x][y] = 0;
		return 0;
	}
	return 2;
}

void brain_turn()
{
	int x, y, i;

	i = -1;
	do {
		x = 0;
		y = 0;
		i++;
		if (terminateAI) return;
	} while (!isFree(x, y));

	if (i > 1) pipeOut("DEBUG %d coordinates didn't hit an empty field", i);
	do_mymove(x, y);
}

void brain_end()
{
}

#ifdef DEBUG_EVAL

void brain_eval(int x, int y)
{
	HDC dc;
	HWND wnd;
	RECT rc;
	char c;
	wnd = GetForegroundWindow();
	dc = GetDC(wnd);
	GetClientRect(wnd, &rc);
	c = (char)(board[x][y] + '0');
	TextOut(dc, rc.right - 15, 3, &c, 1);
	ReleaseDC(wnd, dc);
}

#endif
