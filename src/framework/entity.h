#pragma once

#include "framework.h"

class Mesh;
class Image;
class Camera;
class FloatImage;

enum class eRenderMode {
	POINTCLOUD,
	WIREFRAME,
	TRIANGLES,
	TRIANGLES_INTERPOLATED
};

class Entity
{
public:
	Mesh* mesh = nullptr;
	Matrix44 model;

	Vector3 base_position;
	float rotation_speed;
	float scale_base;
	float scale_amp;
	float phase;
	Image* texture = nullptr;

	eRenderMode mode = eRenderMode::TRIANGLES_INTERPOLATED;
	bool use_texture = true;       // 'T' key
	bool use_zbuffer = true;       // 'Z' key
	bool use_interpolation = true; // 'C' key

	Entity();
	~Entity();

	void Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer);
	void Update(float seconds_elapsed);
};