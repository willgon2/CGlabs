// Full-screen quad vertex shader
// Quad vertices are already in clip space [-1,1], no projection needed
varying vec2 v_uv;

void main()
{
	// UV coords range [0,1]
	v_uv = gl_MultiTexCoord0.xy;

	// Vertices are in clip space already - pass through directly
	gl_Position = vec4(gl_Vertex.xyz, 1.0);
}
