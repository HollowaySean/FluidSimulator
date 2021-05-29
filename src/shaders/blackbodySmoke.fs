#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D densTex;
uniform sampler2D tempTex;

// These need to be parameterized
uniform float bMod = 1.0;
uniform float sMod = 1.0;
uniform float sbConstant = 0.000000000001;

void main()
{
    // Read texutres
    float temp = texture(tempTex, TexCoord).x;
    float dens = texture(densTex, TexCoord).x;

    // Blackbody intensity
    float intensity = sbConstant * temp * temp * temp * temp;

    // BLackbody color temperature
    float t = (temp - 1900.0) / (5400.0 - 1900.0);
    float red = 1.0;
    float green = (147.0 + (255.0 - 147.0) * t) / 255.0;
    float blue = (41.0 + (251.0 - 41.0) * t) / 255.0;

    // Final color as alpha mix of blackbody and density
    gl_FragColor = dens * bMod * vec4(sMod + red * intensity, sMod + green * intensity, sMod + blue * intensity, 1.0);


}