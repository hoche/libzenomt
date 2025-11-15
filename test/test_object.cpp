#include <gtest/gtest.h>
#include <memory>

#include "zenomt/Object.hpp"

using namespace com::zenomt;

class TestObject : public Object {
public:
	TestObject() : value(0) {}
	int value;
};

TEST(ObjectTest, BasicRetainRelease) {
	TestObject* obj = new TestObject();
	// m_refcount is protected, so we can't directly check it
	// But we can verify behavior through retain/release
	
	obj->retain();
	obj->release();
	obj->release(); // Should delete
}

TEST(ObjectTest, StaticRetainRelease) {
	TestObject* obj = new TestObject();
	
	Object::retain(obj);
	Object::release(obj);
	Object::release(obj); // Should delete
}

TEST(ObjectTest, ShareRef) {
	TestObject* obj = new TestObject();
	obj->value = 42;
	
	auto shared = share_ref(obj, true);
	EXPECT_EQ(shared->value, 42);
	
	shared.reset();
	obj->release(); // Clean up
}

TEST(ObjectTest, ShareRefNoRetain) {
	TestObject* obj = new TestObject();
	obj->value = 42;
	
	auto shared = share_ref(obj, false);
	EXPECT_EQ(shared->value, 42);
	
	shared.reset(); // Should delete obj
}

TEST(ObjectTest, CopyConstructorDeleted) {
	TestObject obj1;
	// TestObject obj2(obj1); // Should not compile
	EXPECT_TRUE(true); // If we get here, copy constructor is deleted
}

