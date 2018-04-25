#include "net_connection_test.hpp"

namespace koti {

test_clock::time_point test_clock::now_ = clock::now();

TYPED_TEST_P(net_connection_tests, ctor_dtor)
{
	auto ptr = TypeParam::make(this->ios_);
}

TYPED_TEST_P(net_connection_tests, cannot_bind_acceptor_to_existing_endpoint)
{
	auto
		a = TypeParam::make(this->ios_);
	
	using protocol = typename TypeParam::protocol_type;
	using acceptor_type = typename protocol::acceptor;

	auto local = TypeParam::protocol_type::local_endpoint();

	a->open();

	boost::system::error_code ec;
	a->bind(local, ec);

	ASSERT_EQ(ec, boost::system::error_code{});

	std::shared_ptr<acceptor_type> acceptor = std::make_shared<acceptor_type>(this->ios_);
	EXPECT_NO_THROW(acceptor->open());
	EXPECT_NO_THROW(acceptor->bind(a->local_endpoint(), ec));

	EXPECT_EQ(ec, boost::asio::error::address_in_use);
	EXPECT_EQ(ec.message(), boost::system::error_code(boost::asio::error::address_in_use).message());
}

TYPED_TEST_P(net_connection_tests, connect_async_send_receive)
{
	auto
		a = TypeParam::make(this->ios_),
		b = TypeParam::make(this->ios_);

	auto local = TypeParam::protocol_type::local_endpoint();

	using protocol = typename TypeParam::protocol_type;
	using acceptor_type = typename protocol::acceptor;

	std::shared_ptr<acceptor_type> acceptor = std::make_shared<acceptor_type>(this->ios_);
	EXPECT_NO_THROW(acceptor->open());
	EXPECT_NO_THROW(acceptor->bind(local));
	EXPECT_NO_THROW(acceptor->listen());
	EXPECT_NO_THROW(acceptor->async_accept(
		a->as_socket(),
		[&](const boost::system::error_code & ec)->void
		{
			EXPECT_EQ(a->on_connected(ec), connection_handler::action::normal);
		}));

	auto connectedish = test_clock::set_now(test_clock::now());
	b->last_connection_error_ = {};
	b->had_connected_ = false;
	a->last_connection_error_ = {};
	a->had_connected_ = false;

	EXPECT_NO_THROW(b->async_connect(acceptor->local_endpoint()));

	this->ios_.run();
	this->ios_.reset();

	auto nowish = test_clock::set_now(connectedish + std::chrono::seconds(10));
	ASSERT_LT(connectedish, nowish);
	EXPECT_EQ(connectedish, a->connection_time());
	EXPECT_EQ(connectedish, b->connection_time());
	EXPECT_EQ(a->connection_duration(), (nowish - connectedish));
	EXPECT_EQ(b->connection_duration(), (nowish - connectedish));

	EXPECT_TRUE(a->had_connected_);
	EXPECT_EQ(a->last_connection_error_, boost::system::error_code());
	EXPECT_EQ(a->last_connection_error_.message(), boost::system::error_code().message());

	EXPECT_TRUE(b->had_connected_);
	EXPECT_EQ(b->last_connection_error_, boost::system::error_code());
	EXPECT_EQ(b->last_connection_error_.message(), boost::system::error_code().message());

	EXPECT_NO_THROW(b->async_write_some(asio::buffer("foobar")));
	EXPECT_NO_THROW(a->async_read_some());

	this->ios_.run();
	this->ios_.reset();

	// Read size of zero: buffer was empty.
	EXPECT_TRUE(a->had_read_complete_);
	EXPECT_EQ(a->last_connection_error_, boost::system::error_code());
	EXPECT_EQ(a->last_connection_error_.message(), boost::system::error_code().message());
	EXPECT_EQ(a->last_read_size(), 0u);
	EXPECT_FALSE(a->had_write_complete_);

	EXPECT_TRUE(b->had_write_complete_);
	EXPECT_EQ(b->last_connection_error_, boost::system::error_code());
	EXPECT_EQ(b->last_connection_error_.message(), boost::system::error_code().message());
	EXPECT_EQ(b->last_write_size(), 1+strlen("foobar"));
	EXPECT_FALSE(b->had_read_complete_);

	a->had_read_complete_ = false;
	a->last_connection_error_ = boost::system::error_code();
	b->had_write_complete_ = false;
	b->last_connection_error_ = boost::system::error_code();

	a->read_buffer().resize(1024);
	EXPECT_NO_THROW(a->async_read_some());

	this->ios_.run();
	this->ios_.reset();

	// note read size vs buffer size
	EXPECT_TRUE(a->had_read_complete_);
	EXPECT_EQ(a->last_connection_error_, boost::system::error_code());
	EXPECT_EQ(a->last_connection_error_.message(), boost::system::error_code().message());
	EXPECT_EQ(a->last_read_size(), 1+strlen("foobar"));
	EXPECT_EQ(a->read_buffer().size(), 1024u);

	EXPECT_NO_THROW(a->close());
	EXPECT_NO_THROW(b->close());

	this->ios_.run();
	this->ios_.reset();
}

REGISTER_TYPED_TEST_CASE_P(
	net_connection_tests,
		ctor_dtor,
		connect_async_send_receive,
		cannot_bind_acceptor_to_existing_endpoint
);
INSTANTIATE_TYPED_TEST_CASE_P(
	all,
	net_connection_tests,
	net_connection_all_tests
);

} // namespace koti
