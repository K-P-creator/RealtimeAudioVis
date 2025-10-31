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
#include <fstream>
#include <sstream>

#include "Globals.h"

#define THROW_ON_ERROR(hres, x)  \
              if (FAILED(hres)) { throw std::runtime_error(x); }

#define DEFAULT_M		0
#define SYMMETRIC_M		1
#define DOUBLE_SYM_M	2

struct Settings {
	bool smoothing;
	unsigned int modeIndex; //Default = 0, Symmetric = 1; Double Symetric = 2
	GLfloat baseColor[3];
	float barHeightScale;
	int windowHeight;
	int windowWidth;
	float smoothingCoef;
};

class AudioManager {
	public:
		AudioManager();
		~AudioManager() noexcept;

		void GetAudio();
		void RenderAudio(GLFWwindow *, GLuint &VBO, GLuint&TBO, GLuint&);
		void SetColorFunction();
		void UpdateSmoothing(int);
		void openGLInit(GLuint& VBO, GLuint& VAO, GLuint& EBO, GLuint& TBO);

		GLuint getDefaultShader() { return this->defaultShaderProgram; }
		GLuint getSymmetricShader() { return this->symmetricShaderProgram; }

		// No copys allowed
		AudioManager(const AudioManager&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;

		// Move constructor
		AudioManager(AudioManager&& other) noexcept;

		// Move assignment
		AudioManager& operator=(AudioManager&& other) noexcept;

		Settings settings{DEFAULT_SETTINGS};

	private:
		GLuint defaultShaderProgram, symmetricShaderProgram;

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
		void genColors();
		std::vector<float> colors;

		//	Smoothed verts
		void genSmoothedVerts();

		//	Minimal vertices are the points at the top left of each bar
		//	The purpose of this is to reduce the buffer size as much as possible
		//	and to compute as much on the GPU as we can
		void genMinVerts();
		std::vector<float> minVerts;
};		