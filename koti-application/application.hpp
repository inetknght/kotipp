#pragma once

extern "C" {
#include <sys/types.h>
} // extern "C"

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <boost/asio.hpp>
namespace asio = boost::asio;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "options.hpp"
#include "exceptions/unhandled_value.hpp"

#include "spdlog/logger.h"
namespace spd = spdlog;

namespace koti {

class application
: public options::configurator
{
	using daemon_socket_type = boost::asio::local::stream_protocol::socket;

public:
	class daemonize_status
	final
	{
	public:
		static
		daemonize_status
		child(
			daemon_socket_type && child_socket
		);

		static
		daemonize_status
		parent(
			daemon_socket_type && parent_socket,
			pid_t child_pid
		);

		bool
		is_child() const;

		bool
		is_parent() const;

		daemonize_status(daemonize_status && move_ctor) = default;
		daemonize_status & operator=(daemonize_status && move_assign) = default;

	protected:

		daemonize_status(daemon_socket_type && socket, pid_t pid);

		daemon_socket_type socket_;
		pid_t pid_ = 0;
	};

	class exit_status
	{
	public:
		static
		exit_status
		success();

		static
		exit_status
		failure();

		exit_status(
		) = delete;

		exit_status(
			const exit_status & copy_ctor
		) = default;

		exit_status &
		operator=(
			const exit_status & copy_assign
		) = default;

		exit_status(
			exit_status && move_ctor
		) = default;

		exit_status &
		operator=(
			exit_status && move_assign
		) = default;

		int
		value(
		) const;

		bool
		is_success(
		) const;

		bool
		is_bad(
		) const;

		operator bool(
		) const;

	protected:
		exit_status(
			int status
		);

		static constexpr const int success_ = EXIT_SUCCESS;
		static constexpr const int failure_ = EXIT_FAILURE;

		int exit_status_ = EXIT_FAILURE;
	};

	using step_list_type = std::vector<std::function<options::validate()>>;

	using io_context_type = asio::io_context;
	using io_executor_type = typename asio::io_context::executor_type;
	using executor_work_guard_type = asio::executor_work_guard<io_executor_type>;

	application(
		options::commandline_arguments options
	);

	virtual
	~application() = default;

	exit_status
	run();

	const std::shared_ptr<spd::logger> &
	log() const;

	io_context_type &
	io_context();

	const io_context_type &
	io_context() const;

	std::unique_ptr<executor_work_guard_type> &
	io_work();

	const std::unique_ptr<executor_work_guard_type> &
	io_work() const;

protected:

	virtual
	step_list_type &
	step_order();

	virtual
	options::validate
	add_options(
		options & storage
	) override;

	daemonize_status
	daemonize();

	virtual
	options::validate
	daemonized();

	virtual
	options::validate
	validate_configuration(
		options & storage
	) override;

	std::unique_ptr<executor_work_guard_type> work_;
	io_context_type iox_;
	options options_;
	std::shared_ptr<spd::logger> syslog_;
	pid_t session_id_ = 0;
	bool no_daemonize_ = false;

	daemonize_status daemonization_;

private:
	step_list_type step_list_;
};

} // namespace koti
