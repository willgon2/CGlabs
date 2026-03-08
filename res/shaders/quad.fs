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

	// b) Radial grayscale: dark at center, bright at edges (TRUE CIRCLE)
	//    Problem: UV space is square (0..1 x 0..1) but the screen is wider
	//    than tall. A unit step in x covers more pixels than a unit step in y.
	//    Fix: scale x by u_aspect before length() so the distance metric
	//    matches actual screen pixels → equal distances in all directions.
	vec2 centered_b = vec2((uv.x - 0.5) * u_aspect, uv.y - 0.5);
	float dist_b = length(centered_b) * 2.0;
	vec3 color_b = vec3(dist_b);

	// c) Soft glowing grid: blue vertical + red horizontal lines on black
	//    step() gives a hard binary edge → not blurred.
	//    Instead: compute the distance from each pixel to the nearest grid line,
	//    then apply exp(-dist * sharpness) → Gaussian glow that fades out smoothly.
	//    min(cell, 1-cell) = distance to the nearest edge of the repeating cell.
	//    R = horizontal glow (red), G = 0, B = vertical glow (blue).
	float cellX = mod(uv_aspect.x * 8.0, 1.0);
	float distX = min(cellX, 1.0 - cellX);   // 0 exactly on vertical line
	float gx = exp(-distX * 25.0);

	float cellY = mod(uv.y * 8.0, 1.0);
	float distY = min(cellY, 1.0 - cellY);   // 0 exactly on horizontal line
	float gy = exp(-distY * 25.0);

	vec3 color_c = vec3(gy, 0.0, gx); // intersections go magenta (R+B)

	// d) 2D UV colour gradient WITH visible square grid overlay
	//    Base gradient: R=uv.x, G=0.5+0.5*uv.y, B=0
	//    Grid: 4x4 cells. fract(uv*4) goes 0→1 inside each cell.
	//    step(0.03, fract) is 0 only in the first 3% of a cell = the line band.
	//    1 - step(...) flips it: 1 ON the line, 0 everywhere else.
	//    Multiplying by (1 - on_line) blacks out the grid lines.
	vec3 base_d = vec3(uv.x, 0.5 + 0.5 * uv.y, 0.0);
	float line_x = 1.0 - step(0.03, fract(uv.x * 4.0));
	float line_y = 1.0 - step(0.03, fract(uv.y * 4.0));
	float on_line = clamp(line_x + line_y, 0.0, 1.0);
	vec3 color_d = base_d * (1.0 - on_line);

	// e) Black and white checkerboard
	//    floor(uv*N) gives integer cell index; sum of indices mod 2 alternates 0/1.
	float check = mod(floor(uv.x * 8.0) + floor(uv.y * 8.0), 2.0);
	vec3 color_e = vec3(check);

	// f) Glowing green sine wave with visible colour gradient
	//    wave_y = y-position of the curve at this x.
	//    exp(-dist * 6.0): low sharpness → WIDE glow → the black→green
	//    gradient is visible across a large portion of the screen.
	//    sharpness=15 was too high: glow vanished 15% from the curve.
	//    sharpness=6: still 37% green at 20% from the curve → clear gradient.
	//    Colour: pure G for the main glow, tiny R boost at the bright peak
	//    (glow^2 ≈ 1 only very close to the curve) for a yellow-green highlight.
	const float PI = 3.14159265;
	float wave_y = 0.5 + 0.35 * sin(uv.x * 2.0 * PI);
	// sharpness=12: tight bright core at the curve fades to dark green away from it.
	// Pure green (no R): bright green at curve, nearly black background.
	float glow   = exp(-abs(uv.y - wave_y) * 12.0);
	vec3 color_f = vec3(0.0, 0.05 + 0.95 * glow, 0.0);

	// Select subtask (the only allowed conditional: switching between tasks)
	if (u_subtask == 0)      color = color_a;
	else if (u_subtask == 1) color = color_b;
	else if (u_subtask == 2) color = color_c;
	else if (u_subtask == 3) color = color_d;
	else if (u_subtask == 4) color = color_e;
	else                     color = color_f;

	gl_FragColor = vec4(color, 1.0);
}
