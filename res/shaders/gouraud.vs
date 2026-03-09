uniform mat4 u_model;
uniform mat4 u_viewprojection;

uniform vec3 u_camera_position;
uniform vec3 u_ambient_light;
uniform vec3 u_light_position;
uniform vec3 u_light_color;

uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform float u_shininess;

varying vec3 v_color;
varying vec2 v_uv;

void main()
{
	v_uv = gl_MultiTexCoord0.xy;

	vec3 world_pos    = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;
	vec3 world_normal = normalize((u_model * vec4(gl_Normal.xyz, 0.0)).xyz);

	vec3 L = normalize(u_light_position - world_pos);
	vec3 V = normalize(u_camera_position - world_pos);
	vec3 R = reflect(-L, world_normal);

	float diff = clamp(dot(L, world_normal), 0.0, 1.0);
	float spec = pow(clamp(dot(R, V), 0.0, 1.0), u_shininess);

	v_color = u_Ka * u_ambient_light
	        + u_Kd * diff * u_light_color
	        + u_Ks * spec * u_light_color;

	gl_Position = u_viewprojection * vec4(world_pos, 1.0);
}
