#include "entity.h"
#include "mesh.h"
#include "image.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "material.h"
#include <cmath>

Entity::Entity()
{
    mesh           = nullptr;
    model          = Matrix44();
    base_position  = Vector3(0.0f, 0.0f, 0.0f);
    rotation_speed = 1.0f;
    scale_base     = 1.0f;
    scale_amp      = 0.0f;
    phase          = 0.0f;
}

Entity::~Entity()
{
}

// Lab 3 - software wireframe renderer
static bool IsInsideClipCube(const Vector3& p)
{
    return (p.x >= -1.0f && p.x <= 1.0f &&
            p.y >= -1.0f && p.y <= 1.0f &&
            p.z >= -1.0f && p.z <= 1.0f);
}

static Vector2 ClipToScreen(const Vector3& p, unsigned int width, unsigned int height)
{
    return Vector2(
        (p.x * 0.5f + 0.5f) * (float)(width  - 1),
        (p.y * 0.5f + 0.5f) * (float)(height - 1)
    );
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c)
{
    if (!framebuffer || !camera || !mesh) return;

    const std::vector<Vector3>& vertices = mesh->GetVertices();
    if (vertices.size() < 3) return;

    for (size_t i = 0; i + 2 < vertices.size(); i += 3)
    {
        Vector3 v0 = model * vertices[i];
        Vector3 v1 = model * vertices[i + 1];
        Vector3 v2 = model * vertices[i + 2];

        Vector3 p0 = camera->ProjectVector(v0);
        Vector3 p1 = camera->ProjectVector(v1);
        Vector3 p2 = camera->ProjectVector(v2);

        if (!IsInsideClipCube(p0) || !IsInsideClipCube(p1) || !IsInsideClipCube(p2))
            continue;

        Vector2 s0 = ClipToScreen(p0, framebuffer->width, framebuffer->height);
        Vector2 s1 = ClipToScreen(p1, framebuffer->width, framebuffer->height);
        Vector2 s2 = ClipToScreen(p2, framebuffer->width, framebuffer->height);

        framebuffer->DrawLineDDA((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, c);
        framebuffer->DrawLineDDA((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, c);
        framebuffer->DrawLineDDA((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, c);
    }
}

// Lab 4 - GPU render
void Entity::Render(Camera* camera)
{
    if (!shader || !mesh || !camera) return;

    shader->Enable();
    shader->SetMatrix44("u_model",          model);
    shader->SetMatrix44("u_viewprojection", camera->GetViewProjectionMatrix());

    if (gpu_texture)
        shader->SetTexture("u_texture", gpu_texture);

    mesh->Render();
    shader->Disable();
}

// Lab 5 - material render
void Entity::Render(sUniformData& data)
{
    if (!material || !mesh) return;

    data.model = model;
    material->Enable(data, data.first_pass);
    mesh->Render();
    material->Disable();
}

void Entity::Update(float seconds_elapsed)
{
    phase += seconds_elapsed;

    float angle = phase * rotation_speed;
    float s     = scale_base + scale_amp * (0.5f + 0.5f * sinf(phase));

    Vector3 pos = base_position;
    pos.y += 0.25f * sinf(phase * 1.3f);

    Matrix44 T, R, S;
    T.MakeTranslationMatrix(pos.x, pos.y, pos.z);
    R.MakeRotationMatrix(angle, Vector3(0.0f, 1.0f, 0.0f));
    S.MakeScaleMatrix(s, s, s);

    // TRS order: scale, rotate, translate
    model = T * R * S;
}
