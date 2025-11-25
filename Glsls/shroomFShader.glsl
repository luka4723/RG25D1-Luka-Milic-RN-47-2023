#version 330 core

uniform vec4 filterColor; // RGB + alpha za transparentnost
out vec4 FragColor;

void main()
{
    FragColor = filterColor;
}