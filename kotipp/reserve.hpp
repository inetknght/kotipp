#pragma once

#include <cstddef>

namespace kotipp {

class reserve {
	size_t requested_capacity_ = 0;
public:
	reserve() = default;
	reserve(const reserve & copy_ctor) = default;
	reserve(reserve && move_ctor) = default;
	reserve & operator=(const reserve & copy_assign) = default;
	reserve & operator=(reserve && move_assign) = default;
	inline reserve(size_t requested_capacity) : requested_capacity_(requested_capacity) {}

	inline size_t requested_capacity() const { return requested_capacity_; }
	inline reserve & request(size_t new_requested_capacity) { requested_capacity_ = new_requested_capacity; return *this; }

	template <class T>
	inline operator T() {
		T t;
		t.reserve(requested_capacity());
		return t;
	}
};

} // namespace kotipp
