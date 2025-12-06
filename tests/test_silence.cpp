//	Tests that given no audio input, the magnitudes will all be == (0 +- epsilon)

#include "audio_manager_test_class.h"

TEST_F(AudioManagerTest, silenceTest) {
	//	Get a few frames of audio
	for (int i = 0; i < 10; i ++)
	{
		am.getAudioSample();
		am.vectorizeMagnitudes();
	}

	//	Fill the sample wtih all 0's, should output all 0 magnitudes
	std::fill(am.visualData.begin(), am.visualData.end(), kiss_fft_cpx(0,0));
	std::fill(am.accumulator.begin(), am.accumulator.end(), 0.0f);
	am.vectorizeMagnitudes();

	ASSERT_FALSE(am.magnitudes.empty());
	bool allZero = true;
	for (const auto i : am.magnitudes)
		if (!(i < 0.0f + FLT_EPSILON && i > 0.0f - FLT_EPSILON))
			allZero = false;

	ASSERT_TRUE(allZero);


	//	Set the smoothing and make sure the prevMangitudes is storing the correct data
	//	These should be all zeros because we just filled with 0's
	am.settings.smoothing = true;
	am.getAudioSample();
	am.vectorizeMagnitudes();
	am.smoothMagnitudes();

	ASSERT_FALSE(am.prevMagnitudes.empty());
	allZero = true;
	for (const auto i : am.prevMagnitudes)
		if (!(i < 0.0f + FLT_EPSILON && i > 0.0f - FLT_EPSILON))
			allZero = false;

	ASSERT_TRUE(allZero);
};
