# Realtime Audio Vis

## Info

**WINDOWS ONLY**

Relies on Windows WASAPI to sample sound info from the default audio device. 
This way you can run the .exe and play any audio you want, including encrypted 
downloads on your favorite music streaming app.

STD = C++20

SFML Version 3.0.1 for graphics

- I created a custom sf::Curve class for drawing curves. I will add a ComplexCurve class eventually for better looking curve type graphs.

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

to toggle visualization mode. Right now there is default, symmetric double symmetric, double sym with smoothing (my favorite), and curve with smoothing.

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

Heres a short demo of some color modes and the double sym smoothed and the curve modes.




https://github.com/user-attachments/assets/17080c58-95d2-49f5-8764-724c7a3d2338



