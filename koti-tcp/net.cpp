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

} // namespace koti
