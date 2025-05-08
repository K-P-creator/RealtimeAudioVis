#include "../include/AudioManager.h"


AudioManager::AudioManager()
{
	// Color selection to default
	this->color = '\0';

	// Kiss FFT setup
	cfg = kiss_fft_alloc(FFT_COUNT, 0, nullptr, nullptr);

	// Allocate mem for the visualization data
	magnitudes.resize(FFT_COUNT/2);
	prevMagnitudes.reserve(FFT_COUNT / 2);

	// Contains high for failure, low for success
	HRESULT hr;

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
	color = other.color;


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


sf::Color AudioManager::GetColor(size_t i, float mag) const {
	uint8_t coly = static_cast<uint8_t>((static_cast<unsigned int>((mag / WINDOW_HEIGHT) * mag) + 1) % 255);
	uint8_t colx = static_cast<uint8_t>((i + 1) % 255);

	switch (this->color) {
	case 'r':  // Red
		return sf::Color({ 255, 0, 0 });
	case 'g':  // Green
		return sf::Color({ 0, 255, 0 });
	case 'b':  // Blue
		return sf::Color({ 0, 0, 255 });

		// Color fades based on i and mag
	case '1':
		return sf::Color({ uint8_t(255 - colx), 0, colx });          // Red - Blue
	case '2':
		return sf::Color({ uint8_t(255 - coly), 0, coly });          // Red - Blue (vertical)
	case '3':
		return sf::Color({ uint8_t(255 - coly), colx, 0 });          // Orange - Green
	case '4':
		return sf::Color({ colx, uint8_t(255 - coly), coly });       // Color swirl
	case '5':
		return sf::Color({ colx, coly, 255 });                       // Blue-glow spectrum
	case '6':
		return sf::Color({ uint8_t(colx ^ coly), colx, coly });      // XOR mash
	case '7':
		return sf::Color({ uint8_t(uint8_t(i * mag) % 255), coly, colx });  // Wild fire (based on intensity)
	case '8':
		return sf::Color({ uint8_t(sin(i * 0.1) * 127 + 128),        // Sin wave red
						   uint8_t(cos(i * 0.05) * 127 + 128),       // Cos wave green
						   uint8_t(sin(i * 0.2 + mag) * 127 + 128) });// Sin-cos dynamic purple
	default:
		return sf::Color({ 255, 255, 255 }); // White fallback
	}
}


void AudioManager::SetColorFunction() {
	switch (color) {
	case 'r': color = 'g'; break;
	case 'g': color = 'b'; break;
	case 'b': color = '1'; break;
	case '1': color = '2'; break;
	case '2': color = '3'; break;
	case '3': color = '4'; break;
	case '4': color = '5'; break;
	case '5': color = '6'; break;
	case '6': color = '7'; break;
	case '7': color = '8'; break;
	case '8': color = 'r'; break;
	default: color = 'r'; break;
	}
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
			magnitudes[i] *= static_cast<float>(WINDOW_HEIGHT / 10.0f);
		}
	}

	return;
}


// Draws the audio vector to the SFML window
void AudioManager::RenderAudio(sf::RenderWindow * w) const
{
	if (!w) throw (std::invalid_argument("No render window found in RenderAudio()"));

	sf::RectangleShape r;
	size_t barCount = FFT_COUNT/4;
	
	auto barWidth = static_cast<float>(WINDOW_WIDTH / barCount);

	for (size_t i = 0; i < barCount; i++) {
		if (magnitudes[i] <= 0.1) continue;
		r.setPosition({ barWidth * static_cast<float>(i), WINDOW_HEIGHT - magnitudes[i] });
		r.setSize({ barWidth, magnitudes[i]});
		r.setFillColor(GetColor(i,magnitudes[i]));
		w->draw(r);
	}

	return;
}


// Symmetric version
void AudioManager::RenderAudioSymmetric(sf::RenderWindow * w) const {
	if (!w) throw (std::invalid_argument("No render window found in RenderAudioSymmetric()"));

	sf::RectangleShape r;
	size_t barCount = FFT_COUNT / 4;

	// Bars are twice as skinny because there's 2x more
	auto barWidth = static_cast<float>(WINDOW_WIDTH / (barCount * 2));
	float mid = WINDOW_WIDTH / 2;

	// Draw starting at the center, and draw two rectangles mirror for every bar
	for (size_t i = 0; i < barCount; i++) {
		if (magnitudes[i] <= 0.1) continue;
		float barHeight = WINDOW_HEIGHT - magnitudes[i];
		float barOffset = static_cast<float>(i) * barWidth;

		r.setPosition({ mid + barOffset, barHeight });
		r.setSize({ barWidth, magnitudes[i] });
		r.setFillColor(GetColor(i, magnitudes[i]));
		w->draw(r);

		// Draw mirrored bar now
		r.setPosition({ mid - barOffset, barHeight });
		w->draw(r);
	}
}


// Render functionn with two way symmetry
void AudioManager::RenderAudioFourWaySym(sf::RenderWindow* w) const {
	if (!w) throw (std::invalid_argument("No Render Window found in RenderAudioFourWaySym()"));

	sf::RectangleShape r;
	size_t barCount = FFT_COUNT / 4;

	// Bars are twice as skinny because there's 2x more
	auto barWidth = static_cast<float>(WINDOW_WIDTH / (barCount * 2));
	float mid = WINDOW_WIDTH / 2;

	// Draw starting at the center, and draw two rectangles mirror for every bar
	for (size_t i = 0; i < barCount; i++) {
		if (magnitudes[i] <= 0.1) continue;
		float halfMag = magnitudes[i] / 2;
		float barHeight = WINDOW_HEIGHT/2 - halfMag;
		float barOffset = static_cast<float>(i) * barWidth;

		r.setPosition({ mid + barOffset, barHeight });
		r.setSize({ barWidth, magnitudes[i]});
		r.setFillColor(GetColor(i, magnitudes[i]));
		w->draw(r);

		// Draw mirrored bar now
		r.setPosition({ mid - barOffset, barHeight });
		w->draw(r);
	}
}


// Render functionn with two way symmetry with smoothing
void AudioManager::RenderAudioWithSmoothing(sf::RenderWindow* w) {
	if (!w) throw (std::invalid_argument("No Render Window found in RenderAudioFourWaySym()"));

	sf::RectangleShape r;
	size_t barCount = FFT_COUNT / 4;

	// Bars are twice as skinny because there's 2x more
	auto barWidth = static_cast<float>(WINDOW_WIDTH / (barCount * 2));
	float mid = WINDOW_WIDTH / 2;

	bool first = false;
	if (prevMagnitudes.empty()) 
	{
		prevMagnitudes.resize(FFT_COUNT / 2);
		first = true;
	}

	// Draw starting at the center, and draw two rectangles mirror for every bar
	for (size_t i = 0; i < barCount; i++) {
		// Skip this iteration if negative or zero magnitude
		if (magnitudes[i] <= 0.0)
		{
			prevMagnitudes[i] = 0;
			continue;
		}

		// Height of the bar after applying smoothing
		// If height is increasing, dont smooth. This increases responsiveness A LOT
		float smoothedHeight;
		if (!first) 
		{
			smoothedHeight = SMOOTHING_COEF * prevMagnitudes[i] + (1 - SMOOTHING_COEF) * magnitudes[i];
			if (smoothedHeight < magnitudes[i]) smoothedHeight = magnitudes[i];
		}
		else smoothedHeight = magnitudes[i];

		// X offset for bars
		float barOffset = static_cast<float>(i) * barWidth;

		// Upper left vertex at (middle +- offset, (W_HEIGHT - BAR_HEIGHT) / 2
		r.setPosition({ mid + barOffset, (WINDOW_HEIGHT - smoothedHeight) / 2 });
		r.setSize({ barWidth, smoothedHeight });
		r.setFillColor(GetColor(i, smoothedHeight));
		w->draw(r);

		// Draw mirrored bar now
		r.setPosition({ mid - barOffset, (WINDOW_HEIGHT - smoothedHeight) / 2 });
		w->draw(r);

		// Save the smoothed hieght as the prev height
		prevMagnitudes[i] = smoothedHeight;
	}

	magnitudes.clear();
	magnitudes.resize(FFT_COUNT / 2);
}


// Render as a curve (with smoothing)
void AudioManager::RenderAudioCurve(sf::RenderWindow* w) {
	if (!w) throw (std::invalid_argument("No Render Window found in RenderAudioCurve()"));

	sf::Curve c({ 0, 0 }, { 0, 0 }, { 0, 0 });
	c.setThickness(5.0f);
	c.setVertexCount(50);

	auto widthScale = static_cast<float>(WINDOW_WIDTH / (magnitudes.size() / 3));

	bool first = false;
	if (prevMagnitudes.empty())
	{
		prevMagnitudes.resize(FFT_COUNT / 2);
		first = true;
	}

	std::vector <float> smoothedHeights;
	smoothedHeights.resize(magnitudes.size());
	for (unsigned int i = 0; i < magnitudes.size(); i++) {
		if (magnitudes[i] <= 0.0f) {
			smoothedHeights[i] = 0.0f;
			continue;
		}
		// Height of the bar after applying smoothing
		// If height is increasing, dont smooth. This increases responsiveness A LOT
		if (!first)
		{
			smoothedHeights[i] = SMOOTHING_COEF * prevMagnitudes[i] + (1 - SMOOTHING_COEF) * magnitudes[i];
			if (smoothedHeights[i] <= magnitudes[i]) smoothedHeights[i] = magnitudes[i];
		}
		else smoothedHeights[i] = magnitudes[i];
	}

	for (unsigned int i = 0; i < magnitudes.size(); i += 2) {
		
		// Draw the top half
		c.setPoints({ static_cast<float>(i * widthScale), WINDOW_HEIGHT/2 - smoothedHeights[i]/2 }, 
			{ static_cast<float>((i + 2) * widthScale), WINDOW_HEIGHT/2 - smoothedHeights[i + 2]/2 },
			{ static_cast<float>((i + 1) * widthScale), WINDOW_HEIGHT/2 - smoothedHeights[i + 1]/2 });
		c.setColor(GetColor(i, smoothedHeights[i + 1]));
		w->draw(c);

		// Draw the bottom half
		c.setPoints({ static_cast<float>(i * widthScale), WINDOW_HEIGHT / 2 + smoothedHeights[i] / 2 },
			{ static_cast<float>((i + 2) * widthScale), WINDOW_HEIGHT / 2 + smoothedHeights[i + 2] / 2 },
			{ static_cast<float>((i + 1) * widthScale), WINDOW_HEIGHT / 2 + smoothedHeights[i + 1] / 2 });
		w->draw(c);

		// Save the smoothed hieght as the prev height
		prevMagnitudes[i] = smoothedHeights[i];
	}


}