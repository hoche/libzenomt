#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>

#include "zenomt/RunLoops.hpp"
#include "zenomt/Performer.hpp"

using namespace com::zenomt;

class PerformerTest : public ::testing::Test {
protected:
	void SetUp() override {
		runLoop = std::make_shared<PreferredRunLoop>();
		performer = std::make_shared<Performer>(runLoop.get());
	}

	void TearDown() override {
		if(performer) {
			performer->close();
		}
		if(runLoop) {
			runLoop->clear();
		}
	}

	std::shared_ptr<RunLoop> runLoop;
	std::shared_ptr<Performer> performer;
};

TEST_F(PerformerTest, AsyncPerform) {
	std::atomic<bool> taskExecuted{false};
	
	performer->perform([&]() {
		taskExecuted = true;
		runLoop->stop();
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_TRUE(taskExecuted);
}

TEST_F(PerformerTest, SyncPerform) {
	std::atomic<bool> taskExecuted{false};
	
	std::thread worker([this, &taskExecuted]() {
		performer->perform([&]() {
			taskExecuted = true;
		}, true); // Wait for completion
	});

	std::thread runner([this]() {
		runLoop->run(1.0);
	});

	worker.join();
	runner.join();
	
	EXPECT_TRUE(taskExecuted);
}

TEST_F(PerformerTest, MultipleAsyncPerforms) {
	std::atomic<int> count{0};
	
	for(int i = 0; i < 5; i++) {
		performer->perform([&]() {
			count++;
			if(count == 5) {
				runLoop->stop();
			}
		});
	}

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_EQ(count, 5);
}

TEST_F(PerformerTest, PerformFromRunLoopThread) {
	std::atomic<bool> executed{false};
	
	runLoop->scheduleRel(0.1, 0, true)->action = Timer::makeAction([&](Time now) {
		// This is on the RunLoop thread
		performer->perform([&]() {
			executed = true;
		}, true); // Should execute immediately
		runLoop->stop();
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_TRUE(executed);
}

TEST_F(PerformerTest, PerformAfterClose) {
	bool executed = false;
	
	performer->close();
	performer->perform([&]() {
		executed = true;
	});

	// Task should not execute after close
	EXPECT_FALSE(executed);
}

TEST_F(PerformerTest, NestedPerform) {
	std::atomic<int> outerCount{0};
	std::atomic<int> innerCount{0};
	
	performer->perform([&]() {
		outerCount++;
		performer->perform([&]() {
			innerCount++;
			if(innerCount == 1) {
				runLoop->stop();
			}
		});
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_EQ(outerCount, 1);
	EXPECT_EQ(innerCount, 1);
}

TEST_F(PerformerTest, PerformOrder) {
	std::vector<int> order;
	
	performer->perform([&]() {
		order.push_back(1);
		if(order.size() == 3) {
			runLoop->stop();
		}
	});
	
	performer->perform([&]() {
		order.push_back(2);
		if(order.size() == 3) {
			runLoop->stop();
		}
	});
	
	performer->perform([&]() {
		order.push_back(3);
		if(order.size() == 3) {
			runLoop->stop();
		}
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	// Tasks should execute in order
	EXPECT_EQ(order.size(), 3);
	EXPECT_EQ(order[0], 1);
	EXPECT_EQ(order[1], 2);
	EXPECT_EQ(order[2], 3);
}

