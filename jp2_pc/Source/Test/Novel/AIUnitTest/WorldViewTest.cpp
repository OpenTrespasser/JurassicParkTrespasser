#include "common.hpp"
#include "Game/AI/WorldView.hpp"
#include "Game/AI/AIMain.hpp"
#include "Game/AI/AIInfo.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/Animal.hpp"
//#include "Test/AI/TestAnimal.hpp"

#include "gtest/gtest.h"

class WorldViewTest : public ::testing::Test
{
protected:

	virtual void SetUp() override
	{
		aisystem = std::make_unique<CAISystem>(nullptr);
		aisystem->sNow = 100.0f;
		gpaiSystem = aisystem.get();
		owner = std::make_unique<CAnimal>();
		view = std::make_unique<CWorldView>(owner.get());
	}

	virtual void TearDown() override
	{
		aisystem = nullptr;
		gpaiSystem = nullptr;
		view = nullptr;
		owner = nullptr;
	}

	void InsertNInfluences(size_t n, TSec lastSeen, TReal prio, bool markDiscarded = false)
	{
		for (size_t i = 0; i < n; i++)
			InsertInfluence(lastSeen, prio, markDiscarded);
	}
	
	void InsertInfluence(TSec lastSeen, TReal prio, bool markDiscarded = false)
	{
		//Instance is deleted in CInfluence destructor
		CInfluence influence(new CInstance());
		influence.sLastSeen = lastSeen;
		influence.rImportance = prio;
		influence.setFlags[eifIS_DISCARDABLE] = markDiscarded;
		view->inflInfluences.insert(influence);
	}

	CInfluenceList& GetList()
	{
		return view->inflInfluences;
	}
	
	static bool IsMarkedDiscardable(const CInfluence& influence)
	{
		return influence.setFlags[eifIS_DISCARDABLE];
	}
	
	std::unique_ptr<CAISystem> aisystem;
	std::unique_ptr<CAnimal> owner;
	std::unique_ptr<CWorldView> view;
};

//Nothing weird happens with an empty list
TEST_F(WorldViewTest, RemoveOnEmpty)
{
	ASSERT_TRUE(GetList().empty());
	view->RemoveSomeInfluences();
	EXPECT_TRUE(GetList().empty());
}

//Few old influences, marked but not discarded
TEST_F(WorldViewTest, MarkOldsAsDiscardable)
{
	constexpr size_t elems = 5;
	InsertNInfluences(elems, aisystem->sNow - 2.5f, 10.0f);

	ASSERT_EQ(GetList().size(), elems);
	ASSERT_TRUE(std::none_of(GetList().begin(), GetList().end(), &IsMarkedDiscardable));

	view->RemoveSomeInfluences();

	EXPECT_EQ(GetList().size(), elems);
	EXPECT_TRUE(std::all_of(GetList().begin(), GetList().end(), &IsMarkedDiscardable));
}

//Few new influences, not marked and not removed
TEST_F(WorldViewTest, NoMarkCurrentAsDiscardable)
{
	constexpr size_t elems = 5;
	InsertNInfluences(elems, aisystem->sNow - 0.5f, 10.0f);

	ASSERT_EQ(GetList().size(), elems);
	ASSERT_TRUE(std::none_of(GetList().begin(), GetList().end(), &CInfluence::IsDiscardable));

	view->RemoveSomeInfluences();

	EXPECT_EQ(GetList().size(), elems);
	EXPECT_TRUE(std::none_of(GetList().begin(), GetList().end(), &CInfluence::IsDiscardable));
}

//One zero prio influence, marked but not removed
TEST_F(WorldViewTest, MarkZeroPrioAsDiscardable)
{
	InsertInfluence(aisystem->sNow - 0.5f, 0.0f);

	ASSERT_FALSE(GetList().empty());
	ASSERT_FALSE(GetList().begin()->setFlags[eifIS_DISCARDABLE]);

	view->RemoveSomeInfluences();

	EXPECT_FALSE(GetList().empty());
	EXPECT_TRUE(GetList().begin()->setFlags[eifIS_DISCARDABLE]);
}

//Many current influences, lowest prio one is removed
TEST_F(WorldViewTest, MarkLowestOfFew)
{
	constexpr size_t elems = 5;
	InsertNInfluences(elems, aisystem->sNow - 0.5f, 0.05f);
	InsertInfluence(aisystem->sNow - 0.5f, 0.0f);

	ASSERT_EQ(GetList().size(), elems + 1);
	ASSERT_TRUE(std::none_of(GetList().begin(), GetList().end(), &CInfluence::IsDiscardable));

	view->RemoveSomeInfluences();

	EXPECT_EQ(GetList().size(), elems + 1);
	EXPECT_EQ(std::count_if(GetList().begin(), GetList().end(), &CInfluence::IsDiscardable), 1);
	EXPECT_EQ(std::find_if(GetList().begin(), GetList().end(), &CInfluence::IsDiscardable)->rImportance, 0.0f);
}

TEST_F(WorldViewTest, NoDiscardUnmarked)
{
	constexpr size_t elems = 20;
	InsertNInfluences(elems, aisystem->sNow - 0.5f, 10.0f, false);

	view->RemoveSomeInfluences();

	EXPECT_EQ(GetList().size(), elems);
}

TEST_F(WorldViewTest, DiscardMarkedAll)
{
	constexpr size_t elems = 20;
	InsertNInfluences(elems, aisystem->sNow - 0.5f, 10.0f, true);

	view->RemoveSomeInfluences();

	EXPECT_TRUE(GetList().empty());
}

TEST_F(WorldViewTest, DiscardMarkedMixed)
{
	constexpr size_t keep = 12;
	constexpr size_t discard = 12;
	InsertNInfluences(keep, aisystem->sNow - 0.5f, 10.0f, false);
	InsertNInfluences(discard, aisystem->sNow - 0.5f, 10.0f, true);

	view->RemoveSomeInfluences();

	EXPECT_EQ(GetList().size(), keep);
	EXPECT_TRUE(std::none_of(GetList().begin(), GetList().end(), &CInfluence::IsDiscardable));
}

//Many current influences, none discarded
TEST_F(WorldViewTest, NoDiscardCurrent)
{
	constexpr size_t tocreate = 20;
	InsertNInfluences(tocreate, aisystem->sNow - 0.5f, 10.0f);
	
	ASSERT_EQ(GetList().size(), tocreate);
	view->RemoveSomeInfluences();
	EXPECT_EQ(GetList().size(), tocreate);
}

//Many old influences, will be discarded
TEST_F(WorldViewTest, DiscardOldsMany)
{
	constexpr size_t olds = 20;
	constexpr size_t news = 5;
	InsertNInfluences(news, aisystem->sNow - 0.5f, 10.0f);
	InsertNInfluences(olds, aisystem->sNow - 2.5f, 10.0f);

	ASSERT_EQ(GetList().size(), olds + news);
	view->RemoveSomeInfluences();
	EXPECT_EQ(GetList().size(), news);
}

//Many old influences and a current low-prio one, will be discarded
TEST_F(WorldViewTest, DiscardOldsAndLowestPrioCurrent)
{
	constexpr size_t olds = 20;
	constexpr size_t news = 5;
	InsertNInfluences(news, aisystem->sNow - 0.5f, 10.0f);
	InsertNInfluences(olds, aisystem->sNow - 2.5f, 10.0f);
	InsertInfluence(aisystem->sNow - 0.5f, 0.0f);

	ASSERT_EQ(GetList().size(), olds + news + 1);
	view->RemoveSomeInfluences();
	EXPECT_EQ(GetList().size(), news);
}

//Few influences, one with zero prio is to be removed
TEST_F(WorldViewTest, DiscardFewZeroPrio)
{
	InsertNInfluences(15, aisystem->sNow - 2.5f, 10.0f);
	InsertInfluence(aisystem->sNow - 0.5f, 0.0f);

	view->RemoveSomeInfluences();

	EXPECT_TRUE(GetList().empty());
}

//Some old influences to be removed, one with some prio is not to be removed
TEST_F(WorldViewTest, NoDiscardFewWithPrio)
{
	InsertNInfluences(15, aisystem->sNow - 2.5f, 10.0f);
	InsertInfluence(aisystem->sNow - 0.5f, 1.0f);

	view->RemoveSomeInfluences();

	EXPECT_EQ(GetList().size(), 1);
}

//Many old influences to be removed, new influence but with lowest prio is also removed
TEST_F(WorldViewTest, DiscardManyWithPrio)
{
	InsertNInfluences(40, aisystem->sNow - 2.5f, 10.0f);
	InsertInfluence(aisystem->sNow - 0.5f, 1.0f);

	view->RemoveSomeInfluences();

	EXPECT_TRUE(GetList().empty());
}

//Very few influences, none are removed, including one with zero prio
TEST_F(WorldViewTest, NoDiscardVeryFewZeroPrio)
{
	constexpr size_t elems = 5;
	InsertNInfluences(elems, aisystem->sNow - 2.5f, 10.0f);
	InsertInfluence(aisystem->sNow - 0.5f, 0.0f);

	view->RemoveSomeInfluences();

	EXPECT_EQ(GetList().size(), elems + 1);
}
