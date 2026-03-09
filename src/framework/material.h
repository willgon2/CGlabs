#pragma once

#include "framework.h"
#include "texture.h"

class Shader;

struct sLight
{
	Vector3 position;
	Vector3 color;

	sLight() {}
	sLight(Vector3 pos, Vector3 col) : position(pos), color(col) {}
};

struct sUniformData
{
	Matrix44 viewprojection;
	Matrix44 model;

	Vector3 ambient_light;
	Vector3 camera_position;

	static const int MAX_LIGHTS = 8;
	sLight lights[MAX_LIGHTS];
	int num_lights = 0;

	float time = 0.0f;
	bool first_pass = true; // adds ambient on first light pass only
};

class Material
{
public:
	Shader* shader = nullptr;

	Texture* color_texture  = nullptr;
	Texture* normal_texture = nullptr;

	Vector3 Ka = Vector3(0.1f, 0.1f, 0.1f);
	Vector3 Kd = Vector3(0.8f, 0.8f, 0.8f);
	Vector3 Ks = Vector3(0.5f, 0.5f, 0.5f);
	float   shininess = 32.0f;

	bool use_color_texture  = false;
	bool use_spec_texture   = false;
	bool use_normal_texture = false;

	Material();

	void Enable(const sUniformData& data, bool first_pass = true);
	void Disable();
};
