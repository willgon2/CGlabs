varying vec2 v_uv;

void main()
{
	v_uv = gl_MultiTexCoord0.xy;
	gl_Position = vec4(gl_Vertex.xyz, 1.0);
}
