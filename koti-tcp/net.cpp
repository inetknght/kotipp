#include "net.hpp"

namespace koti
{

tcp46::tcp46(
	const tcp &ffs
)	: tcp(ffs)
{
}


tcp4
tcp46::v4()
{
	return {};
}

tcp6
tcp46::v6()
{
	return {};
}

tcp4::tcp4()
	: tcp46(ip::tcp::v4())
{
}

tcp4::endpoint
tcp4::local_endpoint()
{
	endpoint local;
	local.address(ip::address::from_string("127.0.0.1"));
	return local;
}

bool
tcp4::is_bound(
	const socket & s
)
{
	boost::system::error_code ec;
	auto local = s.local_endpoint(ec);
	if ( ec )
	{
		return false;
	}

	return 0 != local.port();
}

bool
tcp4::is_bound(
	const acceptor & s
)
{
	boost::system::error_code ec;
	auto local = s.local_endpoint(ec);
	if ( ec )
	{
		return false;
	}

	return 0 != local.port();
}

tcp6::tcp6()
	: tcp46(ip::tcp::v6())
{
}

tcp6::endpoint
tcp6::local_endpoint()
{
	endpoint local;
	local.address(ip::address::from_string("::1"));
	return local;
}

bool
tcp6::is_bound(
	const socket & s
)
{
	boost::system::error_code ec;
	auto local = s.local_endpoint(ec);
	if ( ec )
	{
		return false;
	}

	return 0 != local.port();
}

bool
tcp6::is_bound(
	const acceptor & s
)
{
	boost::system::error_code ec;
	auto local = s.local_endpoint(ec);
	if ( ec )
	{
		return false;
	}

	return 0 != local.port();
}

bool
local_stream::endpoint::is_abstract() const
{
	const auto & p = path();
	return (false == p.empty() && p[0] == '\0');
}

local_stream::endpoint
local_stream::local_endpoint()
{
	return endpoint{};
}

bool
local_stream::is_bound(
	const socket & s
)
{
	boost::system::error_code ec;
	auto local = s.local_endpoint(ec);
	if ( ec )
	{
		return false;
	}

	return false == local.path().empty();
}

bool
local_stream::is_bound(
	const acceptor & s
)
{
	boost::system::error_code ec;
	auto local = s.local_endpoint(ec);
	if ( ec )
	{
		return false;
	}

	return false == local.path().empty();
}

} // namespace koti
