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

	// c) Yellow scale - convert to grayscale, then display as shades of yellow.
	//    R=luma, G=luma, B=0: dark areas become dark yellow, bright areas bright yellow.
	//    Unlike "remove blue", this loses original colour info → true "yellow scale".
	float luma_c = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
	vec3 color_c = vec3(luma_c, luma_c, 0.0);

	// d) Black & white threshold - posterize to pure black or pure white
	//    Compute luminance, then step(0.5, luma) snaps to 0 or 1 with no if.
	float luma_d  = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
	vec3 color_d  = vec3(step(0.5, luma_d));

	// e) Cinematic vignette - darkens corners like a camera lens falloff.
	//    dot(c,c) = squared distance from centre (0 at centre, ~0.5 at corners).
	//    Multiply by 1.8 then clamp → reaches 0 (black) near the corners.
	//    Squaring the vignette gives a smooth, natural-looking rolloff curve.
	vec2 centered_e = v_uv - vec2(0.5);
	float vignette = 1.0 - dot(centered_e, centered_e) * 1.8;
	vignette = clamp(vignette, 0.0, 1.0);
	vignette = vignette * vignette;
	vec3 color_e = texColor.rgb * vignette;

	// f) Box blur - average a 3x3 neighbourhood of texels
	//    s = approximate texel size (1/512 for a 512px texture).
	//    We unroll the loop manually to stay in simple GLSL.
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

	// Select subtask (the only allowed conditional: switching between tasks)
	if (u_subtask == 0)      color = color_a;
	else if (u_subtask == 1) color = color_b;
	else if (u_subtask == 2) color = color_c;
	else if (u_subtask == 3) color = color_d;
	else if (u_subtask == 4) color = color_e;
	else                     color = color_f;

	gl_FragColor = vec4(color, 1.0);
}
