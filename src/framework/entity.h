#pragma once

#include "framework.h"
#include "material.h"

class Mesh;
class Image;
class Camera;
class Shader;
class Texture;
class Material;

class Entity
{
public:
    Mesh*     mesh = nullptr;
    Matrix44  model;

    Vector3   base_position;
    float     rotation_speed;
    float     scale_base;
    float     scale_amp;
    float     phase;

    Shader*   shader      = nullptr;
    Texture*  gpu_texture = nullptr;
    Material* material    = nullptr;

    Entity();
    ~Entity();

    void Render(Image* framebuffer, Camera* camera, const Color& c); // Lab 3 wireframe
    void Render(Camera* camera);                                     // Lab 4 GPU
    void Render(sUniformData& data);                                 // Lab 5 material

    void Update(float seconds_elapsed);
};
