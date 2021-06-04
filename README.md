# Fluid Simulator
Fluid Simulation in C++ and OpenGL by Sean Holloway

For more information, see the full write-up [here.](https://blog.seanholloway.com/2021/06/03/fluid-dynamics-simulation-in-c-with-opengl/)

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
