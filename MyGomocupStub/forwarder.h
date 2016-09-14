#ifndef _FORWARDER_H_
#define _FORWARDER_H_

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include "packet.h"

namespace asio = boost::asio;

using asio::ip::tcp;
using std::string;
using std::vector;

typedef std::deque<Packet> message_queue;

class Forwarder
{
public:
	Forwarder(const std::string& host, const std::string& port)
		: host_(host), port_(port), socket_(io_service_)
	{
	}

	void close()
	{
		io_service_.post([this]() { socket_.close(); });
	}

	void do_connect()
	{
		tcp::resolver resolver(io_service_);
		tcp::resolver::query query(host_, port_);
		auto endpoint_iterator = resolver.resolve(query);

		asio::connect(this->socket_, endpoint_iterator);

		wait_answer();
	}

	void send_a_msg(const string& q)
	{
		Packet msg;
		msg.body_length(q.length());
		std::memcpy(msg.body(), q.data(), msg.body_length());
		msg.encode_header();
		asio::write(socket_, asio::buffer(msg.data(), msg.length()));
	}

	void wait_answer()
	{
		read_a_msg();
		dispose_msg();
	}

private:
	void read_a_msg()
	{
		asio::read(socket_, asio::buffer(read_msg_.data(), Packet::header_length));
		read_msg_.decode_header();
		asio::read(socket_, asio::buffer(read_msg_.body(), read_msg_.body_length()));
	}

	void dispose_msg()
	{
		string msg(read_msg_.body(), read_msg_.body_length());

		size_t p = msg.find(" ");
		if (p != msg.npos) {
			string cmd = msg.substr(0, p);
			string args = msg.substr(p);
			if (boost::starts_with(cmd, "TOKEN:")) {
				this->token_ = std::stoi(args);
			}
			else if (boost::starts_with(cmd, "HERE:")) {
				boost::trim(args);
				size_t sep = args.find(" ");
				this->x = std::stoi(args.substr(0, sep));
				this->y = std::stoi(args.substr(sep + 1));
			}
			else if (boost::starts_with(cmd, "UNDO:")) {
				boost::trim(args);
				this->undo_ok = boost::iequals(args, "OK");
			}
			else if (boost::starts_with(cmd, "START:")) {
				boost::trim(args);
				this->start_ok = boost::iequals(args, "OK");
			}
		}
	}

private:
	string host_;
	string port_;

	asio::io_service io_service_;
	tcp::socket socket_;

	Packet read_msg_;

public:
	int token_;
	int x, y;
	bool start_ok;
	bool undo_ok;
};

#endif // _FORWARDER_H_