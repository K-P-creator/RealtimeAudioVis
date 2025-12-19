//	This will test the smoothing function smoothMagnitudes() from AudioManager

#include "audio_manager_test_class.h"
#include <ranges>

/*
	We will test to make sure that increasing values are not smoothed.

	We will also test to ensure the smoothing calculations are taking
	place for decreasing values.
*/

TEST_F(AudioManagerTest, smoothingTest) {

	auto allEqZero = [](float x) { return (x == 0.0f); };
	auto allEqOne = [](float x) { return (x == 1.0f); };

	//	Fill sample with all zeros
	std::fill(am.magnitudes.begin(), am.magnitudes.end(), 0.0f);

	//	Run the smoothing algo
	am.smoothMagnitudes();

	//	The previous magnitudes should all be zero now
	//	Check exact equality - all the 0.0f's should be passed straight through
	ASSERT_FALSE(am.magnitudes.empty());
	ASSERT_FALSE(am.prevMagnitudes.empty());
	ASSERT_TRUE(std::ranges::all_of(am.prevMagnitudes, allEqZero));

	//	Input all 1's into magnitudes
	std::fill(am.magnitudes.begin(), am.magnitudes.end(), 1.0f);
	am.smoothMagnitudes();

	//	Make sure all magnitudes are EXACTLY equal to 1 - this means were NOT smoothing increasing values
	ASSERT_TRUE(std::ranges::all_of(am.magnitudes, allEqOne));

	//	Input new data that's DECREASING (0's)
	std::fill(am.magnitudes.begin(), am.magnitudes.end(), 0.0f);
	am.smoothMagnitudes();

	//	Now the magnitudes should be SOMEWHERE between 0 and 1, but NOT equal to either
	ASSERT_TRUE(std::ranges::all_of(am.magnitudes, [](float x) { return (x < 1.0f && x > 0.0f);} ));

	//	Test increasing magnitudes again
	std::fill(am.magnitudes.begin(), am.magnitudes.end(), 1.0f);
	am.smoothMagnitudes();
	ASSERT_TRUE(std::ranges::all_of(am.magnitudes, allEqOne));
}