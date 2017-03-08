#version 400

in vec2 tex_coord;
uniform sampler2D tex;
uniform vec4 color;
void main(){

    float alpha = texture2D(tex, vec2(tex_coord.x*1, tex_coord.y *1)).a;
    gl_FragColor = vec4(1, 1, 1, alpha) * color;
//    gl_FragColor= vec4(0, 0, .5, 1.0);
}