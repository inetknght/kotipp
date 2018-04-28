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

enum class listener_handler_result
{
	cancel_and_stop,
	ignore_error,
	ignore_connection
};

template <
	class protocol
>
class listener_handler {
public:
	using socket = typename protocol::socket;
	using acceptor = typename protocol::acceptor;
	using endpoint = typename protocol::endpoint;

	void
	on_new_socket(
		socket && s,
		const endpoint & remote_endpoint
	);

	listener_handler_result
	on_listener_error(
		const boost::system::error_code & ec,
		const std::string & msg
	);
};

template <class header_only = void>
std::ostream & operator<<(
	std::ostream & o,
	const listener_handler_result & v
)
{
	using e = listener_handler_result;

	switch (v)
	{
	case e::cancel_and_stop:
		o << "cancel_and_stop"; break;
	case e::ignore_error:
		o << "ignore_error"; break;
	case e::ignore_connection:
		o << "ignore_connection"; break;
	}

	return o;
}

template <
	class protocol = tcp
>
class null_listener_handler
{
public:
	using socket = typename protocol::socket;
	using acceptor = typename protocol::acceptor;
	using endpoint = typename protocol::endpoint;
	using handler_type = listener_handler<protocol>;

	void
	on_new_socket(
		socket && s,
		const endpoint &
	)
	{
		s.close();
	}

	listener_handler_result
	on_listener_error(
		const boost::system::error_code &,
		const std::string &
	)
	{
		return listener_handler_result::ignore_error;
	}
};

template <
	class protocol,
	class listener_handler = null_listener_handler<protocol>,
	class time_source = std::chrono::steady_clock
>
class listener
	: virtual public koti::inheritable_shared_from_this
	, virtual public listener_logs
	, public listener_handler
	// TODO: timestamping belongs in the handler
	, private kotipp::timestamp<time_source>
	, private protocol::acceptor
{
public:
	using this_type = listener;

	using protocol_type = protocol;
	using pointer = std::shared_ptr<this_type>;
	using socket = typename protocol::socket;
	using acceptor = typename protocol::acceptor;
	using endpoint = typename protocol::endpoint;
	using logs_type = listener_logs;
	using listener_handler_type = listener_handler;

	using time_source_type = time_source;
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
		listener_logs::logger()->debug(
			"open():{}",
			last_ec_.first
		);
		if ( last_ec_.first )
		{
			last_ec_.second = "open()";
			log_error();
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
		if ( false == open() )
		{
			listener_logs::logger()->debug(
				"bind({})::open()",
				at
			);
			return false;
		}

		acceptor::bind(
			at,
			last_ec_.first
		);
		listener_logs::logger()->debug(
			"bind({}):{}",
			at,
			last_ec_.first
		);
		if ( last_ec_.first )
		{
			last_ec_.second = fmt::format(
				"bind({})::bind()",
				at
			);
			log_error();
			return false;
		}

		local_endpoint_ = acceptor::local_endpoint();
		if ( last_ec_.first )
		{
			last_ec_.second = fmt::format(
				"bind({})::local_endpoint()",
				local_endpoint_
			);
			log_error();
			acceptor::close();
			return false;
		}

		listener_logs::logger()->info(
			"listener bound to\t{}",
			local_endpoint_
		);
		return true;
	}

	bool
	listen()
	{
		if ( false == is_open() )
		{
			open();
		}

		if ( is_listening() )
		{
			return true;
		}

		if ( false == this_type::bind(protocol_type::local_endpoint()) )
		{
			listener_logs::logger()->debug(
				"listen()::bind()"
			);
			return false;
		}

		acceptor::listen(
			acceptor::max_listen_connections,
			last_ec_.first
		);
		if ( last_ec_.first )
		{
			last_ec_.second = "listen()::listen()";
			log_error();
			return false;
		}

		do_accept();
		return true;
	}

	void
	stop()
	{
		acceptor::close();
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
			last_ec_.second = "is_listening()";
			log_error();
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
	on_accept(const boost::system::error_code & ec)
	try
	{
		if(ec)
		{
			auto result = fail(ec, "accept()");
			if ( listener_handler_result::cancel_and_stop == result )
			{
				acceptor::close();
				return;
			}
			if ( listener_handler_result::ignore_connection == result )
			{
				return;
			}
			assert(listener_handler_result::ignore_error == result);
		}

		endpoint remote = socket_.remote_endpoint(
			last_ec_.first
		);
		if ( last_ec_.first )
		{
			last_ec_.second = fmt::format(
				"on_accept({})::remote_endpoint()",
				remote
			);
			log_error();
		}

		try
		{
			listener_handler::on_new_socket(std::move(socket_), remote);
		}
		catch (const std::exception & e)
		{
			logs_type::logger()->critical(
				"on_accept::on_new_socket({}) threw exception: {}",
				remote,
				e.what()
			);

			// Exceptions are for _exceptional circumstances_, so I don't really
			// care about the performance penalty of a flush after logging the
			// circumstances involved.
			logs_type::logger()->flush();

			// CONSIDER:
			// eating the exception seems to be bad behavior
			//
			// shutdown of the acceptor will stop new problems from arising
			// but will cause denial of service
			//
			// re-throwing will force the problem to be detected upstream
			// (potentially from a crashing application)
			// ... and may also cause denial of service ...
			// so if we can't avoid the possibility, then let's at least
			// try to get a crashdump with the unhandled exception
			socket_.close();
			throw;
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
		, logs_type()
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
		, logs_type()
		, listener_handler()
		, kotipp::timestamp<time_source>()
		, acceptor(ios)
		, ios_(ios)
		, socket_(ios)
	{
		// Open the acceptor.
		if ( false == open() )
		{
			ignore_failure(
				last_ec_.first,
				fmt::format("{0}{1}{3}",
					"listener(ios_,",
					desired_local_endpoint,
					")::open()"
				)
			);
			return;
		}

		// Bind to the server address.
		acceptor::bind(desired_local_endpoint, last_ec_.first);
		if ( last_ec_.first )
		{
			ignore_failure(last_ec_.first, "bind");
			return;
		}

		// Start listening for connections, but do not yet start accepting.
		acceptor::listen(boost::asio::socket_base::max_connections, last_ec_.first );
		if( last_ec_.first )
		{
			ignore_failure(last_ec_.first, "listen");
			return;
		}
	}

	void
	ignore_failure(boost::system::error_code ec, std::string msg)
	{
		auto result = fail(ec, msg);
		if ( listener_handler_result::ignore_error != result )
		{
			logs_type::logger()->debug("ignoring failure instructions\t{}\t{}\t{}", result, ec.message(), msg);
		}
	}

	void
	ignore_failure(std::string msg)
	{
		auto result = fail(msg);
		return ignore_failure(result);
	}

	void
	ignore_failure(listener_handler_result result)
	{
		if ( listener_handler_result::ignore_error != result )
		{
			logs_type::logger()->debug(
				"ignoring failure instructions\t{}\t{}\t{}",
				result,
				last_ec_.first.message(),
				last_ec_.second
			);
		}
	}

	listener_handler_result
	fail(boost::system::error_code ec, std::string msg)
	{
		last_ec_.first = std::move(ec);
		return fail(std::move(msg));
	}

	listener_handler_result
	fail(std::string msg)
	{
		last_ec_.second = std::move(msg);
		return fail();
	}

	listener_handler_result
	fail()
	{
		listener_handler_result result = listener_handler_result::cancel_and_stop;

		// todo: defer logging until we have a result,
		// try/catch around error_handler_
		// if caught, log that too
		try
		{
			result = listener_handler::on_listener_error(
				last_ec_.first,
				last_ec_.second
			);
			log_error();
		}
		catch (const std::exception & e)
		{
			logs_type::logger()->error("exception \n"
				"{}\n"
				"while processing\n"
				"{}\t{}",
				e.what(),
				last_ec_.first.message(), last_ec_.second
			);
		}

		return result;
	}

private:
	void
	log_error() const
	{
		listener_logs::logger()->error(
			"{}:\t{}",
			last_ec_.second,
			last_ec_.first.message()
		);
	}

	asio::io_service & ios_;
	socket socket_;
	error_descriptor last_ec_;

	// local_endpoint_ could be different from acceptor::local_endpoint()
	// because, eg, tcp protocol might have auto-selected a port if we bound 0
	endpoint local_endpoint_;
};

template <class ... Args>
using tcp4_listener = listener<tcp4, Args ...>;

template <class ... Args>
using tcp6_listener = listener<tcp6, Args ...>;

template <class ... Args>
using tcp_listener = tcp6_listener<Args ...>;

template <class ... Args>
using local_listener = listener<local_stream, Args ...>;

} // namespace koti
