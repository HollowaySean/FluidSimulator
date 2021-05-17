// Include statements
#include "headers/Window.h"

// Handles for OpenGL buffers and shaders
unsigned int VAO;
unsigned int EBO;
unsigned int shaderProgram;

// Shader definitions
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";


// Callbacks for OpenGL

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
GLFWwindow* WindowSetup(int xSize, int ySize)
{
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

    // Vertex shader setup
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment shader setup
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Shader program setup
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Activate program and delete shaders
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create Vertex Attribute Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Copy vertices of full sized quad into buffer
    float vertices[] = {
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Copy indices into element buffer
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set up vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set up texture mapping
    float texCoords[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    

    return window;
}

void WindowRenderLoop(GLFWwindow* window)
{
    // Take keyboard and mouse input
    ProcessInput(window);

    // Draw triangles for background
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // End of frame events
    glfwSwapBuffers(window);
    glfwPollEvents();
}