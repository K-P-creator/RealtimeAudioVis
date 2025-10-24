#version 330 core
out vec4 FragColor;
in vec4 fragmentColor;

uniform vec4 BaseColor;

void main()
{
    FragColor = BaseColor;
}