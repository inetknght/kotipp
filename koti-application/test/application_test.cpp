extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
} // extern "C"

#include "../application.hpp"

#include "gtest/gtest.h"

#include <memory>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <functional>

#include "exceptions/timeout.hpp"

#include "test_support.hpp"

// This should be defined from the CMakeLists.txt, ADD_DEFINITIONS(-DTEST_DIR_ROOT=...)
#ifndef TEST_DIR_ROOT
#define TEST_DIR_ROOT ""
#endif

class test_application
: protected koti::application
{
public:
	using koti::application::application;
	virtual ~test_application() = default;
};

class application_test
: public
	::testing::Test
{
public:

	void
	reset()
	{
		app_.reset();
		configure();
		configure_raw();
		int argc = args_.size();
		char ** argv = const_cast<char**>(args_raw_.data());
		app_ = std::make_unique<koti::application>(
			koti::options::commandline_arguments{argc, argv}
		);
	}

	koti::application::exit_status
	run()
	{
		if ( ! app_ )
		{
			reset();
		}
		app_->io_context().post([&](){
			loop_calledback_ = true;
			app_->io_context().stop();
		});
		return app_->run();
	}

	void
	configure()
	{
		if ( args_.empty() )
		{
			args_.push_back("./test");
		}
	}

	void configure_raw()
	{
		args_raw_.clear();
		args_raw_.reserve(args_.size() + 1);
		for ( const auto & arg : args_ )
		{
			args_raw_.push_back(arg.c_str());
		}
		args_raw_.push_back(nullptr);
	}

	std::unique_ptr<koti::application> app_;
	std::vector<std::string> args_;
	std::vector<const char*> args_raw_;
	bool loop_calledback_ = false;
};

TEST_F(application_test, no_parameters) {
	reset();
	EXPECT_FALSE(loop_calledback_);
	EXPECT_NO_THROW(run());
	EXPECT_FALSE(loop_calledback_);
}

TEST_F(application_test, no_daemonize) {
	args_ = {"./test", "--no-daemonize"};
	reset();
	EXPECT_FALSE(loop_calledback_);
	EXPECT_NO_THROW(run());
	EXPECT_TRUE(loop_calledback_);
}
