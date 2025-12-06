#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
uniform int rotation;
void main()
{
    if(rotation == 0) gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    else if (rotation == 1)  gl_Position = vec4(aPos.y, -aPos.x, 0.0, 1.0);
    else if (rotation == 2)  gl_Position = vec4(-aPos.x, -aPos.y, 0.0, 1.0);
    else if (rotation == 3)  gl_Position = vec4(-aPos.y, aPos.x, 0.0, 1.0);
    TexCoords = aTexCoords;
}