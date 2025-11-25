#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec2 ScreenPos;

uniform sampler2D sheet;
uniform vec4 filterColor1;
uniform vec4 filterColor2;
uniform vec4 filterColor3;
uniform vec4 filterColor4;
uniform float alfa;

void main()
{
    vec4 texColor = texture(sheet,TexCoords);
    if (texColor.a < 0.1) {
        discard;
    }
    vec4 topColor = mix(filterColor1, filterColor2, ScreenPos.x);
    vec4 bottomColor = mix(filterColor3, filterColor4, ScreenPos.x);
    vec4 filterColor = mix(bottomColor, topColor, ScreenPos.y);

    FragColor = mix(texColor, filterColor, alfa);
}
