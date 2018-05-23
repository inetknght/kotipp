
#include <gtest/gtest.h>

#include "net_listener_test.hpp"

namespace koti {

TYPED_TEST_P(net_listener_tests, listen_no_name_selected)
{
	auto & listener_ = this->remake();

	EXPECT_NO_THROW(listener_->listen());
	EXPECT_TRUE(listener_->is_open());
	EXPECT_TRUE(listener_->is_bound());
	EXPECT_TRUE(listener_->is_listening());
	EXPECT_FALSE(listener_->had_listener_error_);
	EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
	EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());
	EXPECT_FALSE(listener_->local_endpoint().path().empty());
	EXPECT_TRUE(listener_->local_endpoint().is_abstract());
}

REGISTER_TYPED_TEST_CASE_P(
	net_listener_tests,
		listen_no_name_selected
);
INSTANTIATE_TYPED_TEST_CASE_P(
	local,
	net_listener_tests,
	net_listener_local_tests
);

} // namespace koti
