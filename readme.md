# Realtime Audio Vis

## Info

**WINDOWS ONLY**
Relies on Windows WASAPI to sample sound info from the default audio device. 
This way you can run the .exe and play any audio you want, including encrypted 
downloads on your favorite music streaming app.

STD = C++20
SFML Version 3.0.1 for graphics
kissFFT for Audio Fast Fourier Transform
MSVC 2022

## Usage

After executing the .exe, use

```esc```

to toggle fullscreen mode. Note that there is a 3s cooldown for switching window modes.

Use:

```M```

to toggle visualization mode. Right now there is default, symmetric and double symmetric.

## Compilation

Compiles with CMake. I use MSVC 2022 to build. 

Open file with MSVC 

and build

```ctrl+shift+B```

Then run with

```F5```

If you run in release mode, optimizations will be enabled and latency may be
slightly lower.
