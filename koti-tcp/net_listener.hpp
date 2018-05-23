#pragma once

#include <type_traits>

#include "fmt/ostream.h"
#include "spdlog/spdlog.h"
namespace spd = spdlog;

#include "net.hpp"

#include <timestamp.hpp>

namespace koti {

template <class, class, class>
class listener;

class listener_logs {
protected:
	using logging_pointer = std::shared_ptr<spd::logger>;

	static const std::string_view & logger_name()
	{
		static const std::string_view
		logger_name{"listener"};
		return logger_name;
	}

	static logging_pointer & logger()
	{
		static logging_pointer
		logger{spd::stdout_color_mt({
				listener_logs::logger_name().begin(),
				listener_logs::logger_name().end()
			})};
	
		return logger;
	}
};

enum class listener_action
{
	cancel_and_stop,
	normal,
	reject_connection
};

class listener_handler {
public:
	listener_action
	on_listener_bound(
	);

	listener_action
	on_listener_listening(
	);

	template <
		class Socket
	>
	listener_action
	on_new_socket(
		Socket &&
	);

	void
	on_listener_closed(
	);

	listener_action
	on_listener_error(
		const boost::system::error_code & ec,
		const std::string & listener_message
	);
};

template <class header_only = void>
std::ostream & operator<<(
	std::ostream & o,
	const listener_action & v
)
{
	using e = listener_action;

	switch (v)
	{
	case e::cancel_and_stop:
		o << "cancel_and_stop"; break;
	case e::normal:
		o << "normal"; break;
	}

	return o;
}

class null_listener_handler
{
public:
	listener_action
	on_listener_bound(
	)
	{
		return listener_action::normal;
	}

	listener_action
	on_listener_listening(
	)
	{
		return listener_action::normal;
	}

	template <
		class Socket
	>
	listener_action
	on_new_socket(
		Socket && s
	)
	{
		s.close();
		return listener_action::normal;
	}

	void
	on_listener_closed(
	)
	{
	}

	listener_action
	on_listener_error(
		const boost::system::error_code &,
		const std::string &
	)
	{
		return listener_action::normal;
	}
};

template <
	class Protocol,
	class ListenerHandler = null_listener_handler,
	class TimeSource = std::chrono::steady_clock
>
class listener
	: virtual public koti::inheritable_shared_from_this
	, public ListenerHandler
	// TODO: timestamping belongs in the handler
	, private kotipp::timestamp<TimeSource>
	, private Protocol::acceptor
{
public:
	using this_type = listener;

	using protocol = Protocol;
	using pointer = std::shared_ptr<this_type>;
	using socket = typename protocol::socket;
	using acceptor = typename protocol::acceptor;
	using endpoint = typename protocol::endpoint;
	using logs_type = listener_logs;
	using listener_handler = ListenerHandler;

	using time_source = TimeSource;
	using time_point = typename time_source::time_point;
	using time_duration = typename time_point::duration;

	// .first is the error code
	// .second is the message, if any
	using error_descriptor = std::pair<
		boost::system::error_code,
		std::string
	>;

	static pointer make(
		boost::asio::io_service & ios
	)
	{
		return koti::protected_make_shared_enabler<this_type>(ios);
	}

	static pointer make(
		boost::asio::io_service & ios,
		endpoint endpoint
	)
	{
		return koti::protected_make_shared_enabler<this_type>(ios, endpoint);
	}

	void set_reuse_address(bool to_reuse = true)
	{
		typename acceptor::reuse_address option(to_reuse);
		acceptor::set_option(option);
	}

	bool is_reuse_address() const
	{
		typename acceptor::reuse_address option;
		acceptor::get_option(option);
		return option.value();
	}

	int
	set_linger(
		bool is_set = true,
		int timeout_in_seconds = 5
	)
	{
		typename acceptor::linger option(is_set, timeout_in_seconds);
		acceptor::set_option(option);
		return option.timeout();
	}

	bool
	is_linger() const
	{
		typename acceptor::linger option;
		acceptor::get_option(option);
		return option.timeout();
	}

	bool
	open(
	)
	{
		if ( is_open() )
		{
			return true;
		}

		last_ec_ = {};
		acceptor::open(protocol{}, last_ec_.first);
		if ( last_ec_.first )
		{
			static_cast<listener_handler&>(*this).on_listener_error(
				last_ec_.first,
				last_ec_.second = fmt::format("open():{}", last_ec_.first)
			);
			return false;
		}

		return true;
	}

	bool
	bind(
		endpoint at = protocol::local_endpoint()
	)
	{
		if ( is_bound() )
		{
			return true;
		}

		last_ec_ = {};
		if ( false == this_type::open() )
		{
			// open() shall have already called on_listener_error()
			return false;
		}

		acceptor::bind(
			at,
			last_ec_.first
		);
		if ( last_ec_.first )
		{
			static_cast<listener_handler&>(*this).on_listener_error(
				last_ec_.first,
				last_ec_.second = fmt::format(
					"bind({})::bind():{}",
					at,
					last_ec_.first
				)
			);
			return false;
		}

		local_endpoint_ = acceptor::local_endpoint();
		if ( last_ec_.first )
		{
			acceptor::close();
			static_cast<listener_handler&>(*this).on_listener_error(
				last_ec_.first,
				last_ec_.second = fmt::format(
					"bind({})::local_endpoint():{}",
					local_endpoint_,
					last_ec_.first
				)
			);
			return false;
		}

		auto handler_says = static_cast<listener_handler&>(*this).on_listener_bound();
		if ( listener_action::cancel_and_stop == handler_says )
		{
			this_type::stop();
			return false;
		}
		return true;
	}

	bool
	listen(endpoint at_addr = protocol::local_endpoint())
	{
		if ( is_listening() )
		{
			return true;
		}

		if ( false == this_type::bind(at_addr) )
		{
			// bind() shall have already called on_listener_error
			return false;
		}

		acceptor::listen(
			acceptor::max_listen_connections,
			last_ec_.first
		);
		if ( last_ec_.first )
		{
			static_cast<listener_handler&>(*this).on_listener_error(
				last_ec_.first,
				last_ec_.second = "listen()::listen()"
			);
			return false;
		}

		do_accept();

		auto handler_says = static_cast<listener_handler&>(*this).on_listener_listening();
		if ( listener_action::cancel_and_stop == handler_says )
		{
			this_type::stop();
			return false;
		}
		return true;
	}

	void
	stop()
	{
		acceptor::close();
		static_cast<listener_handler&>(*this).on_listener_closed();
	}

	using acceptor::is_open;

	bool
	is_bound()
	{
		if ( false == is_open() )
		{
			return false;
		}

		return protocol::is_bound(static_cast<acceptor&>(*this));
	}

	bool
	is_listening()
	{
		if ( false == is_bound() )
		{
			return false;
		}

		last_ec_ = {};
		is_listening_option option;
		acceptor::get_option(option, last_ec_.first);
		if ( last_ec_.first )
		{
			static_cast<listener_handler&>(*this).on_listener_error(
				last_ec_.first,
				last_ec_.second = "is_listening()"
			);
			return false;
		}

		return option;
	}

	endpoint
	local_endpoint()
	{
		if ( false == is_bound() )
		{
			return {};
		}

		return local_endpoint_;
	}

	void
	do_accept()
	{
		acceptor::async_accept(
			socket_,
			[=](const boost::system::error_code & ec)
		{
			return this->on_accept(ec);
		});
	}

	void
	on_accept(
		const boost::system::error_code & ec
	)
	try
	{
		if(ec)
		{
			auto result = static_cast<listener_handler&>(*this).on_listener_error(
				last_ec_.first = ec,
				last_ec_.second = "accept()"
			);
			if ( listener_action::cancel_and_stop == result )
			{
				acceptor::close();
				return;
			}
		}

		auto result = static_cast<listener_handler&>(*this).on_new_socket(std::move(socket_));
		if ( listener_action::reject_connection == result )
		{
			socket_.close();
		}

		// Specifically: update timestamp upon _exit_ of on_accept()
		kotipp::timestamp<time_source>::stamp_now();
		socket_ = socket(ios_);
		do_accept();
	}
	catch (...)
	{
		// Specifically: update timestamp upon _exit_ of on_accept()
		kotipp::timestamp<time_source>::stamp_now();
		throw;
	}

	const error_descriptor &
	last_error() const
	{
		return last_ec_;
	}

	void
	clear_error()
	{
		last_ec_ = {{}, {}};
	}

	time_point
	last_accept_time() const
	{
		return kotipp::timestamp<time_source>::previous();
	}

	time_duration
	connection_time() const
	{
		return kotipp::timestamp<time_source>::duration_since_stamp();
	}

protected:

#define BOOST_ASIO_OS_DEF_SO_ACCEPTCONN SO_ACCEPTCONN
	using is_listening_option = asio::detail::socket_option::boolean<
		BOOST_ASIO_OS_DEF(SOL_SOCKET),
		BOOST_ASIO_OS_DEF(SO_ACCEPTCONN)
	>;

	listener(
		asio::io_service & ios
	)
		: koti::inheritable_shared_from_this()
		, listener_handler()
		, kotipp::timestamp<time_source>()
		, acceptor(ios)
		, ios_(ios)
		, socket_(ios)
	{
		
	}

	listener(
		asio::io_service & ios,
		endpoint desired_local_endpoint
	)
		: koti::inheritable_shared_from_this()
		, listener_handler()
		, kotipp::timestamp<time_source>()
		, acceptor(ios)
		, ios_(ios)
		, socket_(ios)
	{
		if ( false == this->listen(desired_local_endpoint) )
		{
			throw std::runtime_error{last_ec_.first.message()};
		}
	}

private:
	asio::io_service & ios_;
	socket socket_;
	error_descriptor last_ec_;

	// local_endpoint_ could be different from acceptor::local_endpoint()
	// because, eg, tcp protocol might have auto-selected a port if we bound 0
	endpoint local_endpoint_;
};

/*
template <
	class Protocol,
	class ListenerHandler,
	class TimeSource = 
>
class listener_logging_policy
: private listener_logs
, private listener<
	typename Listener::protocol,
	listener_logging_policy,
	typename Listener::time_source
>
, private Listener::listener_handler
{
	// My inherited type
	using real_listener_type =
		listener<
			typename Listener::protocol,
			listener_logging_policy,
			typename Listener::time_source
		>;

	// The type I am pretending to be.
	using faux_listener_type = Listener;
public:
	listener_action
	on_listener_bound(
	)
	{
		listener_logs::logger()->info(
			"bind()	{}",
			real_listener_type::local_endpoint()
		);
		return listener_action::normal;
	}

	listener_action
	on_listener_listening(
	)
	{
		listener_logs::logger()->info(
			"listen()	{}",
			real_listener_type::local_endpoint()
		);
		return listener_action::normal;
	}

	template <
		class Socket
	>
	listener_action
	on_new_socket(
		Socket && s
	)
	{
		listener_logs::logger()->info(
			"incoming socket	{}	{}",
			real_listener_type::local_endpoint(),
			s.remote_endpoint()
		);
		return listener_action::normal;
	}

	void
	on_listener_closed(
	)
	{
	}

	listener_action
	on_listener_error(
		const boost::system::error_code & ec,
		const std::string & listener_message
	)
	{
		listener_logs::logger()->error(
			"listener error: {}	{}",
			listener_message,
			ec
		);
		return listener_action::normal;
	}
};
*/

template <class ... Args>
using tcp4_listener = listener<tcp4, Args ...>;

template <class ... Args>
using tcp6_listener = listener<tcp6, Args ...>;

template <class ... Args>
using tcp_listener = tcp6_listener<Args ...>;

template <class ... Args>
using local_listener = listener<local_stream, Args ...>;

} // namespace koti
