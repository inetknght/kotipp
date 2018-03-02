#include <gtest/gtest.h>

#include "../bitfield_ptr.hpp"

#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace kotipp;

struct align_to_1_size_1
{
	std::uint8_t v_ = 0;
};
using align_to_1_size_1_ptr = bitfield_ptr<align_to_1_size_1>;
using pa1s1 = align_to_1_size_1_ptr;
using a1s1 = align_to_1_size_1;

struct alignas(2) align_to_2_size_1 
: public align_to_1_size_1 {};
using align_to_2_size_1_ptr = bitfield_ptr<align_to_2_size_1>;
using pa2s1 = align_to_2_size_1_ptr;
using a2s1 = align_to_2_size_1;

struct alignas(4) align_to_4_size_1 
: public align_to_1_size_1 {};
using align_to_4_size_1_ptr = bitfield_ptr<align_to_4_size_1>;
using pa4s1 = align_to_4_size_1_ptr;
using a4s1 = align_to_4_size_1;

struct alignas(8) align_to_8_size_1 
: public align_to_1_size_1 {};
using align_to_8_size_1_ptr = bitfield_ptr<align_to_8_size_1>;
using pa8s1 = align_to_8_size_1_ptr;
using a8s1 = align_to_8_size_1;

struct alignas(16) align_to_16_size_1 
: public align_to_1_size_1 {};
using align_to_16_size_1_ptr = bitfield_ptr<align_to_16_size_1>;
using pa16s1 = align_to_16_size_1_ptr;
using a16s1 = align_to_16_size_1;

struct align_to_2_size_2
{
	std::uint16_t v_ = 0;
};
using align_to_2_size_2_ptr = bitfield_ptr<align_to_2_size_2>;
using pa2s2 = align_to_2_size_2_ptr;
using a2s2 = align_to_2_size_2;

struct alignas(4) align_to_4_size_2
: public align_to_2_size_2 {};
using align_to_4_size_2_ptr = bitfield_ptr<align_to_4_size_2>;
using pa4s2 = align_to_4_size_2_ptr;
using a4s2 = align_to_4_size_2;

struct alignas(8) align_to_8_size_2
: public align_to_2_size_2 {};
using align_to_8_size_2_ptr = bitfield_ptr<align_to_8_size_2>;
using pa8s2 = align_to_8_size_2_ptr;
using a8s2 = align_to_8_size_2;

struct alignas(16) align_to_16_size_2
: public align_to_2_size_2 {};
using align_to_16_size_2_ptr = bitfield_ptr<align_to_16_size_2>;
using pa16s2 = align_to_16_size_2_ptr;
using a16s2 = align_to_16_size_2;

struct align_to_4_size_4
{
	std::uint32_t v_ = 0;
};
using align_to_4_size_4_ptr = bitfield_ptr<align_to_4_size_4>;
using pa4s4 = align_to_4_size_4_ptr;
using a4s4 = align_to_4_size_4;

struct alignas(8) align_to_8_size_4
: public align_to_4_size_4 {};
using align_to_8_size_4_ptr = bitfield_ptr<align_to_8_size_4>;
using pa8s4 = align_to_8_size_4_ptr;
using a8s4 = align_to_8_size_4;

struct alignas(16) align_to_16_size_4
: public align_to_4_size_4 {};
using align_to_16_size_4_ptr = bitfield_ptr<align_to_16_size_4>;
using pa16s4 = align_to_16_size_4_ptr;
using a16s4 = align_to_16_size_4;

struct align_to_8_size_8
{
	std::uint64_t v_ = 0;
};
using align_to_8_size_8_ptr = bitfield_ptr<align_to_8_size_8>;
using pa8s8 = align_to_8_size_8_ptr;
using a8s8 = align_to_8_size_8;

struct alignas(16) align_to_16_size_8
: public align_to_8_size_8 {};
using align_to_16_size_8_ptr = bitfield_ptr<align_to_16_size_8>;
using pa16s8 = align_to_16_size_8_ptr;
using a16s8 = align_to_16_size_8;

struct align_to_16_size_16
{
	__uint128_t v_ = 0;
};
using align_to_16_size_16_ptr = bitfield_ptr<align_to_16_size_16>;
using pa16s16 = align_to_16_size_16_ptr;
using a16s16 = align_to_16_size_16;

TEST(bitfield_ptr_tests, alignments)
{
	EXPECT_EQ(alignof(pa1s1), alignof(a1s1*));
	EXPECT_EQ(alignof(pa2s1), alignof(a2s1*));
	EXPECT_EQ(alignof(pa4s1), alignof(a4s1*));
	EXPECT_EQ(alignof(pa8s1), alignof(a8s1*));
	EXPECT_EQ(alignof(pa16s1), alignof(a16s1*));
	EXPECT_EQ(alignof(pa2s2), alignof(a2s2*));
	EXPECT_EQ(alignof(pa4s2), alignof(a4s2*));
	EXPECT_EQ(alignof(pa8s2), alignof(a8s2*));
	EXPECT_EQ(alignof(pa16s2), alignof(a16s2*));
	EXPECT_EQ(alignof(pa4s4), alignof(a4s4*));
	EXPECT_EQ(alignof(pa8s4), alignof(a8s4*));
	EXPECT_EQ(alignof(pa16s4), alignof(a16s4*));
	EXPECT_EQ(alignof(pa8s8), alignof(a8s8*));
	EXPECT_EQ(alignof(pa16s8), alignof(a16s8*));
	EXPECT_EQ(alignof(pa16s16), alignof(a16s16*));
}

TEST(bitfield_ptr_tests, sizes)
{
	EXPECT_EQ(sizeof(pa1s1), sizeof(a1s1*));
	EXPECT_EQ(sizeof(pa2s1), sizeof(a2s1*));
	EXPECT_EQ(sizeof(pa4s1), sizeof(a4s1*));
	EXPECT_EQ(sizeof(pa8s1), sizeof(a8s1*));
	EXPECT_EQ(sizeof(pa16s1), sizeof(a16s1*));
	EXPECT_EQ(sizeof(pa2s2), sizeof(a2s2*));
	EXPECT_EQ(sizeof(pa4s2), sizeof(a4s2*));
	EXPECT_EQ(sizeof(pa8s2), sizeof(a8s2*));
	EXPECT_EQ(sizeof(pa16s2), sizeof(a16s2*));
	EXPECT_EQ(sizeof(pa4s4), sizeof(a4s4*));
	EXPECT_EQ(sizeof(pa8s4), sizeof(a8s4*));
	EXPECT_EQ(sizeof(pa16s4), sizeof(a16s4*));
	EXPECT_EQ(sizeof(pa8s8), sizeof(a8s8*));
	EXPECT_EQ(sizeof(pa16s8), sizeof(a16s8*));
	EXPECT_EQ(sizeof(pa16s16), sizeof(a16s16*));
}

TEST(bitfield_ptr_tests, alignment_bits)
{
	EXPECT_EQ(pa1s1  ::alignment_bits_, 0);
	EXPECT_EQ(pa2s1  ::alignment_bits_, 1);
	EXPECT_EQ(pa4s1  ::alignment_bits_, 2);
	EXPECT_EQ(pa8s1  ::alignment_bits_, 3);
	EXPECT_EQ(pa16s1 ::alignment_bits_, 4);
	EXPECT_EQ(pa2s2  ::alignment_bits_, 1);
	EXPECT_EQ(pa4s2  ::alignment_bits_, 2);
	EXPECT_EQ(pa8s2  ::alignment_bits_, 3);
	EXPECT_EQ(pa16s2 ::alignment_bits_, 4);
	EXPECT_EQ(pa4s4  ::alignment_bits_, 2);
	EXPECT_EQ(pa8s4  ::alignment_bits_, 3);
	EXPECT_EQ(pa16s4 ::alignment_bits_, 4);
	EXPECT_EQ(pa8s8  ::alignment_bits_, 3);
	EXPECT_EQ(pa16s8 ::alignment_bits_, 4);
	EXPECT_EQ(pa16s16::alignment_bits_, 4);
}

TEST(bitfield_ptr_tests, mask)
{
	EXPECT_EQ(pa1s1  ::mask_, 0);
	EXPECT_EQ(pa2s1  ::mask_, 1);
	EXPECT_EQ(pa4s1  ::mask_, 3);
	EXPECT_EQ(pa8s1  ::mask_, 7);
	EXPECT_EQ(pa16s1 ::mask_, 15);
	EXPECT_EQ(pa2s2  ::mask_, 1);
	EXPECT_EQ(pa4s2  ::mask_, 3);
	EXPECT_EQ(pa8s2  ::mask_, 7);
	EXPECT_EQ(pa16s2 ::mask_, 15);
	EXPECT_EQ(pa4s4  ::mask_, 3);
	EXPECT_EQ(pa8s4  ::mask_, 7);
	EXPECT_EQ(pa16s4 ::mask_, 15);
	EXPECT_EQ(pa8s8  ::mask_, 7);
	EXPECT_EQ(pa16s8 ::mask_, 15);
	EXPECT_EQ(pa16s16::mask_, 15);
}

TEST(bitfield_ptr_tests, mask_count)
{
	EXPECT_EQ(pa1s1  ::mask_count_, 0);
	EXPECT_EQ(pa2s1  ::mask_count_, 1);
	EXPECT_EQ(pa4s1  ::mask_count_, 2);
	EXPECT_EQ(pa8s1  ::mask_count_, 3);
	EXPECT_EQ(pa16s1 ::mask_count_, 4);
	EXPECT_EQ(pa2s2  ::mask_count_, 1);
	EXPECT_EQ(pa4s2  ::mask_count_, 2);
	EXPECT_EQ(pa8s2  ::mask_count_, 3);
	EXPECT_EQ(pa16s2 ::mask_count_, 4);
	EXPECT_EQ(pa4s4  ::mask_count_, 2);
	EXPECT_EQ(pa8s4  ::mask_count_, 3);
	EXPECT_EQ(pa16s4 ::mask_count_, 4);
	EXPECT_EQ(pa8s8  ::mask_count_, 3);
	EXPECT_EQ(pa16s8 ::mask_count_, 4);
	EXPECT_EQ(pa16s16::mask_count_, 4);
}

TEST(bitfield_ptr_tests, construct_copy_move_destruct)
{
	{
		pa1s1 f;
		ASSERT_NO_THROW(pa1s1{});

		{
			pa1s1 a;
			pa1s1 b{std::move(a)};
			a = std::move(b);
		}
	}

	{
		pa16s16 f;
		ASSERT_NO_THROW(pa16s16{});

		{
			pa16s16 a;
			pa16s16 b{std::move(a)};
			a = std::move(b);
		}
		{
			auto * t = new align_to_16_size_16();
			EXPECT_NO_THROW(f.assign(t));
			EXPECT_EQ(&t->v_, &f->v_);

			EXPECT_EQ(f.pointer(), t);
			EXPECT_EQ(0, f.assign_flag(127));
			EXPECT_EQ(f.pointer(), t);
			EXPECT_EQ(15, f.flag());
			EXPECT_EQ(f.pointer(), t);
			EXPECT_EQ(15, f.assign_flag(4));
			EXPECT_EQ(f.pointer(), t);
			EXPECT_EQ(4, f.flag());
			EXPECT_EQ(f.take(), t);
			EXPECT_EQ(f.pointer(), nullptr);
			EXPECT_EQ(4, f.flag());
			EXPECT_NO_THROW(f.clear());
			EXPECT_EQ(0, f.flag());
			EXPECT_EQ(f.pointer(), nullptr);
		}
	}
}
