// Lab 5 - Task 1.3: Gouraud shading fragment shader
// The illumination (Ip) was computed per vertex and is now interpolated here

varying vec3 v_color;
varying vec2 v_uv;

void main()
{
	gl_FragColor = vec4(v_color, 1.0);
}
