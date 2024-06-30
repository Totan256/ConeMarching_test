#version 150
in vec4 fc;

//
// shader.frag
//

uniform vec2 Res;
uniform vec3 Pos;
uniform vec2 turn;
uniform sampler2D Channel;

float time=0.0;

#define iMAX 90.0
#define REACH 0.0001
#define H(h)(cos((h)*6.3+vec3(0,23,21))*.5+.5)
#define rot(s) mat2(cos(s),-sin(s),sin(s),cos(s))

struct RAY{
    vec3 p;
    vec3 dir;
    vec3 Ndir;
    vec2 tex;
    float obj;
    float total_d;
    vec3 tCol;
};RAY ray;

float sdBox( vec3 p, vec3 b ){
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float MAP(in vec3 q){
    
    vec3 p = mod(q,6.)-3.;
    
    return sdBox(p,vec3(.5));
}

vec3 getNormal(float d){
    //vec3 p=ray.p;
    const vec2 e = vec2(0.001,0.0);
    #if 0
    return normalize(
        vec3(MAP(ray.p + e.xyy) - MAP(ray.p - e.xyy),
             MAP(ray.p + e.yxy) - MAP(ray.p - e.yxy),
             MAP(ray.p + e.yyx) - MAP(ray.p - e.yyx)));
    #else   
    return normalize(
        vec3(MAP(ray.p + e.xyy),
             MAP(ray.p + e.yxy),
             MAP(ray.p + e.yyx) )-MAP(ray.p));
    #endif
            
}
vec3 getReflect(vec3 dir,vec3 n){
    //vec3 n = getNormal(d);
    return reflect(dir,n);
}

bool RM(){
    vec3 dir = ray.dir;
    for(float t_d=0.0,d,i=0.0;i++<iMAX;){
        t_d += d = min(MAP(ray.p),30.);
        ray.p+=dir*d;
        if(d<REACH){
            ray.Ndir=getNormal(d);
            if(ray.obj<5.0){
                ray.total_d=t_d;
                return true;
            }
            dir = getReflect(dir,ray.Ndir);
        }
    }
    return false;
}
vec2 rotate(vec2 k,float t){
	return vec2(cos(t)*k.x-sin(t)*k.y,sin(t)*k.x+cos(t)*k.y);
}
void GetDir(inout vec3 dir){
    //dir.xy *= rot(turn.x);
    dir.zy *= rot(turn.y);
}
void main(void)
{
    vec2 uv = (gl_FragCoord.xy-Res.xy*0.5)/Res.y;
    vec3 dir=normalize(vec3(uv,3.0)),
         pos=Pos;
    //GetDir(dir);
    dir.zy*=rot(turn.y);
    dir.zx*=rot(turn.x);
    ray.dir=dir;
    ray.p=pos;
    ray.obj=1.0;
    ray.tCol=vec3(0.0);
    ray.total_d=0.0;
    vec3 col=vec3(1.0);
    if( RM() ){
        float amb = 0.4+0.7*ray.Ndir.y;
        col=0.017 * vec3(1, 1, 1.5) * ray.total_d*amb;
    }
    else{
        col=vec3(1.0);
    }
    
    
    
    //col=texture(Channel,uv2).xyz;
    gl_FragColor = vec4(col*1.4,1.0);
}