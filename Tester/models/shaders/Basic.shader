#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 col;
layout(location = 2) in vec2 textCoord;

out vec4 ourColor;
out vec2 texCoord;

uniform mat4 u_Transform;

void main()
{
	gl_Position = u_Transform * position;
	ourColor = col;
	texCoord = textCoord;
}

#shader fragment
#version 330 core

out vec4 fragColor;

in vec4 ourColor;
in vec2 texCoord;
uniform vec4 u_Color;

uniform sampler2D ourTexture;

void main()
{
	fragColor = texture(ourTexture, texCoord) * ourColor * u_Color;
};