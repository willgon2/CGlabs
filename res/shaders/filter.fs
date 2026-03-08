// Lab 4 - Task 2.3: Image filters applied to a texture
// No conditionals (if/else/switch) used within the filter formulas
// u_subtask selects which filter: 0=a, 1=b, 2=c, 3=d, 4=e, 5=f

varying vec2 v_uv;

uniform sampler2D u_texture;
uniform int u_subtask;

void main()
{
	vec4 texColor = texture2D(u_texture, v_uv);
	vec3 color = texColor.rgb;

	// a) Grayscale - weighted luminance
	float luma_a = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
	vec3 color_a = vec3(luma_a);

	// b) Negative / invert colors
	vec3 color_b = vec3(1.0) - texColor.rgb;

	// c) Sepia tone (warm brownish look, no ifs needed - it's a dot product per channel)
	vec3 color_c = vec3(
		dot(texColor.rgb, vec3(0.393, 0.769, 0.189)),
		dot(texColor.rgb, vec3(0.349, 0.686, 0.168)),
		dot(texColor.rgb, vec3(0.272, 0.534, 0.131))
	);

	// d) Vignette - darken edges using distance from center
	vec2 centered = v_uv - vec2(0.5);
	float vignette = 1.0 - dot(centered, centered) * 2.5;
	vignette = clamp(vignette, 0.0, 1.0);
	vec3 color_d = texColor.rgb * vignette;

	// e) Saturation boost - mix original with grayscale (factor > 1 = more saturated)
	float luma_e = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
	vec3 color_e = mix(vec3(luma_e), texColor.rgb, 2.5);
	color_e = clamp(color_e, 0.0, 1.0);

	// f) Chromatic aberration - sample R, G, B channels at slightly offset UVs
	float amount = 0.012;
	float r = texture2D(u_texture, v_uv + vec2(amount, 0.0)).r;
	float g = texture2D(u_texture, v_uv).g;
	float b = texture2D(u_texture, v_uv - vec2(amount, 0.0)).b;
	vec3 color_f = vec3(r, g, b);

	// Select subtask (the only allowed conditional: switching between tasks)
	if (u_subtask == 0)      color = color_a;
	else if (u_subtask == 1) color = color_b;
	else if (u_subtask == 2) color = color_c;
	else if (u_subtask == 3) color = color_d;
	else if (u_subtask == 4) color = color_e;
	else                     color = color_f;

	gl_FragColor = vec4(color, 1.0);
}
