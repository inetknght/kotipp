#include "options.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace spd = spdlog;

#include <algorithm>
#include <iostream>
#include <vector>

#include "exceptions/unhandled_value.hpp"

namespace koti {

options::configuration &
options::get_configuration()
{
	return configuration_;
}

const options::configuration &
options::get_configuration() const
{
	return configuration_;
}

options::configurator_list &
options::configurators()
{
	return configurators_;
}

const options::configurator_list &
options::configurators() const
{
	return configurators_;
}

int
options::commandline_arguments::argc() const
{
	return argc_;
}

char **
options::commandline_arguments::argv() const
{
	return argv_;
}

options &
options::add(configurator & p)
{
	configurators_.push_back(&p);
	return *this;
}

options::commandline_arguments::commandline_arguments(
	int argc,
	char **argv
)
	: argc_(argc)
	, argv_(argv)
{
	if ( nullptr != argv_[argc] )
	{
		std::cerr << "warning: argv[" << argc << "] is not null (POSIX violation)\n";
	}
}

const options::commandline_arguments &
options::arguments() const
{
	return args_;
}

options::options(
	const commandline_arguments &args
) :
	args_(args)
{
}

options::validate
options::configure()
{
	return configure_internal();
}

options::validate
options::configure_internal()
{

	using step_description = std::function<validate()>;
	for ( auto step : std::initializer_list<step_description>{
		[&](){return build_configurator_list_and_descriptions();},
		[&](){return parse_commandline();},
		[&](){return notify_and_validate();}
	})
	{
		auto result = step();
		switch (result)
		{
		case validate::ok: continue;
		case validate::reject_not_failure:
			[[fallthrough]]
		case validate::reject:
			return result;
		}
	}

	return validate::ok;
}

options::validate
options::build_configurator_list_and_descriptions()
{
	validate result = validate::ok;
	for (
		std::size_t
			at = 0,
			until = configurators_.size();
		at < until;
		++at
	)
	{
		result = configurators_[at]->add_options(*this);
		switch (result)
		{
		case validate::ok:
			until = configurators_.size();
			continue;
		case validate::reject_not_failure:
			[[fallthrough]]
		case validate::reject:
			return result;
		default:
			throw koti::exception::unhandled_value{
				{"koti::options::validate"},
				result
			};
		};
	}
	return result;
}

options::validate
options::parse_commandline()
{
	try
	{
		std::vector<char *> argv(args_.argc(), nullptr);
		std::copy(
			&args_.argv()[0],
			&args_.argv()[args_.argc()],
			argv.begin()
		);

		po::store(
			po::command_line_parser(argv.size(), argv.data())
			.options(descriptions_)
			.run(),
			configuration_
		);

		po::notify(configuration_);
	}
	catch (const std::exception & e)
	{
		auto console = spd::stderr_color_mt("console");
		console->error(e.what());
		return validate::reject;
	}

	return validate::ok;
}

options::validate
options::notify_and_validate()
{
	for ( auto * configurator : configurators_ )
	{
		auto valid = configurator->validate_configuration(*this);
		switch (valid)
		{
		case validate::ok: continue;
		case validate::reject:
			[[fallthrough]];
		case validate::reject_not_failure:
			return valid;
		}
	}

	return validate::ok;
}

} // namespace koti
