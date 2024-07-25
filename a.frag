#version 330 core

in vec2 texcoord;

uniform sampler2D sampler;

out vec4 col;

void main() {
	col = texture(sampler, texcoord);
}
