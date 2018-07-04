#pragma once

#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

namespace koti {
namespace test {

fs::path
test_socket_path();

} // namespace test
} // namespace koti
