#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "zenomt/RunLoops.hpp"
#include "zenomt/Timer.hpp"

using namespace com::zenomt;

class TimerTest : public ::testing::Test {
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

TEST_F(TimerTest, ScheduleAbsolute) {
	bool fired = false;
	Time now = runLoop->getCurrentTime();
	
	auto timer = runLoop->schedule(now + 0.1, 0, true);
	timer->action = Timer::makeAction([&](Time fireTime) {
		fired = true;
		runLoop->stop();
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_TRUE(fired);
}

TEST_F(TimerTest, ScheduleRelative) {
	bool fired = false;
	
	auto timer = runLoop->scheduleRel(0.1, 0, true);
	timer->action = Timer::makeAction([&](Time fireTime) {
		fired = true;
		runLoop->stop();
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_TRUE(fired);
}

TEST_F(TimerTest, RecurringTimer) {
	int count = 0;
	
	auto timer = runLoop->scheduleRel(0.05, 0.05, true);
	timer->action = Timer::makeAction([&](Time fireTime) {
		count++;
		if(count >= 5) {
			runLoop->stop();
		}
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_GE(count, 5);
}

TEST_F(TimerTest, RecurringTimerNoCatchup) {
	int count = 0;
	Time lastFire = 0;
	
	auto timer = runLoop->scheduleRel(0.05, 0.05, false);
	timer->action = Timer::makeAction([&](Time fireTime) {
		if(lastFire > 0) {
			// Should be approximately 0.05 apart, not catching up
			Time diff = fireTime - lastFire;
			EXPECT_GE(diff, 0.04); // Allow some tolerance
		}
		lastFire = fireTime;
		count++;
		if(count >= 3) {
			runLoop->stop();
		}
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_GE(count, 3);
}

TEST_F(TimerTest, CancelTimer) {
	bool fired = false;
	
	auto timer = runLoop->scheduleRel(0.1, 0, true);
	timer->action = Timer::makeAction([&](Time fireTime) {
		fired = true;
	});
	
	timer->cancel();
	EXPECT_TRUE(timer->isCanceled());

	std::thread t([this]() {
		runLoop->run(0.5);
	});

	t.join();
	
	EXPECT_FALSE(fired);
}

TEST_F(TimerTest, RescheduleTimer) {
	int count = 0;
	
	auto timer = runLoop->scheduleRel(0.1, 0, true);
	timer->action = Timer::makeAction([&](Time fireTime) {
		count++;
		if(count == 1) {
			// Reschedule for later
			timer->setNextFireTime(runLoop->getCurrentTime() + 0.1);
		} else {
			runLoop->stop();
		}
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_EQ(count, 2);
}

TEST_F(TimerTest, SetRecurInterval) {
	int count = 0;
	
	auto timer = runLoop->scheduleRel(0.05, 0.1, true);
	timer->action = Timer::makeAction([&](Time fireTime) {
		count++;
		if(count == 1) {
			// Change recurrence interval
			timer->setRecurInterval(0.05);
		}
		if(count >= 3) {
			runLoop->stop();
		}
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_GE(count, 3);
}

TEST_F(TimerTest, MultipleTimers) {
	int timer1Count = 0;
	int timer2Count = 0;
	
	auto timer1 = runLoop->scheduleRel(0.05, 0.05, true);
	timer1->action = Timer::makeAction([&](Time fireTime) {
		timer1Count++;
	});
	
	auto timer2 = runLoop->scheduleRel(0.1, 0.1, true);
	timer2->action = Timer::makeAction([&](Time fireTime) {
		timer2Count++;
		if(timer2Count >= 2) {
			runLoop->stop();
		}
	});

	std::thread t([this]() {
		runLoop->run(1.0);
	});

	t.join();
	
	EXPECT_GE(timer1Count, 2); // Should fire twice as often
	EXPECT_GE(timer2Count, 2);
}

