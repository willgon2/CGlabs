// Lab 4 - Task 2.5: GPU 3D mesh rasterization vertex shader

uniform mat4 u_model;
uniform mat4 u_viewprojection;

varying vec2 v_uv;

void main()
{
	// Pass texture coordinates to fragment shader
	v_uv = gl_MultiTexCoord0.xy;

	// Transform vertex from local space to world space
	vec3 world_position = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;

	// Project using the view-projection matrix
	gl_Position = u_viewprojection * vec4(world_position, 1.0);
}
