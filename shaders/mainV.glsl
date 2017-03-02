#version 400
in vec3 vp;
void main(){
    float y = vp.y;
    if(vp.y > 0) y *= -1.;
    gl_Position = vec4(vp.x, y, vp.z, 1.0);
}
