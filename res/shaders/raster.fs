// Lab 4 - Task 2.5: GPU 3D mesh rasterization fragment shader
// Samples the diffuse/color texture and outputs it directly

uniform sampler2D u_texture;

varying vec2 v_uv;

void main()
{
	vec4 texColor = texture2D(u_texture, v_uv);
	gl_FragColor = texColor;
}
