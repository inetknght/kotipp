#include <gtest/gtest.h>

#include "../reserve.hpp"

#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace kotipp;

TEST(reserve_tests, foo) {
	{
		reserve r{100};
		assert(r.requested_capacity() == 100);

		r.request(200);
		assert(r.requested_capacity() == 200);
	}

	{
		std::vector<unsigned> foo = reserve{100}, bar;
		assert(100u <= foo.capacity());

		// technically not guaranteed by the standard (initial capacity is implementation-defined)
		// but just checking from experience.
		assert(bar.capacity() < foo.capacity());
	}

	// I'm not sure how to check for capacity on some of these.
	// So for now just compiling successfully ought to be enough of a test.
	// Feel free to offer suggestions or improve.
	{
		std::unordered_map<unsigned, unsigned> foo = reserve{100};
	}
	{
		std::unordered_multimap<unsigned, unsigned> foo = reserve{100};
	}
	{
		std::unordered_set<unsigned> foo = reserve{100};
	}
	{
		std::unordered_multiset<unsigned> foo = reserve{100};
	}
}
