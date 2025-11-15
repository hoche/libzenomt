#include <gtest/gtest.h>
#include "zenomt/Object.hpp"
#include "zenomt/Retainer.hpp"

using namespace com::zenomt;

namespace {
class TestObject : public Object {
public:
	// Simple test object for Retainer tests
};

class SubTestObject : public TestObject {};
}

TEST(RetainerTest, BasicOperations) {
	auto t1 = claim_ref(new TestObject());
	EXPECT_TRUE(t1);
	EXPECT_FALSE(t1.empty());
	
	{
		auto t2 = t1;
		auto t3(t1);
		EXPECT_EQ(t1, t2);
		EXPECT_EQ(t2, t3);
		EXPECT_FALSE(t1.empty());
		EXPECT_FALSE(t2.empty());
		EXPECT_FALSE(t3.empty());
	}
	
	EXPECT_FALSE(t1.empty());
}

TEST(RetainerTest, Reset) {
	auto t1 = claim_ref(new TestObject());
	auto t2 = t1;
	
	EXPECT_FALSE(t1.empty());
	EXPECT_FALSE(t2.empty());
	t2.reset();
	EXPECT_TRUE(t2.empty());
	EXPECT_FALSE(t2);
	EXPECT_FALSE(t1.empty());
}

TEST(RetainerTest, Comparison) {
	auto t1 = claim_ref(new TestObject());
	auto t2 = t1;
	auto t3 = claim_ref(new TestObject());
	
	EXPECT_EQ(t1, t2);
	EXPECT_NE(t1, t3);
	EXPECT_LE(t1, t2);
	EXPECT_GE(t1, t2);
	
	Retainer<TestObject> empty;
	EXPECT_NE(t1, empty);
	EXPECT_LT(empty, t1);
	EXPECT_GT(t1, empty);
}

TEST(RetainerTest, Swap) {
	auto t1 = claim_ref(new TestObject());
	Retainer<TestObject> t2;
	
	std::swap(t1, t2);
	EXPECT_TRUE(t1.empty());
	EXPECT_FALSE(t2.empty());
	
	std::swap(t1, t2);
	EXPECT_FALSE(t1.empty());
	EXPECT_TRUE(t2.empty());
}

TEST(RetainerTest, MoveAssignment) {
	auto t1 = claim_ref(new TestObject());
	Retainer<TestObject> t2;
	
	t2 = std::move(t1);
	EXPECT_TRUE(t1.empty());
	EXPECT_FALSE(t2.empty());
}

TEST(RetainerTest, Inheritance) {
	auto s1 = claim_ref(new SubTestObject());
	auto t1 = claim_ref(new TestObject());
	
	Retainer<TestObject> t2 = s1;
	EXPECT_FALSE(s1.empty());
	EXPECT_FALSE(t2.empty());
	EXPECT_EQ(s1.get(), t2.get()); // Same object
}

TEST(RetainerTest, RetainRef) {
	auto t1 = claim_ref(new TestObject());
	auto t2 = retain_ref(t1.get());
	
	EXPECT_FALSE(t1.empty());
	EXPECT_FALSE(t2.empty());
	EXPECT_EQ(t1, t2);
}

TEST(RetainerTest, NullAssignment) {
	auto t1 = claim_ref(new TestObject());
	t1 = nullptr;
	
	EXPECT_TRUE(t1.empty());
	EXPECT_FALSE(t1);
}

TEST(RetainerTest, ReturnEmpty) {
	auto empty = []() -> Retainer<TestObject> {
		return nullptr;
	}();
	
	EXPECT_TRUE(empty.empty());
}

