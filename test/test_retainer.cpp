#include <gtest/gtest.h>
#include "zenomt/Object.hpp"
#include "zenomt/Retainer.hpp"

using namespace com::zenomt;

namespace {
class Test : public Object {
public:
	long getRefcount() const {
		return m_refcount;
	}
};

class SubTest : public Test {};
}

TEST(RetainerTest, BasicOperations) {
	auto t1 = claim_ref(new Test());
	EXPECT_TRUE(t1);
	EXPECT_EQ(t1->getRefcount(), 1);
	
	{
		auto t2 = t1;
		auto t3(t1);
		EXPECT_EQ(t1->getRefcount(), 3);
		EXPECT_EQ(t2->getRefcount(), 3);
		EXPECT_EQ(t3->getRefcount(), 3);
		EXPECT_EQ(t1, t2);
		EXPECT_EQ(t2, t3);
	}
	
	EXPECT_EQ(t1->getRefcount(), 1);
}

TEST(RetainerTest, Reset) {
	auto t1 = claim_ref(new Test());
	auto t2 = t1;
	
	EXPECT_EQ(t1->getRefcount(), 2);
	t2.reset();
	EXPECT_TRUE(t2.empty());
	EXPECT_FALSE(t2);
	EXPECT_EQ(t1->getRefcount(), 1);
}

TEST(RetainerTest, Comparison) {
	auto t1 = claim_ref(new Test());
	auto t2 = t1;
	auto t3 = claim_ref(new Test());
	
	EXPECT_EQ(t1, t2);
	EXPECT_NE(t1, t3);
	EXPECT_LE(t1, t2);
	EXPECT_GE(t1, t2);
	
	Retainer<Test> empty;
	EXPECT_NE(t1, empty);
	EXPECT_LT(empty, t1);
	EXPECT_GT(t1, empty);
}

TEST(RetainerTest, Swap) {
	auto t1 = claim_ref(new Test());
	Retainer<Test> t2;
	
	std::swap(t1, t2);
	EXPECT_TRUE(t1.empty());
	EXPECT_FALSE(t2.empty());
	EXPECT_EQ(t2->getRefcount(), 1);
	
	std::swap(t1, t2);
	EXPECT_FALSE(t1.empty());
	EXPECT_TRUE(t2.empty());
}

TEST(RetainerTest, MoveAssignment) {
	auto t1 = claim_ref(new Test());
	Retainer<Test> t2;
	
	t2 = std::move(t1);
	EXPECT_TRUE(t1.empty());
	EXPECT_FALSE(t2.empty());
	EXPECT_EQ(t2->getRefcount(), 1);
}

TEST(RetainerTest, Inheritance) {
	auto s1 = claim_ref(new SubTest());
	auto t1 = claim_ref(new Test());
	
	Retainer<Test> t2 = s1;
	EXPECT_EQ(s1->getRefcount(), 2);
	EXPECT_EQ(t2->getRefcount(), 2);
	EXPECT_EQ(s1, t2);
}

TEST(RetainerTest, RetainRef) {
	auto t1 = claim_ref(new Test());
	auto t2 = retain_ref(t1.get());
	
	EXPECT_EQ(t1->getRefcount(), 2);
	EXPECT_EQ(t2->getRefcount(), 2);
	EXPECT_EQ(t1, t2);
}

TEST(RetainerTest, NullAssignment) {
	auto t1 = claim_ref(new Test());
	t1 = nullptr;
	
	EXPECT_TRUE(t1.empty());
	EXPECT_FALSE(t1);
}

TEST(RetainerTest, ReturnEmpty) {
	auto empty = []() -> Retainer<Test> {
		return nullptr;
	}();
	
	EXPECT_TRUE(empty.empty());
}

