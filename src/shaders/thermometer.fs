#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D densTex;
uniform sampler2D tempTex;

// Input parameters
uniform float maxTemp = 1000.0;
uniform float minTemp = 300.0;

void main()
{

    // Convert temperature to (0, 1)
    float texPoint = texture(tempTex, TexCoord).x;
    float t = (texPoint - minTemp) / (maxTemp - minTemp);
    t = clamp(t, 0.0, 1.0);

    // Return lerped color between blue and red
    gl_FragColor = vec4(t, 0.0, 1.0 - t, 1.0);
}
