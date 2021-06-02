// Pre-processor include statements
#include <iostream>
#include <cmath>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <nlohmann/json.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <EasyBMP/EasyBMP.h>
#include <EasyBMP/EasyBMP_BMP.h>
#include <EasyBMP/EasyBMP_DataStructures.h>
#include <EasyBMP/EasyBMP_VariousBMPutilities.h>

using namespace ImGui;

// Project header files
#include "../headers/SimSource.h"
#include "../headers/SimState.h"
#include "../headers/SimTimer.h"
#include "../headers/Window.h"
#include "../headers/StateLoader.h"

// Global variables
std::string projectPath;

int main(int argc, char** argv){

    // Use first argument as filename
    std::string filename = argv[1];

    // Get project path
    std::string fullpath = argv[0];
    projectPath = fullpath.substr(0, fullpath.find_last_of("/")) + "/..";

    // Initialize state objects
    WindowProps props;
    LoadRecord("record", &props);
    int winWidth = props.winWidth;
    SimState state(props.resolution);
    SimSource sources(&state);
    LoadState("record", &state, &sources);

    // Set up simulation and control windows
    GLFWwindow* window = SimWindowSetup(props.resolution, props.winWidth);

    // Initialize timers
    SimTimer timer(props.fps);
    timer.StartSimulation();

    // Create array for image data
    BMP output;
    output.SetSize(winWidth, winWidth);
    output.SetBitDepth(24);
    unsigned char* imageData = (unsigned char *)malloc(sizeof(unsigned char) * (winWidth*winWidth*3));

    // Simulation loop
    while(!glfwWindowShouldClose(window))
    {
        // Declare beginning of frame
        timer.StartFrame();

        // Draw current density to OpenGL window
        SimWindowRenderLoop(window, state.fields.dens, state.fields.temp);
        glfwSwapBuffers(window);

        // Update dynamic sources
        sources.UpdateSourcesDynamic();

        // Update simulation state
        state.SimulationStep(1.0 / float(props.fps));

        // Save openGl data to images
        glReadPixels(0, 0, winWidth, winWidth, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        for(int i = 0; i < winWidth; i++){
            for(int j = 0; j < winWidth; j++){

                int ind = 3 * ((i * winWidth) + j);
                RGBApixel pixelColor;
                pixelColor.Red      = imageData[ind];
                pixelColor.Green    = imageData[ind+1];
                pixelColor.Blue     = imageData[ind+2];
                

                *output(j, winWidth - (i + 1)) = pixelColor;
            }
        }
        std::string savePath = projectPath + "/output/bmp/" + filename + "_" + std::to_string(timer.CurrentFrame()) + ".bmp";
        output.WriteToFile(const_cast<char *>(savePath.c_str()));

        // Read out frame rate per every 
        timer.DisplayFrameRate(int(props.fps));

        if(timer.CurrentFrame() % int(props.fps) == 0){
            std::cout << timer.CurrentFrame() << " frames complete out of " << props.numFrames << std::endl;
        }

        // Escape once complete
        if(timer.CurrentFrame() >= props.numFrames){
            break;
        }
    }

    // End GLFW context
    glfwDestroyWindow(window);
    glfwTerminate();

    // Exit code
    return 0;
}