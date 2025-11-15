#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>

#include "zenomt/RunLoops.hpp"
#include "zenomt/Performer.hpp"

using namespace com::zenomt;

// POSIX-specific tests for Performer
class PerformerPosixTest : public ::testing::Test {
protected:
	void SetUp() override {
		runLoop1 = std::make_shared<PreferredRunLoop>();
		performer1 = std::make_shared<Performer>(runLoop1.get());
		
		runLoop2 = std::make_shared<PreferredRunLoop>();
		performer2 = std::make_shared<Performer>(runLoop2.get());
	}

	void TearDown() override {
		if(performer1) performer1->close();
		if(performer2) performer2->close();
		if(runLoop1) runLoop1->clear();
		if(runLoop2) runLoop2->clear();
	}

	std::shared_ptr<RunLoop> runLoop1;
	std::shared_ptr<Performer> performer1;
	std::shared_ptr<RunLoop> runLoop2;
	std::shared_ptr<Performer> performer2;
};

TEST_F(PerformerPosixTest, CrossThreadPerform) {
	std::atomic<bool> executed{false};
	
	std::thread worker([this, &executed]() {
		// Perform task on runLoop1 from worker thread
		performer1->perform([&]() {
			executed = true;
			runLoop1->stop();
		});
	});

	std::thread runner([this]() {
		runLoop1->run(1.0);
	});

	worker.join();
	runner.join();
	
	EXPECT_TRUE(executed);
}

TEST_F(PerformerPosixTest, CrossThreadSyncPerform) {
	std::atomic<bool> executed{false};
	
	std::thread worker([this, &executed]() {
		performer1->perform([&]() {
			executed = true;
		}, true); // Wait for completion
	});

	std::thread runner([this]() {
		runLoop1->run(1.0);
	});

	worker.join();
	runner.join();
	
	EXPECT_TRUE(executed);
}

TEST_F(PerformerPosixTest, BidirectionalPerform) {
	std::atomic<int> count1{0};
	std::atomic<int> count2{0};
	
	std::thread runner1([this, &count1, &count2]() {
		performer2->perform([&]() {
			count1++;
			if(count1 >= 2) {
				runLoop1->stop();
			}
		});
		runLoop1->run(1.0);
	});
	
	std::thread runner2([this, &count1, &count2]() {
		performer1->perform([&]() {
			count2++;
			if(count2 >= 2) {
				runLoop2->stop();
			}
		});
		runLoop2->run(1.0);
	});

	runner1.join();
	runner2.join();
	
	EXPECT_GE(count1, 1);
	EXPECT_GE(count2, 1);
}

