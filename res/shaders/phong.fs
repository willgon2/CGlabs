varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;

uniform vec3 u_camera_position;
uniform vec3 u_ambient_light;
uniform vec3 u_light_position;
uniform vec3 u_light_color;

uniform float u_shininess;
uniform bool  u_first_pass;

uniform bool u_use_color_texture;
uniform bool u_use_spec_texture;
uniform bool u_use_normal_texture;

uniform sampler2D u_color_texture;
uniform sampler2D u_normal_texture;

uniform mat4 u_model;

uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;

void main()
{
	vec3 N = normalize(v_world_normal);

	if (u_use_normal_texture)
	{
		vec3 tex_normal = texture2D(u_normal_texture, v_uv).rgb;
		tex_normal = tex_normal * 2.0 - vec3(1.0);
		N = normalize((u_model * vec4(tex_normal, 0.0)).xyz);
	}

	vec3 Ka = u_Ka;
	vec3 Kd = u_Kd;

	if (u_use_color_texture)
	{
		vec4 texColor = texture2D(u_color_texture, v_uv);
		Ka = texColor.rgb;
		Kd = texColor.rgb;
	}

	vec3 Ks = u_Ks;
	if (u_use_spec_texture)
		Ks = vec3(texture2D(u_color_texture, v_uv).a);

	vec3 L = normalize(u_light_position - v_world_position);
	vec3 V = normalize(u_camera_position - v_world_position);
	vec3 R = reflect(-L, N);

	float diff = clamp(dot(L, N), 0.0, 1.0);
	float spec = pow(clamp(dot(R, V), 0.0, 1.0), u_shininess);

	vec3 color = Kd * diff * u_light_color
	           + Ks * spec * u_light_color;

	if (u_first_pass)
		color += Ka * u_ambient_light;

	gl_FragColor = vec4(color, 1.0);
}
