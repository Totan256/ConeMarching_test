#version 150
in vec4 fc;

//
// Buffer.frag
//

uniform vec2 Res;

void main(void){
	vec2 uv = (gl_FragCoord.xy-Res.xy*0.5)/Res.y;
	vec3 col = vec3(0.0);
	col.x=fract(sin(uv.x+uv.y*32.)*4343.6387);
	col.y=fract(sin(-uv.x*92.+uv.y*62.)*7234.86);
	col.z=fract(sin(uv.x*uv.y*9876.)*8343.6387);
	gl_FragColor = vec4(col,0.0);
}