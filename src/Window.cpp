// Include statements
#include "headers/Window.h"
#include "headers/Shader.h"
#include "headers/SimState.h"

// Handles for OpenGL buffers and shaders
unsigned int VAO, EBO;
unsigned int densTex, tempTex;
Shader* shader;

// Window size properties
int resolution;
int winWidth;
int texWidth;
int texSize;

// OpenGL Error Callback
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// OpenGL Resize Callback
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // TODO: WORK ON THIS
    int size = std::min(width, height);
    glViewport(0, height - size, size, size);

    // TODO: RESIZE/POSITION CONTROL PANEL
}

// OpenGL input script
void ProcessInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    
    // Set up viewing window
    glViewport(0, 0, windowWidth, windowWidth);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Set up shader
    shader = new Shader("simpleVertex", "blackbody");

    // Create Vertex Attribute Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

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
    unsigned int VBO;
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
        1, 2, 3
    };
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texWidth, texWidth, 0, GL_RED, GL_FLOAT, blank);

    // Set up temperature texture
    glGenTextures(1, &tempTex);
    glBindTexture(GL_TEXTURE_2D, tempTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texWidth, texWidth, 0, GL_RED, GL_FLOAT, blank);

    // Assign texture units
    shader -> Use();
    glUniform1i(glGetUniformLocation(shader->ID, "densTex"), 0);
    glUniform1i(glGetUniformLocation(shader->ID, "densTex"), 0);

    return window;
}

// Processes to be called each frame for simulation window
void SimWindowRenderLoop(GLFWwindow* window, float* density, float* temperature)
{
    // Take keyboard and mouse input
    ProcessInput(window);

    // Activate shader
    shader -> Use();

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texWidth, texWidth, 0, GL_RED, GL_FLOAT, density);
    glBindTexture(GL_TEXTURE_2D, densTex);
    glActiveTexture(GL_TEXTURE1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texWidth, texWidth,  0, GL_RED, GL_FLOAT, temperature);
    glBindTexture(GL_TEXTURE_2D, tempTex);

    // Render quad of triangles
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // End of frame events
    //glfwSwapBuffers(window);
    glfwPollEvents();
}

// Create control window for already active OpenGL environment
void ControlWindowSetup(GLFWwindow* window)
{
    // Create ImGui context in open window
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;   
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

// Processes to be called each frame for control window
void ControlWindowRenderLoop(GLFWwindow* window, SimParams* params)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set static size of window
    ImGui::SetNextWindowPos(ImVec2(900.0, 0.0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(120.0, 1020.0), ImGuiCond_Once);

    // Control panel content
    ImGui::Begin("Window", NULL, ImGuiWindowFlags_NoResize);
    ImGui::Text("Stuff goes here!");

    // Render ImGui frame
    ImGui::End();
    ImGui::Render();
    
    // Send ImGui rendering to OpenGL
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

}
