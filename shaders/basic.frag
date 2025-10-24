#version 330 core
out vec4 FragColor;
in vec4 fragmentColor;

void main()
{
    FragColor = fragmentColor;
}