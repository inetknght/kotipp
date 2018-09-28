
extern "C" {
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
} // extern "C"

#include "application.hpp"

#include <cstdint>
#include <cstddef>
#include <cerrno>
#include <cstring>
#include <array>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include <boost/asio/local/connect_pair.hpp>

#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace koti {

application::daemonize_status
application::daemonize_status::child(
	daemon_socket_type && child_socket
)
{
	return {std::move(child_socket), 0};
}

application::daemonize_status
application::daemonize_status::parent(
	daemon_socket_type && parent_socket,
	pid_t child_pid
)
{
	return {std::move(parent_socket), child_pid};
}

bool
application::daemonize_status::is_child() const
{
	return 0 == pid_;
}

bool
application::daemonize_status::is_parent() const
{
	return 0 != pid_;
}

application::daemonize_status::daemonize_status(
	daemon_socket_type && socket,
	pid_t pid
)
: socket_(std::move(socket))
, pid_(pid)
{
}

application::exit_status
application::exit_status::success()
{
	return {success_};
}

application::exit_status
application::exit_status::failure()
{
	return {failure_};
}

int
application::exit_status::value(
) const
{
	return exit_status_;
}

bool
application::exit_status::is_success(
) const
{
	return success_ == exit_status_;
}

bool
application::exit_status::is_bad(
) const
{
	return success_ != exit_status_;
}

application::exit_status::
operator bool(
) const
{
	return is_success();
}

application::exit_status::exit_status(
	int status
)
: exit_status_(status)
{
}

constexpr const int application::exit_status::success_;
constexpr const int application::exit_status::failure_;

application::application(
	options::commandline_arguments options
)
: iox_()
, options_(options)
, daemonization_(daemonize_status::parent(daemon_socket_type{iox_}, ::getpid()))
{
	work_ = std::make_unique<executor_work_guard_type>(iox_.get_executor());

	options_.add(*this);

	step_list_ = {
		[&](){return options_.configure();},
		//
		// daemonize between configure() and validate()
		[&](){
			if ( no_daemonize_ )
			{
				// daemonize() will have opened syslog.
				// let's use stderr, since we're not daemonizing.
				syslog_ = spd::stderr_color_mt("kotid");
			}
			else
			{
				// Parent shall exit.
				// Child shall live on.
				// Error threw for main() to catch.
				daemonization_ = daemonize();
				if ( daemonization_.is_parent() )
				{
					return daemonized();
				}
			}
			return options::validate::ok;
		},
		[&](){return validate_configuration(options_);}
	};
}

application::step_list_type &
application::step_order()
{
	return step_list_;
}

application::exit_status
application::run()
{
	for ( auto & step : step_order() )
	{
		auto result = step();
		if ( options::validate::reject == result )
		{
			return exit_status::failure();
		}
		if ( options::validate::reject_not_failure == result )
		{
			return exit_status::success();
		}
		if ( options::validate::ok != result )
		{
			throw exception::unhandled_value(result);
		}
	}

	iox_.run();

	return exit_status::success();
}

const std::shared_ptr<spd::logger> &
application::log() const
{
	return syslog_;
}

application::io_context_type&
application::io_context()
{
	return iox_;
}

const application::io_context_type &
application::io_context() const
{
	return iox_;
}

std::unique_ptr<application::executor_work_guard_type> &
application::io_work()
{
	return work_;
}

const std::unique_ptr<application::executor_work_guard_type> &
application::io_work() const
{
	return work_;
}

options::validate
application::add_options(
	options & storage
)
{
	storage.descriptions().add_options()
	("no-daemonize", po::bool_switch(&no_daemonize_), "don't daemonize")
	;

	return options::validate::ok;
}

application::daemonize_status
application::daemonize()
{
	boost::asio::local::stream_protocol::socket
		parent_sock_(iox_),
		child_sock_(iox_);
	boost::asio::local::connect_pair(parent_sock_, child_sock_);

	iox_.notify_fork(asio::execution_context::fork_prepare);

	// detach from parent by forking
	pid_t pid = fork();

	// err?
	if ( pid < 0 )
	{
		iox_.notify_fork(asio::execution_context::fork_parent);
		int err = errno;
		throw std::system_error(
			err,
			std::system_category()
		);
	}

	// parent?
	if ( 0 < pid )
	{
		iox_.notify_fork(asio::execution_context::fork_parent);

		// Wait for child to die or close socket.
		child_sock_.close();
		std::array<char, 512> readbuf;
		std::size_t count = 0;

		// keep reading until closed or error message has been read
		while (0 == count)
		{
			boost::system::error_code ec;
			count += parent_sock_.read_some(
				boost::asio::buffer(readbuf.data() + count, readbuf.size() - count),
				ec
			);

			// Assume any error means remote end closed.
			if ( ec )
			{
				// Is a connection closed type of error?
				if ( (ec == boost::asio::error::connection_reset) || (ec == boost::asio::error::eof) )
				{
					// break read loop
					break;
				}
				throw std::system_error(
					ec.value(),
					std::system_category()
				);
			}

			// Child is reporting an error message, and readbuf is full?
			if (readbuf.size() == count)
			{
				// break read loop
				break;
			}
		}

		// Child reported an error message?
		if (0 < count)
		{
			std::string s(readbuf.data(), count);
			throw std::runtime_error(s);
		}

		// It appears the child successfully daemonized. We're done here.
		return daemonize_status::parent(std::move(parent_sock_), pid);
	}

	iox_.notify_fork(asio::execution_context::fork_child);
	parent_sock_.close();

	// set umask
	umask(0);

	// open syslog
	syslog_ = spd::syslog_logger_mt("kotid", "kotid", LOG_NOWAIT | LOG_PID, LOG_USER);
	syslog_->info("kotid starting");

	// generate a new session ID
	session_id_ = setsid();
	if ( session_id_ < 0 )
	{
		int err = errno;
		std::string_view sv(std::strerror(err));
		syslog_->critical("setsid(): {}", sv);
		boost::system::error_code ec; // to not throw from asio
		child_sock_.write_some(boost::asio::buffer("setsid(): "), ec);
		child_sock_.write_some(boost::asio::buffer(sv), ec);
		throw std::system_error(
			err,
			std::system_category()
		);
	}

	boost::system::error_code ec;
	fs::current_path("/", ec);
	if ( ec )
	{
		auto msg = ec.message();
		syslog_->critical("chdir(\"/\"): {}", msg);
		boost::system::error_code ec; // to not throw from asio
		child_sock_.write_some(boost::asio::buffer("chdir(\"/\"): "), ec);
		child_sock_.write_some(boost::asio::buffer(msg), ec);
		throw std::system_error(ec.value(), std::system_category());
	}

	::close(STDIN_FILENO);
	::close(STDOUT_FILENO);
	::close(STDERR_FILENO);

	return daemonize_status::child(std::move(child_sock_));
}

options::validate
application::daemonized()
{
	return options::validate::reject_not_failure;
}

options::validate
application::validate_configuration(
	options &
)
{
	return options::validate::ok;
}

} // namespace koti
