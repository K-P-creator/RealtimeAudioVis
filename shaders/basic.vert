#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aColor;

out vec4 fragmentColor;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    fragmentColor = vec4(aColor.x, aColor.y, aColor.z, 1.0);
}