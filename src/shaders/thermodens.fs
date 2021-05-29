#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D densTex;
uniform sampler2D tempTex;

// Parameterize me
uniform float maxTemp = 5000.0;
uniform float minTemp = 300.0;
uniform float bMod = 10.0; 

void main()
{
    // Get density
    float dens = texture(densTex, TexCoord).x;

    // Convert temperature to (0, 1)
    float temp = texture(tempTex, TexCoord).x;
    float t = (temp - minTemp) / (maxTemp - minTemp);
    t = clamp(t, 0.0, 1.0);

    // Return lerped color between blue and red
    gl_FragColor = bMod * dens * vec4(t, 0.0, 1.0 - t, 1.0);
}
