#include "../include/AudioManager.h"
#include <gtest/gtest.h>

class AudioManagerTest : public ::testing::Test {
	protected:
	AudioManager am;

	void SetUp() override {
		ASSERT_FALSE(am.magnitudes.empty());
		ASSERT_FALSE(am.accumulator.empty());
		ASSERT_FALSE(am.prevMagnitudes.empty());
	}
};