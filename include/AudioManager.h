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
class GraphicsTest;
class AudioManagerTest_silenceTest_Test;
class AudioManagerTest_smoothingTest_Test;

class AudioManager {
	//	Need private member access for tests
	friend class AudioManagerTest;	
	friend class GraphicsTest;
	friend class AudioManagerTest_silenceTest_Test;
	friend class AudioManagerTest_smoothingTest_Test;

	public:
		AudioManager();
		~AudioManager() noexcept;

		void RenderAudio(GLFWwindow *, GLuint &VBO, GLuint &VAO);
		void SetColorFunction();
		void UpdateSmoothing(int);
		void openGLInit(GLuint& VBO, GLuint& VAO);
		void compileShader(const char* source, GLuint& name, GLenum type, int & success);

		GLuint getColorLocation1() const { return this->colorLocation1; }
		GLuint getColorLocation2() const { return this->colorLocation2; }
		GLuint getColorLocation3() const { return this->colorLocation3; }
		GLuint getBarCountUniform1() const { return this->barCountUniform1; }
		GLuint getBarCountUniform2() const { return this->barCountUniform2; }
		GLuint getBarCountUniform3() const { return this->barCountUniform3; }
		GLuint getDefaultShader() const { return this->defaultShaderProgram; }
		GLuint getSymmetricShader() const { return this->symmetricShaderProgram; }
		GLuint getDoubleSymmetricShader() const { return this->doubleSymmetricShaderProgram; }

		AudioManager(const AudioManager&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;	//	no copies

		AudioManager(AudioManager&& other) noexcept;
		AudioManager& operator=(AudioManager&& other) noexcept;

		Settings settings{DEFAULT_SETTINGS};	// Defined in Globals.h

		bool hasValidAudioDevice() const { return this->validAudioDevice; }

	private:
		bool getAudioSample();		//Returns false if the sample is empty
		void vectorizeMagnitudes();


		GLuint defaultShaderProgram, symmetricShaderProgram, doubleSymmetricShaderProgram;
		GLuint colorLocation1, colorLocation2, colorLocation3, barCountUniform1, barCountUniform2, barCountUniform3;

		// WASAPI interfaces
		IMMDeviceEnumerator* pEnumerator = NULL;
		IMMDevice* pDevice = NULL;
		IAudioClient* pAudioClient = NULL;
		IAudioCaptureClient* pCaptureClient = NULL;
		REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;

		bool validAudioDevice = true;	//	For CI tests, no audio device present

		// Global GUID's for devices
		const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
		const IID IID_IAudioClient = __uuidof(IAudioClient);
		const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

		//	For capture client
		BYTE* pData = nullptr;
		UINT32 numFramesAvailable = 0;
		DWORD flags = 0;
		HRESULT hr = 0;
		int numChannels = 0;

		// Struct to describe the audio properties
		WAVEFORMATEX* pwfx = nullptr;

		// Kiss FFT
		kiss_fft_cfg cfg = nullptr;
		std::vector<kiss_fft_cpx> audioSample;
		std::vector<kiss_fft_cpx> visualData;

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

static std::string fileToString(const char* path) {
	std::ifstream file;
	file.open(path);

	if (!file) throw std::invalid_argument(std::string("Unable to open file: ") + path);

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}
