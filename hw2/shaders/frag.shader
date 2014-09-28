varying vec3 color;
varying float val;
varying vec2 uv;
uniform sampler2D raster;
uniform sampler2D mask;
varying float a;
uniform bool que;
void main(void)
{
vec4 t = texture2D(raster,uv);
vec4 d = texture2D(mask,uv);
vec4 c = vec4(1,1,1,1);
if (!que)
{
	if(val > .5 )
	c = vec4(1,0,0,1);
	else
	c = vec4(0,1,0,1);
	if(val > .5)
	c = vec4(mix(vec3(.5,.2,0),vec3(0,.1,.5),val),1);
	else
	c = vec4(val,val,0,1);
	if(t.r > .4)
	{
		c = vec4(1,1,1,1);
	}
	if(a < 1.0)
	{
		discard;
	}
}
else
{
	if(d.r > 0.0)
	{
		c = t;
	}
	else
	{
		c = vec4(1,1,1,1);
	}
}

gl_FragColor = c;//vec4(color.xyz, 1.0);
}
