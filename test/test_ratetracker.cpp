#include <gtest/gtest.h>
#include <cmath>
#include "zenomt/RateTracker.hpp"

using namespace com::zenomt;

TEST(RateTrackerTest, BasicRate) {
	RateTracker tracker(1.0); // 1 second window
	
	tracker.update(100, 0.0);
	
	double rate = tracker.getRate(1.0);
	EXPECT_NEAR(rate, 100.0, 0.1);
}

TEST(RateTrackerTest, ZeroRate) {
	RateTracker tracker(1.0);
	
	double rate = tracker.getRate(0.0);
	EXPECT_EQ(rate, 0.0);
}

TEST(RateTrackerTest, WindowExpiry) {
	RateTracker tracker(1.0); // 1 second window
	
	tracker.update(100, 0.0);
	
	// After window expires, rate should be zero
	double rate = tracker.getRate(2.0);
	EXPECT_NEAR(rate, 0.0, 0.1);
}

TEST(RateTrackerTest, MultipleAdditions) {
	RateTracker tracker(1.0);
	
	tracker.update(50, 0.0);
	tracker.update(50, 0.5);
	
	double rate = tracker.getRate(1.0);
	EXPECT_NEAR(rate, 100.0, 0.1);
}

TEST(RateTrackerTest, PartialWindow) {
	RateTracker tracker(1.0);
	
	tracker.update(100, 0.0);
	
	// Halfway through window
	// Rate is calculated as count/windowPeriod, not count/elapsedTime
	// So 100 bytes in 0.5s of a 1.0s window = 100/1.0 = 100 bytes/second
	double rate = tracker.getRate(0.5);
	EXPECT_NEAR(rate, 100.0, 0.1);
}

TEST(RateTrackerTest, SlidingWindow) {
	RateTracker tracker(1.0);
	
	tracker.update(100, 0.0);
	tracker.update(100, 1.0);
	tracker.update(100, 2.0);
	
	// At time 2.0:
	// - First update (0.0) expired (delta >= twoWindows = 2.0)
	// - Second update (1.0) is in decay phase (delta = 1.0 >= windowPeriod)
	// - Third update (2.0) is current (delta = 0.0 < windowPeriod)
	// Rate includes both current count and previous rate decay
	// m_count = 100, m_previousRate = 100.0, delta = 0.0
	// Rate = (100/1.0) + (100.0 * 1.0) = 200.0
	double rate = tracker.getRate(2.0);
	EXPECT_NEAR(rate, 200.0, 0.1);
}

