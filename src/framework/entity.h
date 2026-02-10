#pragma once

#include "framework.h"

class Mesh;
class Image;
class Camera;

class Entity
{
public:
	Mesh* mesh;
	Matrix44 model;

	Vector3 base_position;
	float rotation_speed;
	float scale_base;
	float scale_amp;
	float phase;

	Entity();
	~Entity();

	void Render(Image* framebuffer, Camera* camera, const Color& c);
	void Update(float seconds_elapsed);
};