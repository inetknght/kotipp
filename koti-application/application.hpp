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
	enum class daemonize_status
	{
		daemonized_parent,
		daemonized_child
	};

public:

	class exit_status
	{
	public:
		static
		exit_status
		success()
		{
			return {success_};
		}

		static
		exit_status
		failure()
		{
			return {failure_};
		}

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
		) const
		{
			return exit_status_;
		}

		bool
		is_success(
		) const
		{
			return success_ == exit_status_;
		}

		bool
		is_bad(
		) const
		{
			return success_ != exit_status_;
		}

		operator bool(
		) const
		{
			return is_success();
		}

	protected:
		exit_status(
			int status
		)
		: exit_status_(status)
		{
		}

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

private:
	step_list_type step_list_;
};

} // namespace koti
