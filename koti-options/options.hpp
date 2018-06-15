#pragma once

#include <vector>
#include <stdexcept>
#include <type_traits>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

namespace koti {

class options
{
public:
	using description = po::options_description;
	using configuration = po::variables_map;

	configuration & get_configuration();
	const configuration & get_configuration() const;

	options() = default;
	options(const options & copy_ctor) = default;
	options(options && move_ctor) = default;
	options & operator=(const options & copy_assign) = default;
	options & operator=(options && move_assign) = default;
	~options() = default;

	enum class validate
	{
		// options were rejected.
		// eg `command --bad-argument` should EXIT_FAILURE
		reject = 0,

		// everything looks dandy
		ok = 1,

		// options were rejected, but not for a failing reason
		// eg `command --help` should not EXIT_FAILURE
		reject_not_failure = 2
	};

	class configurator
	{
	public:
		virtual
		validate
		add_options(
			options &
		) = 0;

		virtual
		validate
		validate_configuration(
			options &
		) = 0;
	protected:
		description descriptions_;
	};

	using configurator_list = std::vector<configurator*>;

	configurator_list &
	configurators();

	const configurator_list &
	configurators() const;

	options &
	add(configurator & p);

	class commandline_arguments
	{
	public:
		commandline_arguments() = default;
		commandline_arguments(const commandline_arguments & copy_ctor) = default;
		commandline_arguments(commandline_arguments && move_ctor) = default;
		commandline_arguments & operator=(const commandline_arguments& copy_assign) = default;
		commandline_arguments & operator=(commandline_arguments && move_assign) = default;
		~commandline_arguments() = default;

		commandline_arguments(int argc, char **argv);

		int argc() const;
		char **argv() const;

	protected:
		int argc_ = 0;
		char **argv_ = nullptr;
	};

	const commandline_arguments &
	arguments() const;

	commandline_arguments &
	arguments()
	{
		return args_;
	}

	options(
		const commandline_arguments & args
	);

	const description &
	descriptions() const
	{
		return descriptions_;
	}

	description &
	descriptions()
	{
		return descriptions_;
	}

	[[nodiscard]]
	validate
	configure();

protected:
	[[nodiscard]]
	validate
	configure_internal();

	[[nodiscard]]
	validate
	build_configurator_list_and_descriptions();

	[[nodiscard]]
	validate
	parse_commandline();

	[[nodiscard]]
	validate
	notify_and_validate();

	configurator_list configurators_;

	commandline_arguments args_;

	configuration configuration_;
	description descriptions_;
};

} // namespace koti
