# Fluid Simulator
Fluid Simulation in C++ and OpenGL by Sean Holloway

For more information, see the full write-up [here.](https://blog.seanholloway.com/2021/09/09/fluid-dynamics-simulation-in-c-and-opengl/)

### Preface - April '24

Hello! Now that I'm a couple years removed from this project, I just wanted to give a quick summary with the benefit of hindsight.

This project was my introduction to OpenGL and to building a C++ of non-trivial size. As such, while it works, it is not what I would call clean, readable, efficient code, now that I've had a few years of professional experience in the field of software development. It is fairly evident when looking at the code that I really did not understand how memory management worked in C++, nor was I aware of the features and standards of modern C++. 

While I'm still immensely proud of how well this project worked given my experience at the time, I urge any peers and potential employers to not consider this reflective of my current coding standards. I would refer you instead to my newest C++ project, a helper library to simplify setting up OpenGL for my own narrow use cases, [called BasilGL](https://github.com/HollowaySean/BasilGL). As of writing, that project is still in progress, but the code quality, test coverage, and build system functionality are up to par with my current standards. In the near future, once the library is feature complete, I intend to re-implement this fluid simulation in Basil, and use it as a benchmark to measure a) the baseline performance improvement gained by using the library and b) the performance improvement gained by moving the simulation logic to the GPU.

## Overview
This project is a two-dimensional fluid dynamics simulator capable of modeling the dynamics of two mixed fluids, including convection and advection via the Navier-Stokes equations, molecular diffusion via Fick's law, thermal diffusion via Fourier's law, and both density-based and thermal buoyancy. 
The original method was based on the paper "Real-Time Fluid Dynamics for Games" (Stam, 2003), with modifications for thermal and buoyancy dynamics.
External libraries used:
- Niels Lohmann's "JSON for Modern C++"
- Omar Cornut's "Dear ImGui"
- Paul Macklin's "EasyBMP" (Video export build only)
The simulation outputs to an OpenGL-based GUI, allowing for real-time control of sources and physical parameters.

## Building
The simulator was built in Ubuntu 20.04 LTS, and currently is only set up to build for linux.

### Standard Build:

Dependencies:
- GLFW
- GLEW

The following commands can be used to install all dependencies:
> sudo apt update && sudo apt upgrade -y <br>
> sudo apt install build-essential cmake git libglfw3-dev libglew-dev -y

Build steps:
> git clone https://github.com/HollowaySean/FluidSimulator.git<br>
> cd FluidSimulator<br>
> mkdir build<br>
> cd build<br>
> cmake ..<br>
> make

### Export Build:

Dependencies:
- GLFW
- GLEW
- FFMPEG

The following commands can be used to install all dependencies:
> sudo apt update && sudo apt upgrade -y <br>
> sudo apt install build-essential cmake git libglfw3-dev libglew-dev ffmpeg -y

Build steps:
> git clone https://github.com/HollowaySean/FluidSimulator.git<br>
> cd FluidSimulator<br>
> mkdir build<br>
> cd build<br>
> cmake .. -DCMAKE_BUILD_TYPE="Record"<br>
> make
