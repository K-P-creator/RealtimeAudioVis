// SFML includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

//audio headrers
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

// KissFFT for audio transforms
extern "C" {
#include "kiss_fft.h"
}

// STL includes
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>

#include "Globals.h"



#define THROW_ON_ERROR(hres, x)  \
              if (FAILED(hres)) { throw std::runtime_error(x); }

class AudioManager {
	public:
		// Inits all the windows audio stuff and begins storing the output audio in buffer
		AudioManager();
		~AudioManager() noexcept;


		void GetAudio();
		void RenderAudio(sf::RenderWindow *) const;
		void SetColorFunction();

		// Renders the graph in the center and mirrors on the left side
		void RenderAudioSymmetric(sf::RenderWindow *) const; 

		// This will perform a two way mirror with lines of symmetry at y = W_HEIGHT/2 and x = W_WIDTH/2
		void RenderAudioFourWaySym(sf::RenderWindow*) const;

		// two way mirror with smoothing
		void RenderAudioWithSmoothing(sf::RenderWindow* w);

		// No copys allowed
		AudioManager(const AudioManager&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;

		// Move constructor
		AudioManager(AudioManager&& other) noexcept;

		// Move assignment
		AudioManager& operator=(AudioManager&& other) noexcept;

	private:
		// WASAPI interfaces
		IMMDeviceEnumerator* pEnumerator = NULL;
		IMMDevice* pDevice = NULL;
		IAudioClient* pAudioClient = NULL;
		IAudioCaptureClient* pCaptureClient = NULL;

		REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;

		// Global GUID's for devices
		const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
		const IID IID_IAudioClient = __uuidof(IAudioClient);
		const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

		// Struct to describe the audio properties
		WAVEFORMATEX* pwfx = NULL;

		// Kiss FFT
		kiss_fft_cfg cfg = nullptr;

		// Data for visualization
		std::vector<float> magnitudes;

		// Magnitudes for previous cycle
		std::vector<float> prevMagnitudes;

		// Accumulator for audio data
		std::vector<float> accumulator;

		// Takes in the index of the bar and the hieght and generates a color based off what
		// Color generation mechanism the user selects
		sf::Color GetColor(size_t, float) const;
		char color; // Data for color function
};