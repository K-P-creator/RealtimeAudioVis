// glad/opengl/glfw includes
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
#include <array>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>

#include "Globals.h"

#define THROW_ON_ERROR(hres, x)  \
              if (FAILED(hres)) { throw std::runtime_error(x); }

struct Settings {
	bool smoothing;
	unsigned int modeIndex;
	GLfloat baseColor[3];
};

class AudioManager {
	public:
		AudioManager();
		~AudioManager() noexcept;

		void GetAudio();
		void RenderAudio(GLFWwindow *, GLuint &VBO, const Settings&);
		void SetColorFunction();

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

		//	Get the vertex data for bars
		// sequential vector of vertecies to draw
		void genVerts();
		std::vector <float> verts;

		//	Get the color data for bars
		void genColors(const GLfloat c[4]);
		std::vector<std::array<float,4>> colors;

		//	Smoothed verts
		void genSmoothedVerts();
};		