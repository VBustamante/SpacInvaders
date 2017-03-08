#version 400
in vec4 coord;
out vec2 tex_coord;
void main(){
    gl_Position = vec4(coord.xy, 0, 1);
    tex_coord = coord.zw;
}
