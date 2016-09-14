#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <thread>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <Windows.h>

#include "pisqpipe.h"
#include "forwarder.h"

using namespace std;
using std::string;

namespace fs = boost::filesystem;
namespace asio = boost::asio;
using asio::ip::tcp;

const char *infotext = "name=\"Stub\", author=\"Splendor\", version=\"0.1\", country=\"China\", email=\"splendor.kill@gmail.com\"";

#define MAX_BOARD 100

map<string, string> options;
void read_config(map<string, string>&, const string&);

Forwarder *the_client;

void brain_init()
{
	try {
		if (options.empty()) {
			TCHAR buf[MAX_PATH];
			GetModuleFileName(NULL, buf, MAX_PATH);
			fs::path full_path = fs::system_complete(fs::path(buf));
			auto pp = full_path.parent_path();
			auto file = fs::path("config.txt");
			auto cfg = pp / file;
			if (!fs::exists(cfg))
				cfg = pp / fs::path("..") / file;
			read_config(options, cfg.string());
		}
	}
	catch (std::exception&) {
		pipeOut("ERROR config file problems.");
		return;
	}

	try {
		the_client = new Forwarder(options["server_ip"], options["server_port"]);
		the_client->do_connect();
	}
	catch (std::exception&) {
		pipeOut("ERROR check network things.");
		return;
	}

	if (width < 5 || height < 5) {
		pipeOut("ERROR size of the board");
		return;
	}
	if (width > MAX_BOARD || height > MAX_BOARD) {
		pipeOut("ERROR Maximal board size is %d", MAX_BOARD);
		return;
	}

	brain_restart();
}

void brain_restart()
{
	string msg = "START: " + to_string(width);
	try {
		the_client->send_a_msg(msg);
		the_client->wait_answer();
		if (!the_client->start_ok) {
			pipeOut("ERROR start failed.");
			return;
		}
	}
	catch (std::exception&) {
		pipeOut("ERROR start failed.");
		return;
	}
	pipeOut("OK");
}


void brain_my(int x, int y)
{
	string msg = "MOVE: " + to_string(x) + " " + to_string(y) + " 1";

	try {
		the_client->send_a_msg(msg);
	}
	catch (std::exception&) {
		pipeOut("ERROR my move [%d,%d]", x, y);
	}
}

void brain_opponents(int x, int y)
{
	string msg = "MOVE: " + to_string(x) + " " + to_string(y) + " 2";
	try {
		the_client->send_a_msg(msg);
	}
	catch (std::exception&) {
		pipeOut("ERROR opponents's move [%d,%d]", x, y);
	}
}

void brain_block(int x, int y)
{
	string msg = "WIN: " + to_string(x) + " " + to_string(y);
	try {
		the_client->send_a_msg(msg);
	}
	catch (std::exception&) {
		pipeOut("ERROR winning move [%d,%d]", x, y);
	}
}

int brain_takeback(int x, int y)
{
	string msg = "UNDO: " + to_string(x) + " " + to_string(y);
	try {
		the_client->send_a_msg(msg);
		the_client->wait_answer();
		if (!the_client->undo_ok)
			return 0;
	}
	catch (std::exception&) {
		return 0;
	}
	return 2;
}

void brain_turn()
{
	string msg("WHERE: 1");
	try {
		the_client->send_a_msg(msg);
		the_client->wait_answer();
	}
	catch (std::exception&) {
		pipeOut("ERROR i have no idea");
	}
	do_mymove(the_client->x, the_client->y);
}

void brain_end()
{
	string msg = "END: " + to_string(the_client->token_);
	the_client->send_a_msg(msg);
}

void read_config(map<string, string>& options, const string& file_name)
{
	ifstream myfile(file_name);
	if (myfile.is_open()) {
		string line;
		while (getline(myfile, line)) {
			std::regex base_regex("(\\S+)\\s*=\\s*(.*)", std::regex_constants::ECMAScript);
			std::smatch base_match;
			if (std::regex_match(line, base_match, base_regex)) {
				string key = base_match[1].str();
				string value = base_match[2].str();
				boost::trim_right(value);
				options[key] = value;
			}
		}
		myfile.close();
	}
	else {
		throw std::runtime_error("what's matter with the config files?");
	}
}
