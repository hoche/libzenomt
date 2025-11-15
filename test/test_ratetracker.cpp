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
	double rate = tracker.getRate(0.5);
	EXPECT_NEAR(rate, 200.0, 1.0); // Should be higher since less time elapsed
}

TEST(RateTrackerTest, SlidingWindow) {
	RateTracker tracker(1.0);
	
	tracker.update(100, 0.0);
	tracker.update(100, 1.0);
	tracker.update(100, 2.0);
	
	// At time 2.0, only the last addition should be in window
	double rate = tracker.getRate(2.0);
	EXPECT_NEAR(rate, 100.0, 0.1);
}

