#include "../include/AudioManager.h"

static std::string fileToString(const char*);

AudioManager::AudioManager()
{
	// Kiss FFT setup
	cfg = kiss_fft_alloc(FFT_COUNT, 0, nullptr, nullptr);

	// Allocate mem for the visualization data
	magnitudes.resize(FFT_COUNT/2);
	prevMagnitudes.reserve(FFT_COUNT / 2);
	colors.resize(BAR_COUNT * 3);

	// Contains high for failure, low for success
	HRESULT hr;

	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	THROW_ON_ERROR(hr, "Unable to initialize COM library in AudioManager()");

	// Create instance of COM object in pEnumerator
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	THROW_ON_ERROR(hr, "Unable to Initialize pEnumerator in AudioManager()")


		// Get the default audio output (eRender for out)
		hr = pEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &pDevice);
	THROW_ON_ERROR(hr, "Unable to source default audio output in AudioManager()")


		// Get the audio client interface and store in pAudioClient
		hr = pDevice->Activate(
			IID_IAudioClient, CLSCTX_ALL,
			NULL, (void**)&pAudioClient);
	THROW_ON_ERROR(hr, "Unable to get audio client in AudioManager()")


		// Get the format for the audio stream
		hr = pAudioClient->GetMixFormat(&pwfx);
	THROW_ON_ERROR(hr, "Unable to get mix format in AudioManager()")


		// Initialize the client
		// LOOPBACK flag allows for listening to the audio being played
		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_LOOPBACK,
			hnsRequestedDuration,
			0,
			pwfx,
			NULL);
	THROW_ON_ERROR(hr, "Unable to initialize audio client in AudioManager()")


		// Get the capture client to actually listen
		hr = pAudioClient->GetService(
			IID_IAudioCaptureClient,
			(void**)&pCaptureClient);
	THROW_ON_ERROR(hr, "Unable to get capture client in AudioManager()");


	// Start recording audio
	hr = pAudioClient->Start();
	THROW_ON_ERROR(hr, "Unable to start audio client in AudioManager()");

	defaultShaderProgram = symmetricShaderProgram = 0;
	barCountUniform1 = barCountUniform2 = colorLocation1 = colorLocation2 = 0;
}


AudioManager::~AudioManager() noexcept
{
	// Release all memory
	if (pEnumerator) {
		pEnumerator->Release();
		pEnumerator = nullptr;
	}
	if (pwfx) {
		CoTaskMemFree(pwfx);
		pwfx = NULL;
	}
	if (pDevice) {
		pDevice->Release();
		pDevice = nullptr;
	}
	if (pAudioClient) {
		pAudioClient->Release();
		pAudioClient = nullptr;
	}
	if (pCaptureClient) {
		pCaptureClient->Release();
		pCaptureClient = nullptr;
	}
	if (cfg) {
		free(cfg);
		cfg = nullptr;
	}
}


AudioManager::AudioManager(AudioManager&& other) noexcept 
{
	// Move all data
	pEnumerator = other.pEnumerator;
	pDevice = other.pDevice;
	pAudioClient = other.pAudioClient;
	pCaptureClient = other.pCaptureClient;
	hnsRequestedDuration = other.hnsRequestedDuration;
	pwfx = other.pwfx;
	cfg = other.cfg;
	magnitudes = std::move(other.magnitudes);
	accumulator = std::move(other.accumulator);
	colors = std::move(other.colors);
	defaultShaderProgram = other.defaultShaderProgram;
	symmetricShaderProgram = other.symmetricShaderProgram;
	barCountUniform1 = other.barCountUniform1;
	barCountUniform2 = other.barCountUniform2;
	colorLocation1 = other.colorLocation1;
	colorLocation2 = other.colorLocation2;


	// Invalidate the source
	other.pEnumerator = nullptr;
	other.pDevice = nullptr;
	other.pAudioClient = nullptr;
	other.pCaptureClient = nullptr;
	other.pwfx = nullptr;
	other.cfg = nullptr;
}


AudioManager& AudioManager::operator=(AudioManager&& other) noexcept 
{
	if (this == &other) return *this;


	// Release existing resources first
	if (pEnumerator) pEnumerator->Release();
	if (pDevice) pDevice->Release();
	if (pAudioClient) pAudioClient->Release();
	if (pCaptureClient) pCaptureClient->Release();
	if (pwfx) CoTaskMemFree(pwfx); 
	if (cfg) free(cfg);


	// Move data
	pEnumerator = other.pEnumerator;
	pDevice = other.pDevice;
	pAudioClient = other.pAudioClient;
	pCaptureClient = other.pCaptureClient;
	hnsRequestedDuration = other.hnsRequestedDuration;
	pwfx = other.pwfx;
	cfg = other.cfg;
	magnitudes = std::move(other.magnitudes);
	accumulator = std::move(other.accumulator);


	// Invalidate the source
	other.pEnumerator = nullptr;
	other.pDevice = nullptr;
	other.pAudioClient = nullptr;
	other.pCaptureClient = nullptr;
	other.pwfx = nullptr;
	other.cfg = nullptr;

	return *this;
}


// Fills the vector with audio samples at various frequencies
void AudioManager::GetAudio()
{
	BYTE* pData;
	UINT32 numFramesAvailable;
	DWORD flags;
	HRESULT hr;
	std::vector<kiss_fft_cpx> audioSample(FFT_COUNT);
	std::vector<kiss_fft_cpx> visualData(FFT_COUNT);
	int numChannels = pwfx->nChannels;


	// Get the current audio buffer
	hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
	if (FAILED(hr))
	{
		pCaptureClient->ReleaseBuffer(numFramesAvailable);
		std::cerr << "hr failed with code: " << hr << std::endl;
		THROW_ON_ERROR(hr, "Failed to get buffer");
	}



	// Flags is a bitfield returned by buffer, Bitwise AND to check for silence
	// If silent, fill the sample with all 0.0f and return to save time
	if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
		std::fill(magnitudes.begin(), magnitudes.end(), 0.0f);
		pCaptureClient->ReleaseBuffer(numFramesAvailable);
		return;
	}


	// Copy the audio sample into a kissfft friendly vector
	float* floatData = reinterpret_cast<float*>(pData);

	for (UINT32 i = 0; i < numFramesAvailable; ++i) {
		if (numChannels == 2) {
			// Average left and right
			accumulator.push_back(0.5f * (floatData[i * 2] + floatData[i * 2 + 1]));
		}
		else if (numChannels == 1) {
			// Mono already
			accumulator.push_back(floatData[i]);
		}
	}

	if (accumulator.size() >= FFT_COUNT){
		for (UINT32 i = 0; i < numFramesAvailable; i++) {
			if (i < audioSample.size()) {
				audioSample[i].r = accumulator[i];
				audioSample[i].i = 0.0f; // no imaginary component
			}
		}


		accumulator.resize(0);

		// Let windows know youre done reading from the buffer so it can overwrite
		pCaptureClient->ReleaseBuffer(numFramesAvailable);

		if (numFramesAvailable < FFT_COUNT) {
			std::cout << "Not enough Frames!" << std::endl;
		}

		// Apply Hanning window
		//for (UINT32 i = 0; i < FFT_COUNT; ++i) {
		//	float multiplier = 0.5f * (1.0f - cosf(2.0f * 3.1415926535f * i / (FFT_COUNT - 1)));
		//	audioSample[i].r *= multiplier;
		//}

		// Do the FFT to get the output data
		kiss_fft(cfg, audioSample.data(), visualData.data());


		// Store the resulting magnitudes back into the original vector
		// visualData.r will now contain the magnitudes
		// We only need half the data here because the FFT is symmetric
		for (UINT32 i = 0; i < FFT_COUNT / 2; ++i) {
			magnitudes[i] = sqrtf(visualData[i].r * visualData[i].r + visualData[i].i * visualData[i].i);
			magnitudes[i] = log2(magnitudes[i]);
			magnitudes[i] *= static_cast<float>(settings.windowHeight / 10.0f);
		}
	}

	return;
}


void AudioManager::RenderAudio(GLFWwindow * w, GLuint &VBO, GLuint &VAO)
{
	if (!w) throw (std::invalid_argument("No render window found in RenderAudio()"));

	this->genMinVerts();
	glBindVertexArray(VAO);


	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * minVerts.size(), minVerts.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute index (must match your shader's layout(location = 0))
		2,                  // number of components per vertex attribute
		GL_FLOAT,           // type
		GL_TRUE,           // normalized?
		2 * sizeof(float),  // stride (distance between consecutive vertices)
		(void*)0            // offset in the buffer
	);


	static int prevModeIndx = -1;

	if (prevModeIndx != settings.modeIndex)
	{
		switch (settings.modeIndex) {
		case DEFAULT_M:

			glUseProgram(defaultShaderProgram);
			glUniform4f(colorLocation1, settings.barColor[0], settings.barColor[1], settings.barColor[2], settings.barColor[3]);
			glUniform1i(barCountUniform2, BAR_COUNT);
			prevModeIndx = settings.modeIndex;
			break;

		case SYMMETRIC_M:

			glUseProgram(symmetricShaderProgram);
			glUniform4f(colorLocation2, settings.barColor[0], settings.barColor[1], settings.barColor[2], settings.barColor[3]);
			glUniform1i(barCountUniform2, BAR_COUNT);
			prevModeIndx = settings.modeIndex;
			break;

		case DOUBLE_SYM_M:
			break;
		}
	}

	glDrawArrays(GL_POINTS, 0, minVerts.size() / 2);
	glBindVertexArray(0);
}


// Depricated
//void AudioManager::genSmoothedVerts() {
	//bool first = false;
	//if (prevMagnitudes.empty()) {
	//	first = true;
	//	prevMagnitudes.resize(prevMagnitudes.capacity());
	//}

	//if (verts.size() < BAR_COUNT * 12)
	//	verts.resize(BAR_COUNT * 12);

	//unsigned int pixPerBar = float(settings.windowWidth) / float(BAR_COUNT);
	//float horizScale = 2 * float(pixPerBar) / float(settings.windowWidth);
	//float vertScale = this->settings.barHeightScale * 1 / this->settings.windowHeight;

	//for (size_t i = 0; i < BAR_COUNT; i++) {
	//	int indx = i * 4 * 3;

	//	if (magnitudes[i] <= 0.0f) {
	//		prevMagnitudes[i] = 0.0f;
	//		std::fill(verts.begin() + indx, verts.begin() + indx + 12, -1.0f);
	//		continue;
	//	}

	//	float smoothedHeight;
	//	if (!first){
	//		smoothedHeight = settings.smoothingCoef * prevMagnitudes[i] + (1 - settings.smoothingCoef) * magnitudes[i];
	//		if (prevMagnitudes[i] < magnitudes[i]) 
	//		{
	//			smoothedHeight = magnitudes[i];
	//		}
	//	}
	//	else smoothedHeight = magnitudes[i];

	//	float x1 = horizScale * i - 1.0f;
	//	float x2 = x1 + horizScale;
	//	float y = smoothedHeight * vertScale - 1.0f;


	//	//Order of triangles will be 1,2,3 and 2,3,4
	//	//	 x1,0,0 x2,0,0 x1,y,0 x2,y,0 
	//	verts[indx] = x1;
	//	verts[indx + 1] = -1.0f;
	//	verts[indx + 2] = 0.0f;
	//	verts[indx + 3] = x2;
	//	verts[indx + 4] = -1.0f;
	//	verts[indx + 5] = 0.0f;
	//	verts[indx + 6] = x1;
	//	verts[indx + 7] = y;
	//	verts[indx + 8] = 0.0f;
	//	verts[indx + 9] = x2;
	//	verts[indx + 10] = y;
	//	verts[indx + 11] = 0.0f;

	//	prevMagnitudes[i] = smoothedHeight;
	//}
//}


void AudioManager::genMinVerts() {
	static bool first = true;
	float pixPerBar = settings.windowWidth / float(BAR_COUNT);

	float horizScale = 2.0f * float(pixPerBar) / float(settings.windowWidth);
	float vertScale = this->settings.barHeightScale * 1 / this->settings.windowHeight;
	
	if (first) minVerts.resize(BAR_COUNT * 2);

	for (int i = 0; i < BAR_COUNT; i ++) {
		minVerts[i * 2] = i * horizScale - 1.0f;	//	x
		if (magnitudes[i] <= 0.01f)			// y
			minVerts[i * 2 + 1] = 0.01f;	
		else
			minVerts[i * 2 + 1] = magnitudes[i] * vertScale; 
	}
}


void AudioManager::genColors() {
	for (int i = 0; i < BAR_COUNT; i++) {
		int indx = i * 3;
		colors[indx] = settings.baseColor[0];
		colors[indx + 1] = settings.baseColor[1];
		colors[indx + 2] = settings.baseColor[2];
	}
}


void AudioManager::UpdateSmoothing(int val) {
	switch (val) {
	case 1: settings.smoothingCoef = 0.9f; break;
	case 2: settings.smoothingCoef = 0.99f; break;
	case 3: settings.smoothingCoef = 0.999f; break;
	case 4: settings.smoothingCoef = 0.9999f; break;
	case 5: settings.smoothingCoef = 0.99999f; break;
	default: throw std::runtime_error("Invalid smoothing coef selection\n");
	}
}


void AudioManager::openGLInit(GLuint& VBO, GLuint& VAO) {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to load openGL func pointers with glad\n";
		std::abort();
	}
	std::cout << "OpenGL Info\n\n";
	std::cout << "Vendor:   \t" << glGetString(GL_VENDOR) << "\n";
	std::cout << "Renderer: \t" << glGetString(GL_RENDERER) << "\n";
	std::cout << "Version:  \t" << glGetString(GL_VERSION) << "\n";
	glfwSwapInterval(1); // Enable vsync


	//  Set up shaders
	int success;
	auto shaderInit = [&success](const char* source, GLuint& name, GLenum type) {
		name = glCreateShader(type);
		glShaderSource(name, 1, &source, NULL);
		glCompileShader(name);
		glGetShaderiv(name, GL_COMPILE_STATUS, &success);

		if (!success) {
			GLint len = 0; glGetShaderiv(name, GL_INFO_LOG_LENGTH, &len);
			std::string log(len, '\0');
			glGetShaderInfoLog(name, len, nullptr, log.data());
			std::cerr << "Shader compile failed (" << name << "):\n" << log << "\n";
			std::abort();
		}
		};


	defaultShaderProgram = glCreateProgram();
	symmetricShaderProgram = glCreateProgram();

	GLuint vertexShader;
	shaderInit(fileToString("../shaders/default.vert").data(), vertexShader, GL_VERTEX_SHADER);
	GLuint fragmentShader;
	shaderInit(fileToString("../shaders/default.frag").data(), fragmentShader, GL_FRAGMENT_SHADER);
	GLuint symGeomShader;
	shaderInit(fileToString("../shaders/symmetric.geom").data(), symGeomShader, GL_GEOMETRY_SHADER);
	GLuint defGeomShader;
	shaderInit(fileToString("../shaders/default.geom").data(), defGeomShader, GL_GEOMETRY_SHADER);



	glAttachShader(defaultShaderProgram, vertexShader);
	glAttachShader(defaultShaderProgram, fragmentShader);
	glAttachShader(defaultShaderProgram, defGeomShader);
	glLinkProgram(defaultShaderProgram);

	glGetProgramiv(defaultShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		std::cerr << "Failed to link default shaders\n";
		std::abort();
	}


	glAttachShader(symmetricShaderProgram, vertexShader);
	glAttachShader(symmetricShaderProgram, fragmentShader);
	glAttachShader(symmetricShaderProgram, symGeomShader);
	glLinkProgram(symmetricShaderProgram);

	glGetProgramiv(symmetricShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		std::cerr << "Failed to link symmetric shaders\n";
		std::abort();
	}


	glValidateProgram(defaultShaderProgram);
	glValidateProgram(symmetricShaderProgram);


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(symGeomShader);
	glDeleteShader(defGeomShader);

	// init uniforms
	glUseProgram(getDefaultShader());
	colorLocation1 = glGetUniformLocation(getDefaultShader(), "BaseColor");
	barCountUniform1 = glGetUniformLocation(getDefaultShader(), "BarCount");
	glUniform4f(colorLocation1, settings.barColor[0], settings.barColor[1], settings.barColor[2], settings.barColor[3]);
	glUniform1i(barCountUniform2, BAR_COUNT);


	glUseProgram(getSymmetricShader());
	colorLocation2 = glGetUniformLocation(getSymmetricShader(), "BaseColor");
	barCountUniform2 = glGetUniformLocation(getSymmetricShader(), "BarCount");
	glUniform4f(colorLocation2, settings.barColor[0], settings.barColor[1], settings.barColor[2], settings.barColor[3]);
	glUniform1i(barCountUniform2, BAR_COUNT);

	//  Set up buffers 
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}


static std::string fileToString(const char* path) {
	std::ifstream file;
	file.open(path);

	if (!file) throw std::invalid_argument(std::string("Unable to open file: ") + path);

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}
