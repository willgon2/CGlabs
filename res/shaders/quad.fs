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

	// Correct UVs for aspect ratio to avoid deformations in distance/grid checks
	vec2 uv_aspect = vec2(uv.x * u_aspect, uv.y);

	vec3 color = vec3(0.0);

	// a) Horizontal blue-to-red gradient
	//    Left edge: pure blue (0,0,1). Right edge: pure red (1,0,0).
	//    As uv.x goes 0→1, red rises and blue falls.
	vec3 color_a = vec3(uv.x, 0.0, 1.0 - uv.x);

	// b) Radial grayscale: dark at center, bright at edges
	//    length(uv - 0.5) = distance from screen center in UV space.
	//    Multiply by 2 so the corners reach ~1.4 (full bright).
	float dist_b = length(uv - vec2(0.5)) * 2.0;
	vec3 color_b = vec3(dist_b);

	// c) Colored grid: blue vertical lines + red horizontal lines on black
	//    mod(x*N, 1.0) gives a sawtooth 0..1 repeating N times.
	//    step(threshold, mod(...)) is 0 inside the line band, 1 outside.
	//    1 - step(...) flips it: 1 on the line, 0 everywhere else.
	//    R = horizontal lines (red), G = 0, B = vertical lines (blue).
	float gx = 1.0 - step(0.08, mod(uv_aspect.x * 8.0, 1.0)); // vertical lines
	float gy = 1.0 - step(0.08, mod(uv.y * 8.0, 1.0));         // horizontal lines
	vec3 color_c = vec3(gy, 0.0, gx);

	// d) 2D bilinear colour gradient (4-corner blend)
	//    Corners: bottom-left=red, bottom-right=orange, top-left=green, top-right=yellow
	//    mix(mix(BL, BR, x), mix(TL, TR, x), y) bilinearly blends all four.
	vec3 bl = vec3(1.0, 0.0, 0.0); // red   (uv = 0,0)
	vec3 br = vec3(1.0, 0.5, 0.0); // orange (uv = 1,0)
	vec3 tl = vec3(0.0, 1.0, 0.0); // green  (uv = 0,1)
	vec3 tr = vec3(1.0, 1.0, 0.0); // yellow (uv = 1,1)
	vec3 color_d = mix(mix(bl, br, uv.x), mix(tl, tr, uv.x), uv.y);

	// e) Black and white checkerboard
	//    floor(uv*N) gives integer cell index; sum of indices mod 2 alternates 0/1.
	float check = mod(floor(uv.x * 8.0) + floor(uv.y * 8.0), 2.0);
	vec3 color_e = vec3(check);

	// f) Glowing green sine wave on black background
	//    wave_y = vertical position of the sine curve at this x.
	//    exp(-|dist| * sharpness) creates a soft glow around the curve.
	const float PI = 3.14159265;
	float wave_y = 0.5 + 0.35 * sin(uv.x * 4.0 * PI);
	float glow   = exp(-abs(uv.y - wave_y) * 15.0);
	vec3 color_f = vec3(0.0, glow, 0.0);

	// Select subtask (the only allowed conditional: switching between tasks)
	if (u_subtask == 0)      color = color_a;
	else if (u_subtask == 1) color = color_b;
	else if (u_subtask == 2) color = color_c;
	else if (u_subtask == 3) color = color_d;
	else if (u_subtask == 4) color = color_e;
	else                     color = color_f;

	gl_FragColor = vec4(color, 1.0);
}
