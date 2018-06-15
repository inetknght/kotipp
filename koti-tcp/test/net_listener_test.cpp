
//#include <gtest/gtest.h>

//#include "net_listener_test.hpp"

//namespace koti {

//TYPED_TEST_P(net_listener_tests, ctor_dtor)
//{
//	auto a = this->remake();
//}

//TYPED_TEST_P(net_listener_tests, start_stop_listening)
//{
//	auto & listener_ = this->listener_;

//	using protocol = typename TypeParam::protocol;

//	// listen by initialization,
//	// and stop
//	{
//		EXPECT_NO_THROW(this->remake(protocol::local_endpoint()));
//		EXPECT_TRUE(listener_->is_open());
//		EXPECT_TRUE(listener_->is_bound());
//		EXPECT_TRUE(listener_->is_listening());
//		EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
//		EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());

//		EXPECT_NO_THROW(listener_->stop());
//		EXPECT_FALSE(listener_->is_open());
//		EXPECT_FALSE(listener_->is_bound());
//		EXPECT_FALSE(listener_->is_listening());
//		EXPECT_FALSE(listener_->had_listener_error_);
//		EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
//		EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());

//		EXPECT_NO_THROW(listener_->stop());
//	}

//	// remake,
//	// listen by instruction; by specific set-up,
//	// and stop
//	{
//		EXPECT_NO_THROW(this->remake());
//		EXPECT_FALSE(listener_->is_open());
//		EXPECT_FALSE(listener_->is_bound());
//		EXPECT_FALSE(listener_->is_listening());
//		EXPECT_FALSE(listener_->had_listener_error_);
//		EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
//		EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());

//		EXPECT_NO_THROW(listener_->stop());
//		EXPECT_FALSE(listener_->is_open());
//		EXPECT_FALSE(listener_->is_bound());
//		EXPECT_FALSE(listener_->is_listening());
//		EXPECT_FALSE(listener_->had_listener_error_);
//		EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
//		EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());

//		EXPECT_NO_THROW(listener_->stop());
//		EXPECT_NO_THROW(listener_->open());
//		EXPECT_TRUE(listener_->is_open());
//		EXPECT_FALSE(listener_->is_bound());
//		EXPECT_FALSE(listener_->is_listening());
//		EXPECT_FALSE(listener_->had_listener_error_);
//		EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
//		EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());

//		EXPECT_NO_THROW(listener_->listen());
//		EXPECT_TRUE(listener_->is_open());
//		EXPECT_TRUE(listener_->is_bound());
//		EXPECT_TRUE(listener_->is_listening());
//		EXPECT_FALSE(listener_->had_listener_error_);
//		EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
//		EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());
//	}

//	// remake,
//	// listen by instruction
//	{
//		EXPECT_NO_THROW(this->remake());
//		EXPECT_FALSE(listener_->is_open());
//		EXPECT_FALSE(listener_->is_bound());
//		EXPECT_FALSE(listener_->is_listening());
//		EXPECT_FALSE(listener_->had_listener_error_);
//		EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
//		EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());

//		EXPECT_NO_THROW(listener_->listen());
//		EXPECT_TRUE(listener_->is_open());
//		EXPECT_TRUE(listener_->is_bound());
//		EXPECT_TRUE(listener_->is_listening());
//		EXPECT_FALSE(listener_->had_listener_error_);
//		EXPECT_EQ(listener_->last_listener_error_.first, boost::system::error_code());
//		EXPECT_EQ(listener_->last_listener_error_.first.message(), boost::system::error_code().message());
//	}
//}


//REGISTER_TYPED_TEST_CASE_P(
//	net_listener_tests,
//		ctor_dtor,
//		start_stop_listening
//);
//INSTANTIATE_TYPED_TEST_CASE_P(
//	all,
//	net_listener_tests,
//	net_listener_all_tests
//);

//} // namespace koti
