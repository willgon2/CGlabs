// Lab 4 - Task 2.4: Image transformations using time
// All transformations use the u_time variable
// Two different transformations: subtask 0 = rotation, subtask 1 = pixelization

varying vec2 v_uv;

uniform sampler2D u_texture;
uniform float u_time;
uniform int u_subtask;

void main()
{
	vec3 color = vec3(0.0);

	// Transformation a) Rotation around the center of the image
	{
		vec2 centered = v_uv - vec2(0.5);
		float c = cos(u_time * 0.5);
		float s = sin(u_time * 0.5);
		// Rotate the UV coordinates
		vec2 rotated = vec2(
			c * centered.x - s * centered.y,
			s * centered.x + c * centered.y
		) + vec2(0.5);

		// Sample. Pixels outside [0,1] show black (clamped).
		vec2 clamped = clamp(rotated, 0.0, 1.0);
		// Use step to black-out out-of-bounds regions without if:
		float inBounds = step(0.0, rotated.x) * step(rotated.x, 1.0)
		               * step(0.0, rotated.y) * step(rotated.y, 1.0);
		color = texture2D(u_texture, clamped).rgb * inBounds;
	}

	vec3 color_a = color;

	// Transformation b) Pixelization - simulates a lower resolution
	{
		// Number of "pixels" oscillates with time for an animated effect
		float pixelSize = floor(8.0 + 40.0 * (0.5 + 0.5 * sin(u_time * 0.7)));
		vec2 pixelated = floor(v_uv * pixelSize) / pixelSize;
		color = texture2D(u_texture, pixelated).rgb;
	}

	vec3 color_b = color;

	// Select subtask
	if (u_subtask == 0) color = color_a;
	else                color = color_b;

	gl_FragColor = vec4(color, 1.0);
}
