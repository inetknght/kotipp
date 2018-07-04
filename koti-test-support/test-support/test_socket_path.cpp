
#include "test_socket_path.hpp"

#include <boost/filesystem/operations.hpp>

namespace koti {
namespace test {

fs::path
test_socket_path()
{
    auto tmp = fs::temp_directory_path();
    auto pid = ::getpid();
    tmp /= std::to_string(pid) + ".sock";
    return tmp;
}

} // namespace test
} // namespace koti
