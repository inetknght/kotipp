
#include <gtest/gtest.h>

#include "tcp_listener_test.hpp"

namespace koti {

TEST_F(tcp_listener_tests, start_stop_listening)
{
	// listen by initialization,
	// and stop
	{
		listener_options_.address_str("127.0.0.1");
		EXPECT_NO_THROW(remake(listener_options_));
		EXPECT_TRUE(listener_->is_open());
		EXPECT_TRUE(listener_->is_bound());
		EXPECT_TRUE(listener_->is_listening());
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());

		EXPECT_NO_THROW(listener_->stop());
		EXPECT_FALSE(listener_->is_open());
		EXPECT_FALSE(listener_->is_bound());
		EXPECT_FALSE(listener_->is_listening());
		EXPECT_FALSE(listener_->had_listener_error_);
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());

		EXPECT_NO_THROW(listener_->stop());
	}

	// remake,
	// listen by instruction; by specific set-up,
	// and stop
	{
		EXPECT_NO_THROW(remake());
		EXPECT_FALSE(listener_->is_open());
		EXPECT_FALSE(listener_->is_bound());
		EXPECT_FALSE(listener_->is_listening());
		EXPECT_FALSE(listener_->had_listener_error_);
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());

		EXPECT_NO_THROW(listener_->stop());
		EXPECT_FALSE(listener_->is_open());
		EXPECT_FALSE(listener_->is_bound());
		EXPECT_FALSE(listener_->is_listening());
		EXPECT_FALSE(listener_->had_listener_error_);
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());

		EXPECT_NO_THROW(listener_->stop());
		EXPECT_NO_THROW(listener_->open());
		EXPECT_TRUE(listener_->is_open());
		EXPECT_FALSE(listener_->is_bound());
		EXPECT_FALSE(listener_->is_listening());
		EXPECT_FALSE(listener_->had_listener_error_);
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());

		EXPECT_NO_THROW(listener_->stop());
		EXPECT_NO_THROW(listener_->bind());
		EXPECT_TRUE(listener_->is_open());
		EXPECT_TRUE(listener_->is_bound());
		EXPECT_FALSE(listener_->is_listening());
		EXPECT_FALSE(listener_->had_listener_error_);
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());
		EXPECT_NE(listener_->local_endpoint().port(), 0);

		EXPECT_NO_THROW(listener_->listen());
		EXPECT_TRUE(listener_->is_open());
		EXPECT_TRUE(listener_->is_bound());
		EXPECT_TRUE(listener_->is_listening());
		EXPECT_FALSE(listener_->had_listener_error_);
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());
		EXPECT_NE(listener_->local_endpoint().port(), 0);
	}

	// remake,
	// listen by instruction
	{
		EXPECT_NO_THROW(remake());
		EXPECT_FALSE(listener_->is_open());
		EXPECT_FALSE(listener_->is_bound());
		EXPECT_FALSE(listener_->is_listening());
		EXPECT_FALSE(listener_->had_listener_error_);
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());

		EXPECT_NO_THROW(listener_->listen());
		EXPECT_TRUE(listener_->is_open());
		EXPECT_TRUE(listener_->is_bound());
		EXPECT_TRUE(listener_->is_listening());
		EXPECT_FALSE(listener_->had_listener_error_);
		EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
		EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());
	}
}


} // namespace koti
