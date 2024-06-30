#version 150
in vec4 fc;

//
// shader.frag
//

uniform vec2 Res;
uniform vec3 Pos;
uniform vec2 turn;
uniform float buffersize;
uniform sampler2D Channel;


void main(void)
{
	vec2 uv2 = gl_FragCoord.xy / Res;
	vec3 col = texture(Channel, uv2).xyz;
	gl_FragColor = vec4(col , 1.0);
}