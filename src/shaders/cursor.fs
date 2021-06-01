#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform int shape;
uniform float xPos;
uniform float yPos;
uniform float size;

void main()
{
    float xDist = abs(xPos - TexCoord.x);
    float yDist = abs(yPos - TexCoord.y);

    float dist;
    vec4 color;
    if(shape == 0){
        dist = max(xDist, yDist);
    }else if(shape == 1){
        dist = sqrt(xDist * xDist + yDist * yDist);
    }else{
        dist = xDist + yDist;
    }

    // Illuminate area near cursor
    if(dist < size){
        gl_FragColor = vec4(1.0, 0.0, 0.0, 0.1);
    }else{
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}

