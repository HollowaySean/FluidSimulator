#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D densTex;
uniform sampler2D tempTex;
uniform float bMod = 500.0;
uniform float sbConstant = 1.0;

void main()
{
    float temp = texture(tempTex, TexCoord).x;
    float intensity = bMod * sbConstant * temp * temp * temp * temp;
    intensity = min(intensity, 1.0f);

    float red = 1.0f;
    float blue = temp * (1.0 - 0.5765) + 0.5765;
    float green = temp * (1.0 - 0.1608) + 0.1608;

    FragColor = intensity * vec4(red, blue, green, 1.0);
}