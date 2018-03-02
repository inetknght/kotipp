
#pragma once

#include <memory>
#include <stdexcept>
#include <cstdint>

namespace kotipp {

namespace detail
{

template <std::uintptr_t mask = 0>
constexpr std::uintptr_t count_mask()
{
	return
		(mask)
		? count_mask<(mask >> 1)>() + 1
		: 0;
}

static_assert(count_mask<0>() == 0);
static_assert(count_mask<1>() == 1);
static_assert(count_mask<3>() == 2);
static_assert(count_mask<7>() == 3);
static_assert(count_mask<15>() == 4);

constexpr std::uintptr_t alignment_bits(const std::uintptr_t resume)
{
	return
		(0 < resume)
		? (1 < resume)
			? 1 + alignment_bits(resume >> 1)
			: 0
		: 0;
}

static_assert(alignment_bits(0) == 0);
static_assert(alignment_bits(1) == 0);
static_assert(alignment_bits(2) == 1);
static_assert(alignment_bits(4) == 2);
static_assert(alignment_bits(8) == 3);
static_assert(alignment_bits(16) == 4);

template <typename T>
constexpr std::uintptr_t alignment_bits()
{
	constexpr const std::size_t align_size = alignof(T);
	// effectively, log2(alignof(F))
	// 0 = 0
	// 1 = 0
	// 2 = 1
	// 4 = 2
	// 8 = 3
	// etc

	return alignment_bits(align_size);
}

constexpr std::uintptr_t n_bit_mask(const unsigned bit_num)
{
	// 0 : 0b1
	// 1 : 0b11
	// 2 : 0b111
	// etc
	// So, becomes n_bits<N>() will constexpr-mask the [0,N]th bits
	return
		(0 < bit_num)
		? (n_bit_mask(bit_num - 1) << 1) | 1
		: 0;
}

static_assert(n_bit_mask(1) == 1u);
static_assert(n_bit_mask(2) == 3u);
static_assert(n_bit_mask(3) == 7u);
static_assert(n_bit_mask(4) == 15u);

template <typename T>
constexpr std::uintptr_t alignment_mask()
{
	constexpr const std::uintptr_t bits = alignment_bits<T>();
	return n_bit_mask(bits);
}

template <typename T>
struct static_assurance
{
	static constexpr std::uintptr_t alignment_bits() { return detail::alignment_bits<T>(); }
	static constexpr std::uintptr_t mask() { return detail::alignment_mask<T>(); }
	static constexpr std::uintptr_t mask_count() { return detail::count_mask<mask()>(); }
};

} // namespace detail

/**
 * @brief bitfield_ptr
 *  Allows you to (ab)use the first individual bits of a T* which are typically
 * always zero due to alignment, among other things.
 * @param T
 */
template <typename T, typename F = unsigned>
class bitfield_ptr
	final /* non-virtual dtor */
	: public detail::static_assurance<T>
{
public:
	using static_assurance = detail::static_assurance<T>;

	bitfield_ptr() = default;

	bitfield_ptr(T * ptr) :
		ptr_(std::move(ptr))
	{
	}

	bitfield_ptr(const bitfield_ptr & copy_ctor) = delete;

	bitfield_ptr(bitfield_ptr && move_ctor)
	{
		ptr_ = move_ctor.ptr_;
		move_ctor.ptr_ = reinterpret_cast<std::uintptr_t>(nullptr);
	}

	bitfield_ptr & operator=(T * ptr) {
		assign(ptr);
	}

	bitfield_ptr & operator=(const bitfield_ptr & copy_assign) = delete;

	bitfield_ptr & operator=(bitfield_ptr && move_assign)
	{
		ptr_ = move_assign.ptr_;
		move_assign.ptr_ = reinterpret_cast<std::uintptr_t>(nullptr);
		return *(this);
	}

	~bitfield_ptr()
	{
		clear();
	}

	T & operator*() const
	{
		return *pointer();
	}

	T * operator->() const
	{
		return pointer();
	}

	operator bool() const
	{
		return nullptr != pointer();
	}

	T * pointer() const
	{
		return reinterpret_cast<T*>(ptr_ & ~static_assurance::mask());
	}

	T * take()
	{
		T * ptr = pointer();
		ptr_ = (ptr_ & static_assurance::mask());
		return ptr;
	}

	void clear()
	{
		ptr_ = reinterpret_cast<std::uintptr_t>(nullptr);
	}

	T * assign(T * new_ptr)
	{
		T * old = take();
		ptr_ = reinterpret_cast<std::uintptr_t>(new_ptr);
		return old;
	}

	F flag() const
	{
		return static_cast<F>(ptr_) & static_assurance::mask();
	}

	F assign_flag(F new_flag)
	{
		F v = (ptr_ & static_assurance::mask());
		ptr_ = (ptr_ & ~static_assurance::mask()) | (new_flag & static_assurance::mask());
		return v;
	}

	bool operator<(const bitfield_ptr & rhs) const
	{
		return pointer() < rhs.pointer();
	}

	bool operator<=(const bitfield_ptr & rhs) const
	{
		return pointer() <= rhs.pointer();
	}

	bool operator==(const bitfield_ptr & rhs) const
	{
		return pointer() == rhs.pointer();
	}

	bool operator>=(const bitfield_ptr & rhs) const
	{
		return pointer() >= rhs.pointer();
	}

	bool operator>(const bitfield_ptr & rhs) const
	{
		return pointer() > rhs.pointer();
	}

	bool operator!=(const bitfield_ptr & rhs) const
	{
		return pointer() != rhs.pointer();
	}

	bool operator<(const T * rhs) const
	{
		return pointer() < rhs;
	}

	bool operator<=(const T * rhs) const
	{
		return pointer() <= rhs;
	}

	bool operator==(const T * rhs) const
	{
		return pointer() == rhs;
	}

	bool operator>=(const T * rhs) const
	{
		return pointer() >= rhs;
	}

	bool operator>(const T * rhs) const
	{
		return pointer() > rhs;
	}

	bool operator!=(const T * rhs) const
	{
		return pointer() != rhs;
	}

private:

	std::uintptr_t ptr_ = reinterpret_cast<std::uintptr_t>(nullptr);
};

} // namespace kotipp
