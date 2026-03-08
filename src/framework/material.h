#pragma once

#include "framework.h"
#include "texture.h"

class Shader;

// -----------------------------------------------
// sLight: represents one point light in the scene
// -----------------------------------------------
struct sLight
{
	Vector3 position;
	Vector3 color;

	sLight() {}
	sLight(Vector3 pos, Vector3 col) : position(pos), color(col) {}
};

// -----------------------------------------------
// sUniformData: all data that travels from
// Application -> Entity -> Material -> Shader
// -----------------------------------------------
struct sUniformData
{
	Matrix44 viewprojection;   // camera VP matrix
	Matrix44 model;            // entity model matrix (filled in Entity::Render)

	Vector3 ambient_light;     // global scene ambient
	Vector3 camera_position;   // eye position

	static const int MAX_LIGHTS = 8;
	sLight lights[MAX_LIGHTS];
	int num_lights = 0;

	float time = 0.0f;

	// For multipass rendering: true on the first light pass (add ambient),
	// false on subsequent passes (only diffuse+specular, no ambient double-add)
	bool first_pass = true;
};

// -----------------------------------------------
// Material: encapsulates a shader and its properties.
// One material = one "look" for an entity.
// -----------------------------------------------
class Material
{
public:
	Shader* shader = nullptr;

	// Textures (Task 1.5)
	Texture* color_texture  = nullptr;
	Texture* normal_texture = nullptr;

	// Material coefficients for lighting
	Vector3 Ka = Vector3(0.1f, 0.1f, 0.1f);   // ambient
	Vector3 Kd = Vector3(0.8f, 0.8f, 0.8f);   // diffuse
	Vector3 Ks = Vector3(0.5f, 0.5f, 0.5f);   // specular
	float   shininess = 32.0f;

	// Texture toggle flags (toggled by C, S, N keys)
	bool use_color_texture  = false;
	bool use_spec_texture   = false;
	bool use_normal_texture = false;

	Material();

	// Enable the material: binds shader and uploads ALL uniforms to the GPU
	void Enable(const sUniformData& data, bool first_pass = true);

	// Disable the material: unbinds shader (and any textures)
	void Disable();
};
