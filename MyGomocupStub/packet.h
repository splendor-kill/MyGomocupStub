#ifndef _PACKET_H_
#define _PACKET_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <boost/asio.hpp>

class Packet
{
public:
	enum { header_length = 4 };
	enum { max_body_length = 512 };

	Packet()
		: body_length_(0)
	{
	}

	const char* data() const
	{
		return data_;
	}

	char* data()
	{
		return data_;
	}

	size_t length() const
	{
		return header_length + body_length_;
	}

	const char* body() const
	{
		return data_ + header_length;
	}

	char* body()
	{
		return data_ + header_length;
	}

	size_t body_length() const
	{
		return body_length_;
	}

	void body_length(size_t new_length)
	{
		body_length_ = new_length;
		if (body_length_ > max_body_length)
			body_length_ = max_body_length;
	}

	bool decode_header()
	{
		using boost::asio::detail::socket_ops::network_to_host_long;
		boost::uint32_t length;
		std::memcpy(&length, data_, sizeof length);
		body_length_ = network_to_host_long(length);

		if (body_length_ > max_body_length)
		{
			body_length_ = 0;
			return false;
		}
		return true;
	}

	void encode_header()
	{
		using boost::asio::detail::socket_ops::host_to_network_long;
		boost::uint32_t length = body_length_;
		length = host_to_network_long(length);
		std::memcpy(data_, &length, sizeof length);
	}

private:
	char data_[header_length + max_body_length];
	size_t body_length_;
};

#endif // _PACKET_H_
