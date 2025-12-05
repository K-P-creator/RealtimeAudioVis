#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

extern "C" {
#include "kiss_fft.h"
}

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
	GLfloat baseColor[4];
	GLfloat barColor[4];
	float barHeightScale;
	int windowHeight;
	int windowWidth;
	float smoothingCoef;
};

//	Tests classes
class AudioManagerTest;
class AudioManagerTest_ZeroMagnitudeSilence_Test;

class AudioManager {
	//	Need private member access for tests
	friend class AudioManagerTest;	
	friend class AudioManagerTest_ZeroMagnitudeSilence_Test;

	public:
		AudioManager();
		~AudioManager() noexcept;

		void GetAudio();
		void RenderAudio(GLFWwindow *, GLuint &VBO, GLuint &VAO);
		void SetColorFunction();
		void UpdateSmoothing(int);
		void openGLInit(GLuint& VBO, GLuint& VAO);

		GLuint getColorLocation1(){ return this->colorLocation1; }
		GLuint getColorLocation2(){ return this->colorLocation2; }
		GLuint getColorLocation3() { return this->colorLocation3; }
		GLuint getBarCountUniform1(){ return this->barCountUniform1; }
		GLuint getBarCountUniform2(){ return this->barCountUniform2; }
		GLuint getBarCountUniform3() { return this->barCountUniform3; }
		GLuint getDefaultShader() { return this->defaultShaderProgram; }
		GLuint getSymmetricShader() { return this->symmetricShaderProgram; }
		GLuint getDoubleSymmetricShader() { return this->doubleSymmetricShaderProgram; }

		AudioManager(const AudioManager&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;	//	no copies

		AudioManager(AudioManager&& other) noexcept;
		AudioManager& operator=(AudioManager&& other) noexcept;

		Settings settings{DEFAULT_SETTINGS};	// Defined in Globals.h

	private:
		GLuint defaultShaderProgram, symmetricShaderProgram, doubleSymmetricShaderProgram;
		GLuint colorLocation1, colorLocation2, colorLocation3, barCountUniform1, barCountUniform2, barCountUniform3;

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
		std::vector<float> prevMagnitudes;

		// Accumulator for audio data
		std::vector<float> accumulator;

		//	Get the color data for bars
		void genColors();
		std::vector<float> colors;

		//	Smoothed verts - depricated
		//void genSmoothedVerts();

		//	Minimal vertices are the points at the top left of each bar
		//	The purpose of this is to reduce the buffer size as much as possible
		//	and to compute as much on the GPU as we can
		void genMinVerts();
		std::vector<float> minVerts;

		//	Smooths the magnitudes CPU side before they are translated into vertices
		//	GPU side someday?
		void smoothMagnitudes();
};		