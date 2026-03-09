varying vec2 v_uv;

uniform sampler2D u_texture;
uniform int u_subtask;

void main()
{
	vec4 texColor = texture2D(u_texture, v_uv);
	vec3 color = texColor.rgb;

	// a) grayscale
	float luma_a = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
	vec3 color_a = vec3(luma_a);

	// b) negative
	vec3 color_b = vec3(1.0) - texColor.rgb;

	// c) yellow scale
	float luma_c = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
	vec3 color_c = vec3(luma_c, luma_c, 0.0);

	// d) b&w threshold
	float luma_d = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
	vec3 color_d = vec3(step(0.5, luma_d));

	// e) vignette
	vec2 centered_e = v_uv - vec2(0.5);
	float vignette = 1.0 - dot(centered_e, centered_e) * 1.8;
	vignette = clamp(vignette, 0.0, 1.0);
	vignette = vignette * vignette;
	vec3 color_e = texColor.rgb * vignette;

	// f) box blur 3x3
	float s = 0.002;
	vec3 color_f = vec3(0.0);
	color_f += texture2D(u_texture, v_uv + vec2(-s, -s)).rgb;
	color_f += texture2D(u_texture, v_uv + vec2(0.0, -s)).rgb;
	color_f += texture2D(u_texture, v_uv + vec2( s, -s)).rgb;
	color_f += texture2D(u_texture, v_uv + vec2(-s, 0.0)).rgb;
	color_f += texture2D(u_texture, v_uv + vec2(0.0, 0.0)).rgb;
	color_f += texture2D(u_texture, v_uv + vec2( s, 0.0)).rgb;
	color_f += texture2D(u_texture, v_uv + vec2(-s,  s)).rgb;
	color_f += texture2D(u_texture, v_uv + vec2(0.0,  s)).rgb;
	color_f += texture2D(u_texture, v_uv + vec2( s,  s)).rgb;
	color_f /= 9.0;

	if (u_subtask == 0)      color = color_a;
	else if (u_subtask == 1) color = color_b;
	else if (u_subtask == 2) color = color_c;
	else if (u_subtask == 3) color = color_d;
	else if (u_subtask == 4) color = color_e;
	else                     color = color_f;

	gl_FragColor = vec4(color, 1.0);
}
