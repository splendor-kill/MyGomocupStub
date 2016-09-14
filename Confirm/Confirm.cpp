// Confirm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::asio::ip::tcp;


using namespace std;

void print(const boost::system::error_code& /*e*/)
{
	std::cout << "Hello, world!" << std::endl;
}

void print1(const boost::system::error_code& /*e*/,
	boost::asio::deadline_timer* t, int* count)
{
	if (*count < 5)
	{
		std::cout << *count << std::endl;
		++(*count);

		t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
		t->async_wait(boost::bind(print1,
			boost::asio::placeholders::error, t, count));
	}
}

class printer
{
public:
	printer(boost::asio::io_service& io)
		: timer_(io, boost::posix_time::seconds(1)),
		count_(0)
	{
		timer_.async_wait(boost::bind(&printer::print, this));
	}

	~printer()
	{
		std::cout << "Final count is " << count_ << std::endl;
	}

	void print()
	{
		if (count_ < 5)
		{
			std::cout << count_ << std::endl;
			++count_;

			timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
			timer_.async_wait(boost::bind(&printer::print, this));
		}
	}

private:
	boost::asio::deadline_timer timer_;
	int count_;
};

class printer2
{
public:
	printer2(boost::asio::io_service& io)
		: strand_(io),
		timer1_(io, boost::posix_time::seconds(1)),
		timer2_(io, boost::posix_time::seconds(1)),
		count_(0)
	{
		timer1_.async_wait(strand_.wrap(boost::bind(&printer2::print1, this)));
		timer2_.async_wait(strand_.wrap(boost::bind(&printer2::print2, this)));
	}

	~printer2()
	{
		std::cout << "Final count is " << count_ << std::endl;
	}

	void print1()
	{
		if (count_ < 10)
		{
			std::cout << "tid: " << boost::this_thread::get_id() << ", " << "Timer 1: " << count_ << std::endl;
			++count_;

			timer1_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));
			timer1_.async_wait(strand_.wrap(boost::bind(&printer2::print1, this)));
		}
	}

	void print2()
	{
		if (count_ < 10)
		{
			
			std::cout << "tid: " << boost::this_thread::get_id() << ", " << "Timer 2: " << count_ << std::endl;
			++count_;

			timer2_.expires_at(timer2_.expires_at() + boost::posix_time::seconds(1));
			timer2_.async_wait(strand_.wrap(boost::bind(&printer2::print2, this)));
		}
	}

private:
	boost::asio::io_service::strand strand_;
	boost::asio::deadline_timer timer1_;
	boost::asio::deadline_timer timer2_;
	int count_;
};

int main(int argc, char* argv[])
{
	//boost::asio::io_service io;

	//try
	//{
	//	if (argc != 2)
	//	{
	//		std::cerr << "Usage: client <host>" << std::endl;
	//		return 1;
	//	}

	//	tcp::resolver resolver(io);
	//	tcp::resolver::query query(argv[1], "daytime");
	//	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

	//	tcp::socket socket(io);
	//	boost::asio::connect(socket, endpoint_iterator);

	//	for (;;)
	//	{
	//		boost::array<char, 128> buf;
	//		boost::system::error_code error;

	//		size_t len = socket.read_some(boost::asio::buffer(buf), error);

	//		if (error == boost::asio::error::eof)
	//			break; // Connection closed cleanly by peer.
	//		else if (error)
	//			throw boost::system::system_error(error); // Some other error.

	//		std::cout.write(buf.data(), len);
	//	}
	//}
	//catch (std::exception& e)
	//{
	//	std::cerr << e.what() << std::endl;
	//}


	//{
	//printer2 p(io);
	//boost::thread t(boost::bind(&boost::asio::io_service::run, &io));
	//io.run();
	//t.join();
	//}

	//printer p(io);
	//io.run();

	//int count = 0;
	//boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
	//t.async_wait(boost::bind(print1,
	//	boost::asio::placeholders::error, &t, &count));
	//io.run();
	//std::cout << "Final count is " << count << std::endl;

	//boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
	//t.async_wait(&print);
	//io.run();

	return 0;
}