varying vec2 v_uv;

uniform float u_time;
uniform float u_aspect;
uniform int   u_subtask;

void main()
{
	vec2 uv = v_uv;
	vec2 uv_aspect = vec2(uv.x * u_aspect, uv.y);
	vec3 color = vec3(0.0);

	// a) horizontal gradient blue to red
	vec3 color_a = vec3(uv.x, 0.0, 1.0 - uv.x);

	// b) radial grayscale, aspect-corrected so it stays circular
	vec2 centered_b = vec2((uv.x - 0.5) * u_aspect, uv.y - 0.5);
	vec3 color_b = vec3(length(centered_b) * 2.0);

	// c) glowing grid using exp falloff from grid lines
	float cellX = mod(uv_aspect.x * 8.0, 1.0);
	float cellY = mod(uv.y * 8.0, 1.0);
	float gx = exp(-min(cellX, 1.0 - cellX) * 25.0);
	float gy = exp(-min(cellY, 1.0 - cellY) * 25.0);
	vec3 color_c = vec3(gy, 0.0, gx);

	// d) pixelated UV gradient
	float N_d = 10.0;
	vec2 cell_uv = floor(vec2(uv.x * N_d * u_aspect, uv.y * N_d))
	             / vec2(N_d * u_aspect, N_d);
	vec3 color_d = vec3(cell_uv.x, cell_uv.y, 0.0);

	// e) checkerboard
	float check = mod(floor(uv.x * 8.0) + floor(uv.y * 8.0), 2.0);
	vec3 color_e = vec3(check);

	// f) sine wave filled shape with vertical gradient
	const float PI = 3.14159265;
	float wave_y = 0.5 + 0.35 * sin(uv.x * 2.0 * PI);
	float mask = 1.0 - smoothstep(wave_y - 0.005, wave_y + 0.005, uv.y);
	float intensity = mix(1.0 - uv.y, uv.y, mask);
	vec3 color_f = vec3(0.0, intensity, 0.0);

	if (u_subtask == 0)      color = color_a;
	else if (u_subtask == 1) color = color_b;
	else if (u_subtask == 2) color = color_c;
	else if (u_subtask == 3) color = color_d;
	else if (u_subtask == 4) color = color_e;
	else                     color = color_f;

	gl_FragColor = vec4(color, 1.0);
}
