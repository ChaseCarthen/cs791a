varying vec3 color;
varying float val;
uniform sampler1D tex;

void main(void)
{
vec4 t = texture1D(tex,val);
vec4 c;
if(val > .5 )
c = vec4(1,0,0,1);
else
c = vec4(0,1,0,1);
gl_FragColor = t;//vec4(mix(vec3(.5,0,0),vec3(0,0,.5),val),1); //+ vec4(color.xyz, 1.0);
}
