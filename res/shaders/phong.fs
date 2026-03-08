// Lab 5 - Tasks 1.4, 1.5, 1.6: Phong shading fragment shader
// Illumination computed per pixel.
// Supports color texture (Task 1.5), specular texture (Task 1.5), normal texture (Task 1.5)
// Supports multipass (Task 1.6): u_first_pass controls whether ambient is added

varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;

// Scene uniforms (uploaded via sUniformData)
uniform vec3 u_camera_position;
uniform vec3 u_ambient_light;
uniform vec3 u_light_position;
uniform vec3 u_light_color;

// Material properties
uniform float u_shininess;
uniform bool  u_first_pass;       // true on first light pass (add ambient), false on additional

// Texture toggles
uniform bool u_use_color_texture;
uniform bool u_use_spec_texture;
uniform bool u_use_normal_texture;

// Textures
uniform sampler2D u_color_texture;
uniform sampler2D u_normal_texture;

// Default material coefficients (used when textures are off)
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;

void main()
{
	// --- Normal ---
	// Start with the interpolated vertex normal (must not modify varyings directly)
	vec3 N = normalize(v_world_normal);

	if (u_use_normal_texture)
	{
		// Normal map stores values in [0,1]; convert to [-1,1] direction
		vec3 tex_normal = texture2D(u_normal_texture, v_uv).rgb;
		tex_normal = tex_normal * 2.0 - 1.0;
		// Convert from local/object space to world space using model matrix
		tex_normal = normalize((vec4(tex_normal, 0.0)).xyz); // already world via model
		// Blend between vertex normal and texture normal for smoother result
		N = normalize(mix(N, tex_normal, 0.7));
	}

	// --- Material coefficients ---
	vec3 Ka = u_Ka;
	vec3 Kd = u_Kd;

	if (u_use_color_texture)
	{
		vec4 texColor = texture2D(u_color_texture, v_uv);
		// RGB replaces Ka and Kd entirely
		Ka = texColor.rgb;
		Kd = texColor.rgb;
	}

	// Ks: use alpha channel of color texture as specular map if enabled
	vec3 Ks = u_Ks;
	if (u_use_spec_texture)
	{
		float specVal = texture2D(u_color_texture, v_uv).a;
		Ks = vec3(specVal); // grayscale specular
	}

	// --- Lighting vectors (normalize directions only) ---
	vec3 L = normalize(u_light_position - v_world_position);   // surface -> light
	vec3 V = normalize(u_camera_position - v_world_position);  // surface -> camera
	vec3 R = reflect(-L, N); // reflect incident ray (-L) around normal

	// Diffuse & specular (clamp to avoid lighting back faces)
	float diff = clamp(dot(L, N), 0.0, 1.0);
	float spec = pow(clamp(dot(R, V), 0.0, 1.0), u_shininess);

	// --- Illumination equation: Ip = Ka*Ia + Kd*(L.N)*Id + Ks*(R.V)^n*Is ---
	vec3 color = Kd * diff * u_light_color
	           + Ks * spec * u_light_color;

	// Ambient is only added once (on the first pass in multipass rendering)
	if (u_first_pass)
		color += Ka * u_ambient_light;

	gl_FragColor = vec4(color, 1.0);
}
