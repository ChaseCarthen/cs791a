attribute vec3 v_position;
attribute vec3 v_color;
attribute vec2 v_uv;
attribute float active;
varying vec3 color;
varying float val;
varying vec2 uv;
uniform mat4 mvpMatrix;
uniform float h;
varying float a;
void main(void)
{
    gl_Position = mvpMatrix * vec4(v_position.x,h*v_position.y,v_position.z, 1.0);
    color = v_color;
    val = v_position.y;
    uv = v_uv;
    a = active;
    //discard;
}
