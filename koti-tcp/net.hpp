#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <boost/asio.hpp>

namespace koti {

namespace asio = boost::asio;
namespace ip = asio::ip;
namespace local = boost::asio::local;

using port_number = std::uint16_t;

class tcp6;
class tcp4 : public ip::tcp
{
public:

	class endpoint : public asio::ip::basic_endpoint<tcp4>
	{
	public:
		using basic_endpoint::basic_endpoint;
		inline tcp4 protocol() const { return {}; }
	};

	//using endpoint = asio::ip::basic_endpoint<tcp4>;
	using socket = asio::basic_stream_socket<tcp4>;
	using acceptor = asio::basic_socket_acceptor<tcp4>;
	using resolver = asio::ip::basic_resolver<tcp4>;
	using iostream = asio::basic_socket_iostream<tcp4>;

	tcp4();

	static
	tcp4
	v4();

	static
	tcp6
	v6();

	using tcp::tcp;

	static
	endpoint
	local_endpoint();

	static
	bool
	is_bound(
		const socket & s
	);

	static
	bool
	is_bound(
		const acceptor & s
	);
};

class tcp6 : public ip::tcp
{
public:

	class endpoint : public asio::ip::basic_endpoint<tcp6>
	{
	public:
		using basic_endpoint::basic_endpoint;
		inline tcp6 protocol() const { return {}; }
	};

	//using endpoint = asio::ip::basic_endpoint<tcp6>;
	using socket = asio::basic_stream_socket<tcp6>;
	using acceptor = asio::basic_socket_acceptor<tcp6>;
	using resolver = asio::ip::basic_resolver<tcp6>;
	using iostream = asio::basic_socket_iostream<tcp6>;

	tcp6();

	static
	tcp4
	v4();

	static
	tcp6
	v6();

	using tcp::tcp;

	static
	endpoint
	local_endpoint();

	static
	bool
	is_bound(
		const socket & s
	);

	static
	bool
	is_bound(
		const acceptor & s
	);
};

using tcp = tcp6;

class local_stream : public local::stream_protocol
{
public:
	class endpoint
	: public asio::local::basic_endpoint<local_stream>
	{
	public:
		using asio::local::basic_endpoint<local_stream>::basic_endpoint;
		bool is_abstract() const;
	};

	using socket = asio::basic_stream_socket<local_stream>;
	using acceptor = asio::basic_socket_acceptor<local_stream>;
//	using resolver = asio::ip::basic_resolver<local_stream>;
	using iostream = asio::basic_socket_iostream<local_stream>;

	static
	endpoint
	local_endpoint();

	static
	bool
	is_bound(
		const socket & s
	);

	static
	bool
	is_bound(
		const acceptor & s
	);
};

// https://stackoverflow.com/a/32172486/1111557
class inheritable_shared_from_this
	: virtual public std::enable_shared_from_this<inheritable_shared_from_this>
{
protected:
	virtual ~inheritable_shared_from_this() = default;

	template <typename Derived>
	std::shared_ptr<Derived> shared_from()
	{
		return std::static_pointer_cast<Derived>(shared_from_this());
	}
};

template <typename T, typename ... Args>
typename T::pointer
protected_make_shared_enabler(Args && ... args)
{
	struct magic : public T
	{
		magic(Args && ... args)
			: T(std::forward<Args>(args)...)
		{
		}
	};

	return std::make_shared<
		magic
	>(
		std::forward<Args>(args)...
	);
}

} // namespace koti
