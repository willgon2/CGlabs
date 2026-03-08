// Lab 4 - Task 2.2: Mathematical formula patterns
// No conditionals (if/else/switch) used within pattern formulas
// u_subtask selects which pattern: 0=a, 1=b, 2=c, 3=d, 4=e, 5=f

varying vec2 v_uv;

uniform float u_time;
uniform float u_aspect; // window_width / window_height
uniform int   u_subtask;

void main()
{
	vec2 uv = v_uv;

	// Correct UVs for aspect ratio to avoid deformations
	vec2 uv_aspect = vec2(uv.x * u_aspect, uv.y);

	vec3 color = vec3(0.0);

	// a) Simple smooth gradient using both UV axes
	vec3 color_a = vec3(uv.x, uv.y, 1.0 - uv.x * uv.y);

	// b) Vertical stripes using mod + step (no if needed)
	float stripe = step(0.5, mod(uv_aspect.x * 8.0, 1.0));
	vec3 color_b = mix(vec3(0.1, 0.1, 0.8), vec3(0.9, 0.2, 0.1), stripe);

	// c) Concentric rings using distance from center
	vec2 centered = uv_aspect - vec2(u_aspect * 0.5, 0.5);
	float d = length(centered);
	float ring = step(0.5, mod(d * 10.0, 1.0));
	vec3 color_c = mix(vec3(0.0, 0.2, 0.8), vec3(1.0, 0.8, 0.0), ring);

	// d) Checkerboard using floor + mod (no if)
	float check = mod(floor(uv.x * 8.0) + floor(uv.y * 8.0), 2.0);
	vec3 color_d = vec3(check);

	// e) Sine wave color pattern
	float wave = 0.5 + 0.5 * sin(uv_aspect.x * 20.0 + uv.y * 15.0);
	vec3 color_e = mix(vec3(0.0, 0.0, 0.8), vec3(1.0, 0.8, 0.0), wave);

	// f) Complex radial pattern: polar + distance bands combined
	vec2 c2 = uv - vec2(0.5);
	c2.x *= u_aspect;
	float dist = length(c2);
	// angle in [0,1] range using atan - no conditionals needed
	float angle = atan(c2.y, c2.x) / (2.0 * 3.14159265) + 0.5;
	float radial = step(0.5, mod(angle * 12.0 + dist * 15.0, 1.0));
	vec3 color_f = mix(vec3(0.05, 0.0, 0.3), vec3(1.0, 0.3, 0.0), radial);
	// Add a center glow (no if needed)
	color_f = mix(color_f, vec3(1.0), 1.0 - smoothstep(0.0, 0.1, dist));

	// Select subtask (the only allowed conditional: switching between tasks)
	if (u_subtask == 0)      color = color_a;
	else if (u_subtask == 1) color = color_b;
	else if (u_subtask == 2) color = color_c;
	else if (u_subtask == 3) color = color_d;
	else if (u_subtask == 4) color = color_e;
	else                     color = color_f;

	gl_FragColor = vec4(color, 1.0);
}
