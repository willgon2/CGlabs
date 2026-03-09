varying vec2 v_uv;

uniform sampler2D u_texture;
uniform float u_time;
uniform float u_aspect;
uniform int u_subtask;

void main()
{
	vec3 color = vec3(0.0);

	// a) rotation around center
	{
		vec2 centered = v_uv - vec2(0.5);
		centered.x *= u_aspect;

		float c = cos(u_time * 0.5);
		float s = sin(u_time * 0.5);
		vec2 rotated = vec2(
			c * centered.x - s * centered.y,
			s * centered.x + c * centered.y
		);

		rotated.x /= u_aspect;
		rotated += vec2(0.5);

		vec2 clamped = clamp(rotated, 0.0, 1.0);
		float inBounds = step(0.0, rotated.x) * step(rotated.x, 1.0)
		               * step(0.0, rotated.y) * step(rotated.y, 1.0);
		color = texture2D(u_texture, clamped).rgb * inBounds;
	}
	vec3 color_a = color;

	// b) pixelization animated with time
	{
		float pixelSize = floor(8.0 + 40.0 * (0.5 + 0.5 * sin(u_time * 0.7)));
		vec2 pixelated = floor(v_uv * pixelSize) / pixelSize;
		color = texture2D(u_texture, pixelated).rgb;
	}
	vec3 color_b = color;

	if (u_subtask == 0) color = color_a;
	else                color = color_b;

	gl_FragColor = vec4(color, 1.0);
}
