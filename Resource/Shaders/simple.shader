#vertex shader
#version 450 core

layout (location = 0) in vec4 position;


out Fragment
{
	vec4 color;
} fragment;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main(void)
{
	vec2 pos[4] = vec2[4](
		vec2(-1.0, -1.0),
		vec2(1.0, -1.0),
		vec2(-1.0, 1.0),
		vec2(1.0, 1.0));

	gl_Position = projection * view * model * vec4(pos[gl_VertexID], 0.0, 1.0);

	/*gl_Position = projection * view * model * position;*/
	fragment.color = vec4(1.0);

}

#fragment shader
#version 450 core

in Fragment
{
	vec4 color;
} fragment;

out vec4 color;

void main(void)
{
	color = fragment.color;
}
