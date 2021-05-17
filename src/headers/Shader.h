/* Custom class to assist in using shaders      */
/* Taken nearly verbatim from learnopengl.com   */

// Preprocessor statements
#ifndef SHADER_H
#define SHADER_H

// Include statements
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Class to read in shaders from file
class Shader
{
    public:
        
        // ID int
        unsigned int ID;

        // Constructor
        Shader();
        Shader(const char* vertexPath, const char* fragmentPath);

        // Activate shader
        void Use();

        // Utility functions
        void SetBool(   const std::string &name, bool  value) const;
        void SetInt(    const std::string &name, int   value) const;
        void SetFloat(  const std::string &name, float value) const;
};

// Closing preprocessor statement
#endif