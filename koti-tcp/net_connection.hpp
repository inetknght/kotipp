//#pragma once

//#include "net.hpp"

//#include <chrono>
//#include <functional>
//#include <stdexcept>

//#include <reserve.hpp>
//#include <timestamp.hpp>

//namespace koti {

//template <class, class>
//class connection;

//enum class connection_action {
//	disconnect,
//	abort,
//	normal
//};

//class connection_handler {
//public:
//	using action = connection_action;

//	action
//	on_connected(const boost::system::error_code &);

//	action
//	on_write_complete(const boost::system::error_code &, std::size_t);

//	action
//	on_read_complete(const boost::system::error_code & ec, std::size_t transferred);

//	action
//	on_error(const boost::system::error_code &);

//	action
//	on_disconnect();
//};

//class null_connection_handler
//	: private connection_handler
//{
//public:
//	using action = typename connection_handler::action;

//	action
//	on_connected(const boost::system::error_code &)
//	{
//		return action::disconnect;
//	}

//	action
//	on_write_complete(const boost::system::error_code &, std::size_t)
//	{
//		return action::normal;
//	}

//	action
//	on_read_complete(const boost::system::error_code &, std::size_t)
//	{
//		return action::normal;
//	}

//	action
//	on_error(const boost::system::error_code &)
//	{
//		return action::abort;
//	}

//	action
//	on_disconnect()
//	{
//		return action::normal;
//	}
//};

//class buffered_read_connection_handler
//{
//public:
//	using action = connection_action;
//	using buffer_type = std::vector<char>;

//	buffer_type &
//	read_buffer()
//	{
//		return read_buffer_;
//	}

//	const buffer_type &
//	read_buffer() const
//	{
//		return read_buffer_;
//	}

//	std::size_t &
//	last_read_size()
//	{
//		return completed_read_length_;
//	}

//	const std::size_t &
//	last_read_size() const
//	{
//		return completed_read_length_;
//	}

//	action
//	on_read_complete(const boost::system::error_code &, std::size_t transferred)
//	{
//		completed_read_length_ = transferred;
//		return action::normal;
//	}

//protected:
//	std::vector<char> read_buffer_ = kotipp::reserve(4096);
//	std::size_t completed_read_length_ = 0;
//};

//class buffered_write_connection_handler
//{
//public:
//	using action = connection_action;
//	using buffer_type = std::vector<char>;

//	buffer_type &
//	write_buffer()
//	{
//		return write_buffer_;
//	}

//	const buffer_type &
//	write_buffer() const
//	{
//		return write_buffer_;
//	}

//	std::size_t &
//	last_write_size()
//	{
//		return completed_write_length_;
//	}

//	const std::size_t &
//	last_write_size() const
//	{
//		return completed_write_length_;
//	}

//	action
//	on_write_complete(const boost::system::error_code &, std::size_t transferred)
//	{
//		completed_write_length_ = transferred;
//		return action::normal;
//	}

//protected:
//	std::vector<char> write_buffer_ = kotipp::reserve(4096);
//	std::size_t completed_write_length_ = 0;
//};

//template <class TimeSource = std::chrono::steady_clock>
//class connection_timer_handler
//: private kotipp::timestamp<TimeSource>
//{
//public:
//	using action = connection_action;
//	using time_source = TimeSource;
//	using time_point = typename time_source::time_point;
//	using time_duration = typename time_point::duration;

//	action
//	on_connected(const boost::system::error_code &)
//	{
//		kotipp::timestamp<TimeSource>::stamp_now();
//		return action::normal;
//	}

//	time_point
//	connection_time() const
//	{
//		return kotipp::timestamp<TimeSource>::previous();
//	}

//	time_duration
//	connection_duration() const
//	{
//		return kotipp::timestamp<TimeSource>::duration_since_stamp();
//	}
//};

//template <
//	class Protocol = ip::tcp,
//	class ConnectionHandler = null_connection_handler
//>
//class connection
//	: private Protocol::socket
//{
//public:
//	using this_type = connection;
//	using protocol = Protocol;
//	using socket = typename protocol::socket;
//	using connection_handler = ConnectionHandler;
//	using error_code = boost::system::error_code;

//	using pointer = std::unique_ptr<connection_handler>;
//	using handler_pointer = typename connection_handler::pointer;

//	using action = connection_action;

//	connection(const connection & copy_ctor) = delete;
//	connection(connection && move_ctor) = default;
//	connection & operator=(const connection & copy_assign) = delete;
//	connection & operator=(connection && move_assign) = default;

//	template <class ... Args>
//	connection(
//		Args && ... args
//	)
//		: socket(std::forward<Args...>(args...))
//	{
//	}

//	socket & as_socket()
//	{
//		return static_cast<socket&>(*this);
//	}

//	const socket & as_socket() const
//	{
//		return static_cast<const socket&>(*this);
//	}

//	using socket::get_executor;
//	using socket::get_option;
//	using socket::local_endpoint;
//	using socket::remote_endpoint;

//	using socket::open;
//	using socket::bind;

//	//using socket_type::async_connect;
//	template <class ... Args>
//	auto async_connect(
//		Args && ... args
//	)
//	{
//		return socket::async_connect(
//			std::forward<Args>(args)...,
//			[&](const error_code & ec)
//		{
//			return static_cast<connection_handler&>(*this).on_connected(
//				ec
//			);
//		});
//	}

//	//using socket_type::async_write_some;
//	template <class ... Args>
//	auto async_write_some(
//		Args && ... args
//	)
//	{
//		return socket::async_write_some(
//			std::forward<Args>(args)...,
//			[&](const error_code & ec, std::size_t transferred)
//		{
//			return static_cast<connection_handler&>(*this).on_write_complete(
//				ec,
//				transferred
//			);
//		});
//	}

//	//using socket_type::async_read_some;
//	template <class ... Args>
//	auto async_read_some(
//		Args && ... args
//	)
//	{
//		return socket::async_read_some(
//			asio::buffer(read_buffer()),
//			std::forward<Args>(args)...,
//			[&](const error_code & ec, std::size_t transferred)
//		{
//			return static_cast<connection_handler&>(*this).on_read_complete(
//				ec,
//				transferred
//			);
//		});
//	}

//	using socket::shutdown;

//	using socket::cancel;
//	using socket::close;

//	using read_buffer_type = std::vector<char>;
//	read_buffer_type & read_buffer() { return read_buffer_; }
//	const read_buffer_type & read_buffer() const { return read_buffer_; }

//protected:
//	read_buffer_type read_buffer_;
//};

//template <
//	class handler
//>
//using tcp4_connection = connection<
//	tcp4,
//	handler
//>;

//template <
//	class handler
//>
//using tcp6_connection = connection<
//	tcp6,
//	handler
//>;

//template <
//	class handler
//>
//using tcp_connection = tcp6_connection<handler>;

//template <
//	class handler
//>
//using local_connection = connection<
//	local_stream,
//	handler
//>;

//} // namespace koti
