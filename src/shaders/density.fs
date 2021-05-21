#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D densTex;
uniform sampler2D tempTex;
uniform float bMod = 1.0;

void main()
{
    float texPoint = bMod * texture(densTex, TexCoord).x;
    gl_FragColor = vec4(texPoint, texPoint, texPoint, 1.0);
}
