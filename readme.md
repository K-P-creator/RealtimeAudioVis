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

The goal of this project is to create a cool, customizable audio visualization tool that runs with super low latency. C++ is the perfect tool for the job. The kissFFT library I use is also super fast and written in C. The slowest part of the visualization is the drawing sequence with SFML. 

I have plans to create a more user friendly expierience and I may add a settings menu as well as other additional display modes. I think some different approaches to graphing would be cool, such as a circular graph and maybe some special effects like a video background. 

## Usage

After executing the .exe, use

```esc```

to toggle fullscreen mode. Note that there is a 3s cooldown for switching window modes.

Use:

```M```

to toggle visualization mode. Right now there is default, symmetric double symmetric, and double sym with smoothing (my favorite).

Use:

```C```

to toggle color modes. Theres a whole bunch of color modes and most of them scale with the x and y values for the bar graphs. Pretty cool.

## Compilation

Compiles with CMake. I use MSVC 2022 to build. 

Open file with MSVC 

and build

```ctrl+shift+B```

Then run with

```F5```

If you run in release mode, optimizations will be enabled and latency may be
slightly lower.
