#include "net.hpp"

namespace koti
{

tcp4::tcp4()
	: ip::tcp(ip::tcp::v4())
{
}

tcp4
tcp4::v4()
{
	return {};
}

tcp6
tcp4::v6()
{
	return {};
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
	: ip::tcp(ip::tcp::v6())
{
}

tcp4
tcp6::v4()
{
	return {};
}

tcp6
tcp6::v6()
{
	return {};
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

local_stream::endpoint
local_stream::local_endpoint()
{
	std::string anonymous_path;
	anonymous_path.reserve(16);
	anonymous_path.push_back('\0');
	anonymous_path += std::to_string(getuid());
	anonymous_path.push_back('_');
	anonymous_path += std::to_string(getpid());
	return endpoint{anonymous_path};
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
