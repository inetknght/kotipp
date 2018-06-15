//#include "net_connection_test.hpp"

//namespace koti {

//TYPED_TEST_P(net_connection_tests, autobind)
//{
//	auto
//		a = TypeParam::make(this->ios_),
//		b = TypeParam::make(this->ios_);

//	auto local = TypeParam::protocol_type::local_endpoint();

//	a->open();

//	EXPECT_EQ(a->local_endpoint().port(), 0u);

//	a->bind(local);
//	EXPECT_NE(a->local_endpoint().port(), 0u);

//	b->async_connect(a->local_endpoint());

//	this->ios_.run();
//	this->ios_.reset();

//	EXPECT_FALSE(a->had_connected_);
//	EXPECT_TRUE(b->had_connected_);
//	EXPECT_NE(b->last_connection_error_, boost::system::error_code());
//	EXPECT_EQ(b->last_connection_error_, asio::error::basic_errors::connection_refused);
//}

//TYPED_TEST_P(net_connection_tests, no_duplicate_listens)
//{
//	auto
//		a = TypeParam::make(this->ios_),
//		b = TypeParam::make(this->ios_);

//	auto local = TypeParam::protocol_type::local_endpoint();

//	a->open();
//	b->open();

//	boost::system::error_code ec;
//	a->bind(local, ec);

//	ASSERT_EQ(ec, boost::system::error_code{});

//	auto at = a->local_endpoint();
//	b->bind(at, ec);
//	EXPECT_EQ(ec, boost::asio::error::address_in_use);
//	EXPECT_EQ(ec.message(), boost::system::error_code(boost::asio::error::address_in_use).message());
//}

//TYPED_TEST_P(net_connection_tests, connect_not_listening)
//{
//	auto
//		a = TypeParam::make(this->ios_),
//		b = TypeParam::make(this->ios_);

//	auto local = TypeParam::protocol_type::local_endpoint();

//	a->open();
//	b->open();

//	a->bind(local);
//	b->async_connect(b->local_endpoint());

//	this->ios_.run();
//	this->ios_.reset();

//	// async_connect failed: remote end is not listening (refused)
//	EXPECT_TRUE(b->had_connected_);
//	EXPECT_EQ(b->last_connection_error_, boost::asio::error::connection_refused);
//	EXPECT_EQ(b->last_connection_error_.message(), boost::system::error_code(boost::asio::error::connection_refused).message());
//}

//REGISTER_TYPED_TEST_CASE_P(
//	net_connection_tests,
//		autobind,
//		no_duplicate_listens,
//		connect_not_listening
//);
//INSTANTIATE_TYPED_TEST_CASE_P(
//	tcp,
//	net_connection_tests,
//	net_connection_tcp_tests
//);

//} // namespace koti
