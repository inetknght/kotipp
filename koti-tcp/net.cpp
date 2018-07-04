#include "net.hpp"

#include <boost/system/error_code.hpp>

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

local_stream::ucred
local_stream::remote_identity(
	socket & s,
	boost::system::error_code & ec
)
{
	// from `man socket` -> see also fcntl, see also unix
	// from `man fcntl`
	// from `man unix` -> SCM_CREDENTIALS, see also cmsg
	// from `man cmsg`
	// struct msghdr msgh;
	// struct cmsghdr *cmsg;
	// struct ucred *credptr;
	// for (
	// 	 cmsg = CMSG_FIRSTHDR(&cmsg);
	// 	 cmsg != nullptr;
	// 	 cmsg = CMSG_NEXTHDR(&msgh, cmsg)
	// )
	// {
	// 	if ( cmsg->cmsg_level != SOL_SOCKET )
	// 	{
	// 		continue;
	// 	}
	// 	if ( cmsg->cmsg_type != SCM_CREDENTIALS )
	// 	{
	// 		continue;
	// 	}
	// 	credptr = (struct ucred *)CMSG_DATA(cmsg);
	// }

	// from `man socket` -> see also getsockopt
	ucred ident;
	ident.pid = -1;
	ident.uid = -1;
	ident.gid = -1;

	socklen_t len = sizeof(ident);
	if ( getsockopt(s.native_handle(), SOL_SOCKET, SO_PEERCRED, &ident, &len) < 0 )
	{
		ec = boost::system::error_code{errno, boost::system::generic_category()};
	}
	else if ( sizeof(ident) != len )
	{
		assert(false && "oh snap, need to create our own error condition et al and tell the client code that getsockopt() behaves unexpectedly");
		ec = boost::system::error_code{EINVAL, boost::system::generic_category()};
	}

	return ident;
}

local_stream::ucred
local_stream::remote_identity(
	socket & s
)
{
	boost::system::error_code ec;
	auto ident = remote_identity(s, ec);
	if ( ec )
	{
		throw std::runtime_error{ec.message()};
	}
	return ident;
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
