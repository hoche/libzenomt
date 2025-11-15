#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "zenomt/RunLoops.hpp"
#include "zenomt/Timer.hpp"

using namespace com::zenomt;

class RunLoopTest : public ::testing::Test {
protected:
	void SetUp() override {
		runLoop = std::make_shared<PreferredRunLoop>();
	}

	void TearDown() override {
		if(runLoop) {
			runLoop->clear();
		}
	}

	std::shared_ptr<RunLoop> runLoop;
};

TEST_F(RunLoopTest, BasicTimer) {
	bool timerFired = false;
	
	auto timer = runLoop->scheduleRel(0.1, 0, true);
	timer->action = Timer::makeAction([&](Time now) {
		timerFired = true;
		runLoop->stop();
	});

	std::thread t([this]() {
		runLoop->run(1.0); // Max 1 second
	});

	t.join();
	
	EXPECT_TRUE(timerFired);
}

TEST_F(RunLoopTest, RecurringTimer) {
	int fireCount = 0;
	
	auto timer = runLoop->scheduleRel(0.05, 0.05, true);
	timer->action = Timer::makeAction([&](Time now) {
		fireCount++;
		if(fireCount >= 3) {
			runLoop->stop();
		}
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_GE(fireCount, 3);
}

TEST_F(RunLoopTest, TimerCancellation) {
	bool timerFired = false;
	
	auto timer = runLoop->scheduleRel(0.1, 0, true);
	timer->action = Timer::makeAction([&](Time now) {
		timerFired = true;
	});
	
	timer->cancel();

	std::thread t([this]() {
		runLoop->run(0.5);
	});

	t.join();
	
	EXPECT_FALSE(timerFired);
}

TEST_F(RunLoopTest, DoLater) {
	bool taskExecuted = false;
	
	runLoop->doLater([&]() {
		taskExecuted = true;
		runLoop->stop();
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_TRUE(taskExecuted);
}

TEST_F(RunLoopTest, OnEveryCycle) {
	int cycleCount = 0;
	
	// Need a timer to keep the loop running and cycling
	auto timer = runLoop->scheduleRel(0.01, 0.01, true);
	timer->action = Timer::makeAction([&](Time now) {
		// Timer keeps loop active
	});
	
	runLoop->onEveryCycle = [&]() {
		cycleCount++;
		if(cycleCount >= 5) {
			runLoop->stop();
		}
	};

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_GE(cycleCount, 5);
}

TEST_F(RunLoopTest, TimeFunctions) {
	Time t1 = runLoop->getCurrentTime();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	Time t2 = runLoop->getCurrentTimeNoCache();
	
	EXPECT_GT(t2, t1);
	EXPECT_GT(t2 - t1, 0.005); // At least 5ms
}

TEST_F(RunLoopTest, StopFlag) {
	bool timerFired = false;
	bool stopCalled = false;
	
	auto timer = runLoop->scheduleRel(0.1, 0, true);
	timer->action = Timer::makeAction([&](Time now) {
		timerFired = true;
		runLoop->stop(); // Stop from within timer callback
		stopCalled = true;
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	// Timer should fire and then stop the loop
	EXPECT_TRUE(timerFired);
	EXPECT_TRUE(stopCalled);
	
	// Verify loop actually stopped (timer fired but loop exited)
	// The fact that t.join() returned means the loop stopped
}

TEST_F(RunLoopTest, IsRunningInThisThread) {
	bool isRunning = false;
	
	std::thread t([this, &isRunning]() {
		isRunning = runLoop->isRunningInThisThread();
		runLoop->run(0.1);
	});

	t.join();
	
	// Should be false from outside thread
	EXPECT_FALSE(runLoop->isRunningInThisThread());
}

