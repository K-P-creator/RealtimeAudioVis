#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform int BarCount;


void build_bar(vec4 position)
{    //TL, BL, TR, BR
    gl_Position = vec4(position.x, position.y - 1.0f, position.z, position.w);    //  top left
    EmitVertex();   
    gl_Position = vec4(position.x, -1.0f, position.z, position.w);    //  bottom left
    EmitVertex();
    gl_Position = vec4(position.x + 1.0f/float(BarCount), position.y - 1.0f, position.z, position.w);    //  top right
    EmitVertex();
    gl_Position = vec4(position.x + 1.0f/float(BarCount), -1.0f, position.z, position.w );    //  bottom right
    EmitVertex();

    EndPrimitive();
}

void main() {    
    build_bar(gl_in[0].gl_Position);
} 