// Include statements
#include "headers/Window.h"
#include "headers/Shader.h"
#include "headers/SimState.h"
#include "headers/StateLoader.h"

// Handles for OpenGL buffers and shaders
unsigned int VAO, VBO, EBO;
unsigned int cursorVBO, cursorEBO;
unsigned int * pInd;
unsigned int densTex, tempTex;
Shader* shaders;
Shader* currentShader;
static int shaderParam = 0;
int numShaders = 7;
std::string defaultJSON = "match";
bool showCursor = false;

// Window size properties
int resolution;
int winWidth;
int texWidth;
int texSize;
int controlWidth;
float bottomPos;
ImVec2 controlPos;
ImVec2 controlSize;

ShaderVars shaderVars;

// OpenGL Error Callback
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// OpenGL Resize Callback
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Save lower limit
    bottomPos = float(height);

    // Resize window with simulation centered in window
    int size = std::min(width - controlWidth, height);
    glViewport((width - controlWidth - size) / 2, (height - size) / 2, size, size);

    // Resize control panel
    controlPos = ImVec2(float(width - controlWidth), 0.0);
    controlSize = ImVec2(float(controlWidth), float(height));
}

// OpenGL input script
void ProcessInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// OpenGL key press callback
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        showCursor = !showCursor;
}

// OpenGL environment and background setup for simulation display
GLFWwindow* SimWindowSetup(int N, int windowWidth)
{
    // Save parameters
    resolution = N;
    winWidth = windowWidth;
    texWidth = N + 2;
    texSize = texWidth * texWidth;

    // Set up OpenGL state
    if(!glfwInit()){
        fprintf(stderr, "GLFW failed to init.");
    }
    glfwSetErrorCallback(ErrorCallback);

    // OpenGL initialization settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Set up main window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowWidth, "Fluid Simulator", NULL, NULL);
    if(window == NULL){ 
        fprintf(stderr, "GLFW failed to create window."); 
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    // Set up GLEW environment
    GLenum err = glewInit();
    if(GLEW_OK != err){
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    
    // Set up viewing window
    glViewport(0, 0, windowWidth, windowWidth);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, windowWidth, windowWidth);

    // Set input callback
    glfwSetKeyCallback(window, KeyCallback);

    // Set up shader
    shaders = new Shader[numShaders];
    shaders[0] = Shader("simpleVertex", "blackbodySmoke");
    shaders[1] = Shader("simpleVertex", "blackbody");
    shaders[2] = Shader("simpleVertex", "density");
    shaders[3] = Shader("simpleVertex", "thermometer");
    shaders[4] = Shader("simpleVertex", "thermodens");
    shaders[5] = Shader("simpleVertex", "blank");
    shaders[6] = Shader("simpleVertex", "cursor");

    currentShader = &(shaders[0]);

    // Create Vertex Attribute Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Calculate the limits of visible data
    float uvMin = 1.0 / float(texWidth);
    float uvMax = 1.0 - uvMin;

    // Copy vertices of full sized quad into buffer
    float vertices[] = {
        // Position             // UV coordinates
         1.0f,  1.0f, -1.0f,     uvMax, uvMax,
         1.0f, -1.0f, -1.0f,     uvMax, uvMin,
        -1.0f, -1.0f, -1.0f,     uvMin, uvMin,
        -1.0f,  1.0f, -1.0f,     uvMin, uvMax,
         1.0f,  1.0f,  0.0f,      1.0f,  0.0f,
         1.0f, -1.0f,  0.0f,      1.0f,  1.0f,
        -1.0f, -1.0f,  0.0f,      0.0f,  1.0f,
        -1.0f,  1.0f,  0.0f,      0.0f,  0.0f

    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set up vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Copy indices into element buffer
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,
        4, 5, 7,
        5, 6, 7
    };
    pInd = indices;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // Set blending properties for cursor
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glBlendEquation(GL_FUNC_ADD);

    // Set up textures for shader
    SetupTextures();

    return window;
}

// Processes to be called each frame for simulation window
void SimWindowRenderLoop(GLFWwindow* window, float* density, float* temperature)
{
    // Clear background color
    glBlendFunc(GL_ONE, GL_ZERO);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Take keyboard and mouse input
    ProcessInput(window);

    // Activate shader
    currentShader -> Use();

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texWidth, 0, GL_RED, GL_FLOAT, density);
    glBindTexture(GL_TEXTURE_2D, densTex);
    glActiveTexture(GL_TEXTURE1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texWidth,  0, GL_RED, GL_FLOAT, temperature);
    glBindTexture(GL_TEXTURE_2D, tempTex);

    // Render quad of triangles
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Render cursor
    if(showCursor)
        DrawCursor(window);

    // End of frame events
    // glfwSwapBuffers(window);
    glfwPollEvents();
}

// Draw cursor for source creation
void DrawCursor(GLFWwindow* window)
{
        // Blend rectangle on top of textured rectangle
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Activate shader and pass inmouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        shaders[6].Use();
        shaders[6].SetFloat("xPos", (float(xpos) / winWidth));
        shaders[6].SetFloat("yPos", (float(ypos) / winWidth));

        // Draw squares then reset shader
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) (6 * sizeof(unsigned int)));
        currentShader -> Use();
}

// Set up textures in OpenGL
void SetupTextures()
{

    // Blank data for initial texture (revise?)
    float blank[texSize] = {0.0f};

    // Set up density texture
    glGenTextures(1, &densTex);
    glBindTexture(GL_TEXTURE_2D, densTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texWidth, 0, GL_RED, GL_FLOAT, blank);

    // Set up temperature texture
    glGenTextures(1, &tempTex);
    glBindTexture(GL_TEXTURE_2D, tempTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texWidth, 0, GL_RED, GL_FLOAT, blank);

    // Assign texture units
    for(int i = 0; i < numShaders; i++){
        shaders[i].Use();
        glUniform1i(glGetUniformLocation(shaders[i].ID, "densTex"), 0);
        glUniform1i(glGetUniformLocation(shaders[i].ID, "tempTex"), 1);
    }
}

// Create control window for already active OpenGL environment
void ControlWindowSetup(GLFWwindow* window, int controlPanelWidth)
{
    // Save control panel size
    controlWidth = controlPanelWidth;

    // Create ImGui context in open window
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;   
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Resize window
    glfwSetWindowSize(window, winWidth + controlWidth, winWidth);
    FramebufferSizeCallback(window, winWidth + controlWidth, winWidth);
}

// Processes to be called each frame for control window
void ControlWindowRenderLoop(GLFWwindow* window, SimState* state, SimSource* source, SimTimer* timer, WindowProps* props)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set static size of window
    ImGui::SetNextWindowPos(controlPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(controlSize, ImGuiCond_Always);

    // Control panel content
    ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize);

    // Call GUI submethods
    ParameterGUI(state);
    ShaderGUI();
    ResetGUI(state, source);
    WindowGUI(state, source, props, timer);
    SourceGUI(state, source);
    FramerateGUI(timer);

    // Render ImGui frame
    ImGui::End();
    ImGui::Render();
    
    // Send ImGui rendering to OpenGL
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

// Close OpenGL and ImGui contexts
void CloseWindows(GLFWwindow* window)
{
    // End ImGui context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // End GLFW context
    glfwDestroyWindow(window);
    glfwTerminate();
}

// GUI for parameters
void ParameterGUI(SimState* state)
{
    // Scale parameter adjustment
    static int scaleParam = 0;
    ImGui::Text("Time & Length Scales:");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered()){
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(const_cast<char*>(state->params.FloatTip(scaleParam, SimParams::scale).c_str()));
        ImGui::EndTooltip(); }
    ImGui::Combo("##scalecombo", &scaleParam, "Length Scale\0Time Scale\0");
    ImGui::InputFloat("##scalebox", 
        state->params.FloatPointer(scaleParam, SimParams::scale), 
        0.001, 0.1, "%.3e");
    if(*(state->params.FloatPointer(scaleParam, SimParams::scale)) < state->params.FloatMin(scaleParam, SimParams::scale)){
        *(state->params.FloatPointer(scaleParam, SimParams::scale)) = state->params.FloatMin(scaleParam, SimParams::scale);
    }
    // ImGui::Text("");

    // Fluid parameter adjustment
    static int fluidParam = 0;
    ImGui::Text("Fluid Properties:");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered()){
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(const_cast<char*>(state->params.FloatTip(fluidParam, SimParams::fluid).c_str()));
        ImGui::EndTooltip(); }
    ImGui::Combo("##fluidcombo", &fluidParam, "Viscosity\0Molecular Diffusion\0Thermal Diffusion\0");
    ImGui::InputFloat("##fluidbox", 
        state->params.FloatPointer(fluidParam, SimParams::fluid), 
        0.000001, 0.0001, "%.3e");
    // ImGui::Text("");

    // Background parameter adjustment
    static int backgroundParam = 0;
    ImGui::Text("Background Properties:");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered()){
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(const_cast<char*>(state->params.FloatTip(backgroundParam, SimParams::background).c_str()));
        ImGui::EndTooltip(); }
    ImGui::Combo("##backgroundcombo", &backgroundParam, "Gravitational Force\0Background Density\0Mass Ratio\0Background Temperature\0");
    ImGui::InputFloat("##backgroundbox", 
        state->params.FloatPointer(backgroundParam, SimParams::background), 
        0.1, 1.0, "%.3e");
    // ImGui::Text("");

    // Decay parameter adjustment
    static int decayParam = 0;
    ImGui::Text("Decay Rates:");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered()){
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(const_cast<char*>(state->params.FloatTip(decayParam, SimParams::decay).c_str()));
        ImGui::EndTooltip(); }
    ImGui::Combo("##decaycombo", &decayParam, "Density Decay Rate\0Decay Temperature Factor\0Temperature Decay Rate\0");
    ImGui::InputFloat("##decaybox", 
        state->params.FloatPointer(decayParam, SimParams::decay), 
        1.0, 10.0, "%.3e");
    // ImGui::Text("");

    // Boolean controls
    ImGui::Checkbox("Gravity On", &(state->params.gravityOn));
    if(ImGui::Checkbox("Temperature On", &(state->params.temperatureOn)) && !(state->params.temperatureOn)){
        currentShader = &(shaders[2]);
        shaderParam = 2;
    }
    ImGui::Checkbox("Closed Boundaries", &(state->params.closedBoundaries));
    ImGui::Text("");
    ImGui::Separator();

}

// GUI for shader control
void ShaderGUI()
{
    // Shader select
    ImGui::Text("Select Shader:");
    if(ImGui::Combo("##shader", &shaderParam, "Physical\0Blackbody\0Fluid Density\0Temperature\0Fluid Temp\0None\0")){
        currentShader = &(shaders[shaderParam]);
    }

    // Shader parameter controls
    if(shaderParam < 3 || shaderParam == 4){
        ImGui::Text("Brightness:");
        ImGui::InputFloat("##brightnessSH", &(shaderVars.brightness), 0.01, 0.1);
        currentShader->SetFloat("bMod", shaderVars.brightness);
    }
    if(shaderParam == 0){
        ImGui::Text("Fluid Brightness:");
        ImGui::InputFloat("##fluidbrightnessSH", &(shaderVars.brightnessFluid), 0.01, 0.1);
        currentShader->SetFloat("sMod", shaderVars.brightnessFluid);
    }
    if(shaderParam == 3 || shaderParam == 4){
        ImGui::Text("Max Temperature:");
        ImGui::InputFloat("##maxTempSH", &(shaderVars.tempHigh), 1.0, 10.0);
        currentShader->SetFloat("maxTemp", shaderVars.tempHigh);
        ImGui::Text("Min Temperature:");
        ImGui::InputFloat("##minTempSH", &(shaderVars.tempLow), 1.0, 10.0);
        currentShader->SetFloat("minTemp", shaderVars.tempLow);
    }

    static bool smoothingOn = true;
    if(ImGui::Checkbox("Smooth Image", &smoothingOn)){

        // Change filtering mode of texture
        if(smoothingOn){
            glBindTexture(GL_TEXTURE_2D, densTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, densTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }else{
            glBindTexture(GL_TEXTURE_2D, densTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, densTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        // Clear color to avoid flickering
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    ImGui::Text("");
    ImGui::Separator();
}

// GUI for reset functions
void ResetGUI(SimState* state, SimSource* source)
{
    // Reset fields
    ImGui::Text("Reset:");
    if(ImGui::Button("State", ImVec2(80.0, 20.0))){
        state->ResetState();
        source->UpdateSources();
    }
    ImGui::SameLine();
    if(ImGui::Button("Parameters", ImVec2(80.0, 20.0))){
        LoadParameters(const_cast<char*>(defaultJSON.c_str()), &(state->params));
    }
    if(ImGui::Button("Sources", ImVec2(80.0, 20.0))){
        source->RemoveAllSources();
        LoadSources(const_cast<char*>(defaultJSON.c_str()), source);
    }
    ImGui::SameLine();
    if(ImGui::Button("All", ImVec2(80.0, 20.0))){
        source->RemoveAllSources();
        state->ResetState();
        LoadState(const_cast<char*>(defaultJSON.c_str()), state, source);
    }

    // Load preset JSON files
    static int preset = 0;
    if(ImGui::Button("Load Preset: ", ImVec2(170.0, 20.0))){
        switch(preset){
            case 0:
                defaultJSON = "match";
                break;
            case 1:
                defaultJSON = "fog";
                break;
        }
        source->RemoveAllSources();
        state->ResetState();
        LoadState(const_cast<char*>(defaultJSON.c_str()), state, source);
    }
    ImGui::Combo("##preset", &preset, "Match\0Fog\0");

    ImGui::Text("");
    ImGui::Separator();
}

// GUI for window control
void WindowGUI(SimState* state, SimSource* source, WindowProps* props, SimTimer* timer)
{
    ImGui::Text("Simulation Resolution:");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered()){
        ImGui::BeginTooltip();
        ImGui::TextUnformatted("Resizes the simulation grid to N x N, where N is the resolution value");
        ImGui::EndTooltip(); }
    ImGui::InputInt("##N", &resolution);
    if(ImGui::Button("Resize", ImVec2(80.0, 20.0))){

        // Resize shader texture sizes
        texWidth = resolution + 2;
        texSize = texWidth * texWidth;

        // Calculate the limits of visible data
        float uvMin = 1.0 / float(texWidth);
        float uvMax = 1.0 - uvMin;

        // Copy vertices of full sized quad into buffer
        float vertices[] = {
            // Position             // UV coordinates
            1.0f,  1.0f, 0.0f,     uvMax, uvMax,
            1.0f, -1.0f, 0.0f,     uvMax, uvMin,
            -1.0f, -1.0f, 0.0f,     uvMin, uvMin,
            -1.0f,  1.0f, 0.0f,     uvMin, uvMax
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Resize simulation objects
        state -> ResizeGrid(resolution);
        source -> Reset();
        LoadSources(const_cast<char*>(defaultJSON.c_str()), source);
    }

    ImGui::Text("Framerate Cap:");
    ImGui::InputInt("##fps", &(props -> maxFrameRate));
    if(ImGui::Button("Set Cap")){
        if(props -> maxFrameRate > 0){
            timer -> SetFrameRate(props -> maxFrameRate);
            timer -> StartSimulation();
        }else{
            props -> maxFrameRate = timer -> MaxFrameRate();
        }
    }

    ImGui::Text("Solver Steps:");
    ImGui::InputInt("##solvesteps", &(state -> params.solverSteps));

    ImGui::Text("");
    ImGui::Separator();
}

// GUI for source control
void SourceGUI(SimState* state, SimSource* source)
{

}

// GUI for framerate functions
void FramerateGUI(SimTimer* timer)
{
    // FPS readout
    ImGui::Text("Current FPS: %f \nAverage FPS: %f", timer->CurrentFrameRate(), timer->AverageFrameRate());
}