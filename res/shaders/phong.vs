// Lab 5 - Tasks 1.4, 1.5, 1.6: Phong shading vertex shader
// Passes world-space position and normal to the fragment shader
// Illumination is computed PER PIXEL in the fragment shader

uniform mat4 u_model;
uniform mat4 u_viewprojection;

varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;

void main()
{
	v_uv = gl_MultiTexCoord0.xy;

	// Transform to world space and pass to fragment shader
	v_world_position = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;

	// Transform normal to world space (use 0.0 w to ignore translation)
	v_world_normal = (u_model * vec4(gl_Normal.xyz, 0.0)).xyz;

	gl_Position = u_viewprojection * vec4(v_world_position, 1.0);
}
