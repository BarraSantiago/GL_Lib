#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 col;

out vec4 ourColor;

uniform mat4 u_Transform;

void main()
{
	gl_Position = u_Transform * position;
	ourColor = col;
}

#shader fragment
#version 330 core

out vec4 fragColor;

in vec4 ourColor;
uniform vec4 u_Color;

void main()
{
	fragColor = ourColor * u_Color;
};