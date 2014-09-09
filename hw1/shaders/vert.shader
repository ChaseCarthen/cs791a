attribute vec3 v_position;
attribute vec3 v_color;
varying vec3 color;
varying float val;
uniform mat4 mvpMatrix;
uniform float h;
void main(void)
{
    gl_Position = mvpMatrix * vec4(v_position.x,h*v_position.y,v_position.z, 1.0);
    color = v_color;
    val = v_position.y;
}
