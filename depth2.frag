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


float time = 0.0;

#define iMAX 30.0
#define REACH 0.001
#define H(h)(cos((h)*6.3+vec3(0,23,21))*.5+.5)
#define rot(s) mat2(cos(s),-sin(s),sin(s),cos(s))

#define repeat(p,r) (mod(p,r)-r/2.)
#define Mod(p,a) mod(p-a/2.,a)-a/2.

struct RAY {
    vec3 p;
    vec3 dir;
    vec3 Ndir;
    vec2 tex;
    float obj;
    float total_d;
    vec3 tCol;
}; RAY ray;

float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}
float hash(float n) { return fract(sin(n) * 432.123); }
float MAP(in vec3 p0) {
#if 0
    vec3 p = mod(p0, 6.) - 3.;

    return sdBox(p, vec3(.5));
#else

    vec3 p = p0;
    p.x = Mod(p.x, 13.);
    p.z = Mod(p.z, 15.);
    vec2 mas = vec2(p0.xz - p.xz) + 0.1;
    mas = floor(mas);
    for (float i = 0., h = (hash(mas.x + 3. * mas.y)) * 2.0 + 4.; i++ < h;) { p.xz = p.zx; }

    vec3 q = p;
    q.z += 16. + (hash(mas.x + mas.y)) * 5.;

    float d = 100., d2;

    float b = (hash(mas.x + mas.y + 0.1)) * 5. + 6.;
    float a = 1.;
    for (float i = 0.0; i++ < 7.;) {
        q.xz *= rot(b / a);
        q = abs(q) - 9. * a;
        d2 = max(q.x, max(q.y, q.z));
        d = min(d, d2);
        a /= 1.75;
    }
    float F = max(-d, p.y - 10.);

    float T = sdBox(p - vec3(0., 0., 0.), vec3(5., 1. + (hash(mas.x + mas.y + 0.1)) * 10. + 5., 5.));

    return max(F, T);
#endif
}

vec3 getNormal(float d) {
    //vec3 p=ray.p;
    const vec2 e = vec2(0.001, 0.0);
#if 0
    return normalize(
        vec3(MAP(ray.p + e.xyy) - MAP(ray.p - e.xyy),
            MAP(ray.p + e.yxy) - MAP(ray.p - e.yxy),
            MAP(ray.p + e.yyx) - MAP(ray.p - e.yyx)));
#else   
    return normalize(
        vec3(MAP(ray.p + e.xyy),
            MAP(ray.p + e.yxy),
            MAP(ray.p + e.yyx)) - MAP(ray.p));
#endif

}
vec3 getReflect(vec3 dir, vec3 n) {
    //vec3 n = getNormal(d);
    return reflect(dir, n);
}

bool RM(float first_d) {
    vec3 dir = ray.dir;
    ray.p += dir * first_d;
    float t_d = first_d;
    if (MAP(ray.p) < REACH) {
        ray.total_d = t_d;
        return true;
    }
    for (float d, i = 0.0; i++ < iMAX;) {
        t_d += d = min(MAP(ray.p), 3.);
        ray.p += dir * d;
        if (d < REACH) {
            ray.Ndir = getNormal(d);
            if (ray.obj < 5.0) {
                ray.total_d = t_d;
                return true;
            }
            dir = getReflect(dir, ray.Ndir);
        }
    }
    ray.total_d = t_d;
    return false;
}
vec2 rotate(vec2 k, float t) {
    return vec2(cos(t) * k.x - sin(t) * k.y, sin(t) * k.x + cos(t) * k.y);
}
void main(void)
{
    vec2 uv = (gl_FragCoord.xy - Res.xy * 0.5) / Res.y;
    vec2 uv2 = gl_FragCoord.xy / Res/4.;
    vec3 dir = normalize(vec3(uv, 3.0)),
        pos = Pos;
    //GetDir(dir);


    float depth = (texture(Channel, uv2).x);
    //depth = 0.;

    //if (buffersize == 2.0)pos = texture(Channel, uv2).xyz;
    dir.zy *= rot(turn.y);
    dir.zx *= rot(turn.x);
    ray.dir = dir;
    ray.p = pos;
    ray.obj = 1.0;
    ray.tCol = vec3(0.0);
    ray.total_d = 0.0;
    vec3 col = vec3(1.0);
    if (RM(depth)) {
        float amb = 0.4 + 0.7 * ray.Ndir.y;
        col = 0.017 * vec3(1, 1, 1.5) * ray.total_d * amb;
    }
    else {
        col = vec3(1.0);
    }
    //col= texture(Channel, uv2).xyz;
    gl_FragColor = vec4(col, ray.total_d);
}