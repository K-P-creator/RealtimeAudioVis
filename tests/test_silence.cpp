//	Tests that given no audio input, the magnitudes will all be == (0 +- epsilon)

#include "audio_manager_test_class.h"

TEST_P(AudioManagerTest, ZeroMagnitudeSilence) {
	for (auto& i : am.magnitudes) {

	}
};
