#pragma once

#include "framework.h"
#include "material.h"   // for sUniformData

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

    // Animation parameters
    Vector3   base_position;
    float     rotation_speed;
    float     scale_base;
    float     scale_amp;
    float     phase;

    // ---- Lab 4 Task 2.5: GPU rendering resources ----
    // The shader that draws this entity on the GPU
    Shader*   shader      = nullptr;
    // The colour/diffuse texture on the GPU
    Texture*  gpu_texture = nullptr;

    // ---- Lab 5: Material-based rendering ----
    // A Material groups a shader + all its properties together
    Material* material    = nullptr;

    Entity();
    ~Entity();

    // Lab 3 - software rasterizer (wireframe)
    void Render(Image* framebuffer, Camera* camera, const Color& c);

    // Lab 4 Task 2.5 - renders the mesh through a raw shader on the GPU
    void Render(Camera* camera);

    // Lab 5 - renders using the Material pipeline
    //   data contains everything the shader needs: VP matrix, lights, etc.
    //   Entity fills in data.model (the one thing only it knows) before passing on.
    void Render(sUniformData& data);

    void Update(float seconds_elapsed);
};
