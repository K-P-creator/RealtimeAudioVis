#include "../include/AudioManager.h"
#include <gtest/gtest.h>


class AudioManagerTest : public ::testing::Test {
	protected:
	AudioManager am;

	void SetUp() override {
		//	Assert that AM was constructed correctly

		ASSERT_FALSE(am.magnitudes.empty());
		ASSERT_TRUE(am.prevMagnitudes.empty());

		ASSERT_EQ(am.getDefaultShader(), 0);
		ASSERT_EQ(am.getSymmetricShader(), 0);
		ASSERT_EQ(am.getDoubleSymmetricShader(), 0);
		
		ASSERT_EQ(am.getColorLocation1(), 0);
		ASSERT_EQ(am.getColorLocation2(), 0);
		ASSERT_EQ(am.getColorLocation3(), 0);
		ASSERT_EQ(am.getBarCountUniform1(), 0);
		ASSERT_EQ(am.getBarCountUniform2(), 0);
		ASSERT_EQ(am.getBarCountUniform3(), 0);
	}
};