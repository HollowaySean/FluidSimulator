#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D densTex;
uniform sampler2D tempTex;

const float maxTemp = 5000.0;
const float minTemp = 300.0;

const vec4 RED = vec4( 1.0, 0.0, 0.0, 1.0);
const vec4 BLU = vec4( 0.0, 0.0, 1.0, 1.0);

void main()
{

    float texPoint = texture(tempTex, TexCoord).x;
    float t = (texPoint - minTemp) / (maxTemp - minTemp);
    t = clamp(t, 0.0, 1.0);

    gl_FragColor = mix(BLU, RED, t);
}
