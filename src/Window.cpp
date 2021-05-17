// Include statements
#include "headers/Window.h"
#include "headers/Shader.h"

// Handles for OpenGL buffers and shaders
unsigned int VAO, EBO;
unsigned int densTex, tempTex;
Shader* shader;

// Window size properties
int N;
int size;
int cellSize;

// OpenGL Error Callback
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// OpenGL Resize Callback
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// OpenGL input script
void ProcessInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// OpenGL environment and background setup
GLFWwindow* WindowSetup(int N_in, int cellSize_in)
{
    // Save parameters
    N = N_in + 2;
    size = N * N;
    cellSize = cellSize_in;

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
    int xSize = (N + 2) * cellSize;
    int ySize = xSize;
    GLFWwindow* window = glfwCreateWindow(xSize, ySize, "Fluid Simulator", NULL, NULL);
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
    glViewport(0, 0, xSize, ySize);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Set up shader
    shader = new Shader(
        "./src/shaders/simpleVertex.vs", 
        "./src/shaders/blackbody.fs");

    // Create Vertex Attribute Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Copy vertices of full sized quad into buffer
    float vertices[] = {
        // Position             // UV coordinates
         1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f
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
    float blank[size] = {0.0f};

    // Set up density texture
    glGenTextures(1, &densTex);
    glBindTexture(GL_TEXTURE_2D, densTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, N, N, 0, GL_RED, GL_FLOAT, blank);

    // Set up temperature texture
    glGenTextures(1, &tempTex);
    glBindTexture(GL_TEXTURE_2D, tempTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, N, N, 0, GL_RED, GL_FLOAT, blank);

    // Assign texture units
    shader -> Use();
    glUniform1i(glGetUniformLocation(shader->ID, "densTex"), 0);
    glUniform1i(glGetUniformLocation(shader->ID, "densTex"), 0);

    return window;
}

void WindowRenderLoop(GLFWwindow* window, float* density, float* temperature)
{
    // Take keyboard and mouse input
    ProcessInput(window);
    // Activate shader
    shader -> Use();

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, N, N, 0, GL_RED, GL_FLOAT, density);
    glBindTexture(GL_TEXTURE_2D, densTex);
    glActiveTexture(GL_TEXTURE1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, N, N, 0, GL_RED, GL_FLOAT, temperature);
    glBindTexture(GL_TEXTURE_2D, tempTex);

    // Render quad of triangles
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // End of frame events
    glfwSwapBuffers(window);
    glfwPollEvents();
}