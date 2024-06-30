#version 150 core
//
// shader.vert
//

in vec4 pv;

void main(void)
{
    
    gl_Position = pv;
    
}