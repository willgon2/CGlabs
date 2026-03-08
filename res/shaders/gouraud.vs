// Lab 5 - Task 1.3: Gouraud shading vertex shader
// Illumination is computed per vertex (in world space) and interpolated to the fragment shader

uniform mat4 u_model;
uniform mat4 u_viewprojection;

// Scene data
uniform vec3 u_camera_position;
uniform vec3 u_ambient_light;
uniform vec3 u_light_position;
uniform vec3 u_light_color;

// Material properties
uniform vec3 u_Ka;         // ambient coefficient
uniform vec3 u_Kd;         // diffuse coefficient
uniform vec3 u_Ks;         // specular coefficient
uniform float u_shininess; // specular shininess exponent

varying vec3 v_color;  // Ip: per-vertex illumination result
varying vec2 v_uv;

void main()
{
	v_uv = gl_MultiTexCoord0.xy;

	// World-space position and normal
	vec3 world_pos    = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;
	vec3 world_normal = normalize((u_model * vec4(gl_Normal.xyz, 0.0)).xyz);

	// Direction vectors (only normalize directions, NOT positions)
	vec3 L = normalize(u_light_position - world_pos);   // surface -> light
	vec3 V = normalize(u_camera_position - world_pos);  // surface -> camera
	vec3 R = reflect(-L, world_normal);                 // reflect incident ray around N

	// Clamp dot products to [0,1]: negative values would light back faces
	float diff = clamp(dot(L, world_normal), 0.0, 1.0);
	float spec = pow(clamp(dot(R, V), 0.0, 1.0), u_shininess);

	// Illumination: Ip = Ka*Ia + Kd*(L.N)*Id + Ks*(R.V)^n*Is
	v_color = u_Ka * u_ambient_light
	        + u_Kd * diff * u_light_color
	        + u_Ks * spec * u_light_color;

	gl_Position = u_viewprojection * vec4(world_pos, 1.0);
}
