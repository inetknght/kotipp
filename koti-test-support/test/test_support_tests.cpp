
#include <gtest/gtest.h>

#include "../test_support.hpp"
#include "../test-support/test_socket_path.hpp"

TEST(test_support_tests, foobar)
{
    EXPECT_NO_THROW(koti::test::test_socket_path());
}
