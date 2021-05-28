#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D densTex;
uniform sampler2D tempTex;
uniform float bMod = 1.0;
uniform float sbConstant = 0.0000000000001;
uniform float smokePoint = 2000.0;

void main()
{
    float temp = texture(tempTex, TexCoord).x;
    float dens = texture(densTex, TexCoord).x;
    dens = clamp(dens, 0.0, 1.0);
    float intensity = dens * bMod * sbConstant * temp * temp * temp * temp;
    intensity = min(intensity, 1.0f);


    float t = (temp - 1900.0) / (5400.0 - 1900.0);
    float red = 1.0;
    float green = (147.0 + (255.0 - 147.0) * t) / 255.0;
    float blue = (41.0 + (251.0 - 41.0) * t) / 255.0;

    float bright = 0.21 + 0.72 * green + 0.07 * blue;

    if(temp > smokePoint && bright > dens * bMod){
        gl_FragColor = intensity * vec4(red, green, blue, 1.0);
    }else{
        gl_FragColor = dens * bMod * vec4(1.0, 1.0, 1.0, 1.0);
    }


}