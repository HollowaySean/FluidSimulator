#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D densTex;
uniform sampler2D tempTex;
uniform float bMod = 0.00000000000005;
uniform float sbConstant = 1.0;

void main()
{
    float temp = texture(tempTex, TexCoord).x;
    float dens = texture(densTex, TexCoord).x;
    dens = clamp(dens, 0.0, 1.0);
    float intensity = dens * bMod * sbConstant * temp * temp * temp * temp;
    intensity = min(intensity, 1.0f);

    float t = (temp - 1900.0) / (5200.0 - 1900.0);

    float red = 1.0f;
    float blue = t * (1.0 - 0.5765) + 0.5765;
    float green = t * (1.0 - 0.1608) + 0.1608;

    FragColor = intensity * vec4(red, blue, green, 1.0);
}