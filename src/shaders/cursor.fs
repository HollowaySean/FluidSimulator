#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform float xPos;
uniform float yPos;

void main()
{
    // Illuminate area near cursor

    if((abs(TexCoord.x - xPos) < 0.1) && (abs(TexCoord.y - yPos) < 0.1)){
        gl_FragColor = vec4(1.0, 0.0, 0.0, 0.1);
    }else{
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}

