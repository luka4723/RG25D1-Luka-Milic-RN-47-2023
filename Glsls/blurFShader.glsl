#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screen;
uniform vec2 vec;
const int sampleCount = 20;

void main()
{
    vec4 sum = vec4(0.0); // analogno rc,gc,bc
    vec2 texelSize = vec2(1.0 /(9*32*3), 1.0 / (6*32*3));

    for(float i=0;i<20;i++)
    {
        float scalar = i/20.0;
        vec2 offset = vec*scalar*texelSize;
        sum += texture(screen, TexCoords + offset);
    }
    sum /= 20.0;

    FragColor = sum;
}
