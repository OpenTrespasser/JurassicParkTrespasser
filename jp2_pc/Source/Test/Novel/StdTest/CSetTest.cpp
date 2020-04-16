#include "Lib/Std/UTypes.hpp"
#include "Lib/Std/UDefs.hpp"
#include "Lib/Std/Set.hpp"

#include "gtest/gtest.h"

#include <type_traits>

enum TestEnum
{
	Start,
	Foo,
	Bar,
	End
};

//Check general prerequisites to make copy-assignment operator implementation work as expected
TEST(CSetHelper, SafetyChecks)
{
	using CSet_t = CSet<TestEnum>;
	using CSetHC_t = CSet_t::CSetHelperConst;

	//Same size, CSetHelperConst has no new members
	static_assert(sizeof(CSet_t::CSetHelper) == sizeof(CSetHC_t));

	//We can unofficially skip destruction in the copy-assignment operator (and in general)
	static_assert(std::is_trivially_destructible_v<CSetHC_t>);

	//No virtual methods
	static_assert(!std::is_polymorphic_v<CSetHC_t>);
}

TEST(CSetHelper, Copy_Assignment_Same_Bitfield)
{
	CSet<TestEnum> bitfield = Set(TestEnum::Bar);

	bitfield[TestEnum::Foo] = bitfield[TestEnum::Bar];
	
	EXPECT_TRUE(bitfield[TestEnum::Foo]);
}

TEST(CSetHelper, Copy_Assignment_Different_Bitfield)
{
	CSet<TestEnum> first = Set(TestEnum::Start);
	CSet<TestEnum> second = Set(TestEnum::Bar) + TestEnum::End;

	first[TestEnum::Foo] = second[TestEnum::Bar];
	
	EXPECT_TRUE(first[TestEnum::Start]); //From init, not altered
	EXPECT_TRUE(first[TestEnum::Foo]);   //Was assigned
	EXPECT_FALSE(first[TestEnum::Bar]);  //Was not copied
	EXPECT_FALSE(first[TestEnum::End]);  //Was not copied
}
