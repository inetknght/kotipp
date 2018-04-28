
//#include <gtest/gtest.h>

//#include "net_listener_test.hpp"

//namespace koti {

//TYPED_TEST_P(net_listener_tests, listen_no_port_selected)
//{
//	auto & listener_ = this->listener_;

//	EXPECT_NO_THROW(listener_->listen());
//	EXPECT_TRUE(listener_->is_open());
//	EXPECT_TRUE(listener_->is_bound());
//	EXPECT_TRUE(listener_->is_listening());
//	EXPECT_FALSE(listener_->had_listener_error_);
//	EXPECT_EQ(listener_->last_listener_error_, boost::system::error_code());
//	EXPECT_EQ(listener_->last_listener_error_.message(), boost::system::error_code().message());
//	EXPECT_NE(listener_->local_endpoint().port(), 0u);
//}

//REGISTER_TYPED_TEST_CASE_P(
//	net_listener_tests,
//		listen_no_port_selected
//);
//INSTANTIATE_TYPED_TEST_CASE_P(
//	tcp,
//	net_listener_tests,
//	net_listener_tcp_tests
//);

//} // namespace koti
