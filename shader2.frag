#version 150 core
#extension GL_OES_standard_derivatives:enable
in vec4 fc;

//
// shader.frag
//

uniform vec2 Res;
uniform vec3 Pos;
uniform vec2 turn;
uniform float Time;
//uniform sampler2D Channel;
float iTime ;



#define iMAX 64.0
#define REACH 0.0001
#define repeat(p,r) (mod(p,r)-r/2.)
#define rot(a) mat2(cos(a),sin(a),-sin(a),cos(a))


struct RAY{
    vec3 p;
    vec3 dir;
    vec3 Ndir;
    vec2 tex;
    float obj;
    float total_d;
    vec3 tCol;
};RAY ray;

float hash(float n){return fract(sin(n*321.345)*432.123);}
float sdBox( vec3 p, vec3 b ){
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
float sdBox2( vec3 p, vec3 b ){

    vec3 q = abs(p) - b;
    float F= length(max(q.xy,0.0)) + min(max(q.x,q.y),0.0);
    float G= length(max(q.yz,0.0)) + min(max(q.y,q.z),0.0);
    float H= length(max(q.xz,0.0)) + min(max(q.x,q.z),0.0);
    return min(min(F,G),H);
    //return F;
}
#define Mod(p,a) mod(p-a/2.,a)-a/2.

float MAP(in vec3 p0){
    
    vec3 p=p0;
    p.x=Mod(p.x,13.);
    p.z=Mod(p.z,15.);
    vec2 mas = vec2(p0.xz-p.xz)+0.1;
    mas=floor(mas);
    for(float i=0.,h=(hash(mas.x+3.*mas.y))*2.0+4.; i++<h;){p.xz=p.zx;}
    
    vec3 q=p;
    q.z+=16.+(hash(mas.x+mas.y))*5.;
    
    float d=100.,d2;
    float b=iTime*0.13+(hash(mas.x+mas.y+0.1))*5.+6.;
    float a=1.;
    for(float i=0.0;i++<7.;){
    q.xz*=rot(b/a);
    q = abs(q) - 9.*a;
    d2= max(q.x,max(q.y,q.z));
    d=min(d,d2);
    a/=1.75;
    }
    float F=max(-d,p.y-10.);
    
    float T=sdBox(p-vec3(0.,0.,0.),vec3(5.,10.,5.));
    
    return max(F,T);
    
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
        t_d += d = min(MAP(ray.p),1.0);
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
vec3 GetCol(){
    return vec3(1.);
}
vec2 rotate(vec2 k,float t){
	return vec2(cos(t)*k.x-sin(t)*k.y,sin(t)*k.x+cos(t)*k.y);
}

void main(void)
{
    vec2 uv = (gl_FragCoord.xy-Res.xy*0.5)/Res.y;
    vec3 dir=normalize(vec3(uv,1.0)),
         pos=Pos;
    
    dir.yz*=rot(turn.y);
    dir.xz*=rot(turn.x);
    iTime=Time;
    ray.dir=dir;
    ray.p=pos;
    ray.obj=1.0;
    ray.tCol=vec3(0.0);
    ray.total_d=0.0;
    vec3 col=vec3(1.0);
    if( RM() ){
        float amb = 0.2*ray.Ndir.x+.3*ray.Ndir.y+.3;
        col=0.017 * vec3(1, 1, 1) * ray.total_d*amb;
        //if(ray.obj==2.0){col=texture(iChannel0,ray.tex/32.).xyz;}
    }
    else{
        col=vec3(1.0);
    }
    col=pow(col, vec3(.4545));
    gl_FragColor = vec4(col*1.4,1.0);
}