#include "common.hpp"
#include "Game/AI/Influence.hpp"
#include "Lib/EntityDBase/Instance.hpp"

#include "gtest/gtest.h"


TEST(CInfluence, ComparisonSetFlagsNoInfluence)
{
	//Comparison is based on the pins_target pointer value
	//Use specific pointers
	CInfluence a(reinterpret_cast<CInstance*>(60));
	CInfluence b(reinterpret_cast<CInstance*>(80));

	const std::less<> less;
	ASSERT_TRUE(less(a, b));
	ASSERT_FALSE(less(b, a));

	//Test that the discardable flag has no influence on comparison
	for (int i=0; i < 4; i++)
	{
		a.setFlags[eifIS_DISCARDABLE] = static_cast<bool>(i & (1 << 0)); //First bit
		b.setFlags[eifIS_DISCARDABLE] = static_cast<bool>(i & (1 << 1)); //Second bit

		EXPECT_TRUE(less(a, b));
		EXPECT_FALSE(less(b, a));
	}
}

TEST(CInfluence, SetFlagOnConst)
{
	const CInfluence influence(nullptr);
	ASSERT_FALSE(influence.setFlags[eifIS_DISCARDABLE]);

	//Test that we can set the flag despite constness
	influence.setFlags[eifIS_DISCARDABLE] = true;
	EXPECT_TRUE(influence.setFlags[eifIS_DISCARDABLE]);
}