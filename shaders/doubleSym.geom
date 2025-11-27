#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 8) out;	// Output two mirrored bars

uniform int BarCount;

//		Leftmost bar, will be in the x < 0 range, righmost in x > 0
void build_bar(vec4 position)
{	////TL1, BL1, TR1, BR1 
	//	bar1 is the rightmost
	gl_Position = vec4(0.5f + position.x/2, position.y, position.z, position.w);	//TL
	EmitVertex();
	gl_Position = vec4(0.5f + position.x/2, -position.y, position.z, position.w);		//BL
	EmitVertex();
	gl_Position = vec4(0.5f + position.x/2 + 0.5f/float(BarCount), position.y, position.z, position.w);		//TR
	EmitVertex();
	gl_Position = vec4(0.5f + position.x/2 + 0.5f/float(BarCount), -position.y, position.z, position.w);		// BR
	EmitVertex();

	EndPrimitive();

	//	Bar2 is the leftmost
	gl_Position = vec4(-0.5f - position.x/2, position.y, position.z, position.w);	//TL
	EmitVertex();
	gl_Position = vec4(-0.5f - position.x/2, -position.y, position.z, position.w);		//BL
	EmitVertex();
	gl_Position = vec4(-0.5f - position.x/2 + 0.5f/float(BarCount), position.y, position.z, position.w);		//TR
	EmitVertex();
	gl_Position = vec4(-0.5f - position.x/2 + 0.5f/float(BarCount), -position.y, position.z, position.w);		// BR
	EmitVertex();

	EndPrimitive();
}

void main(){
	build_bar(gl_in[0].gl_Position);
}