#include "entity.h"
#include "mesh.h"
#include "camera.h"
#include "image.h"

Entity::Entity()
    : mesh(nullptr), model(), base_position(0, 0, 0),
    rotation_speed(0), scale_base(1), scale_amp(0), phase(0), texture(nullptr)
{
}

Entity::~Entity()
{
}

void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer)
{
    if (!framebuffer || !camera || !mesh) return;

    const std::vector<Vector3>& vertices = mesh->GetVertices();
    const std::vector<Vector2>& uvs = mesh->GetUVs();

    if (vertices.size() < 3) return;

    float width = (float)framebuffer->width;
    float height = (float)framebuffer->height;

    for (size_t i = 0; i + 2 < vertices.size(); i += 3)
    {
        Vector3 v0 = model * vertices[i];
        Vector3 v1 = model * vertices[i + 1];
        Vector3 v2 = model * vertices[i + 2];

        Vector3 p0 = camera->ProjectVector(v0);
        Vector3 p1 = camera->ProjectVector(v1);
        Vector3 p2 = camera->ProjectVector(v2);

        if (p0.x < -1.0f || p0.x > 1.0f || p0.y < -1.0f || p0.y > 1.0f || p0.z < 0.0f || p0.z > 1.0f ||
            p1.x < -1.0f || p1.x > 1.0f || p1.y < -1.0f || p1.y > 1.0f || p1.z < 0.0f || p1.z > 1.0f ||
            p2.x < -1.0f || p2.x > 1.0f || p2.y < -1.0f || p2.y > 1.0f || p2.z < 0.0f || p2.z > 1.0f)
            continue;

        Vector2 s0((p0.x + 1.0f) * 0.5f * width, (p0.y + 1.0f) * 0.5f * height);
        Vector2 s1((p1.x + 1.0f) * 0.5f * width, (p1.y + 1.0f) * 0.5f * height);
        Vector2 s2((p2.x + 1.0f) * 0.5f * width, (p2.y + 1.0f) * 0.5f * height);

        // --- RENDER MODES ---

        if (mode == eRenderMode::WIREFRAME)
        {
            // Draw lines for wireframe
            framebuffer->DrawLineDDA((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, Color::WHITE);
        }
        else if (mode == eRenderMode::POINTCLOUD)
        {
            framebuffer->SetPixel((int)s0.x, (int)s0.y, Color::WHITE);
            framebuffer->SetPixel((int)s1.x, (int)s1.y, Color::WHITE);
            framebuffer->SetPixel((int)s2.x, (int)s2.y, Color::WHITE);
        }
        else // TRIANGLES_INTERPOLATED
        {
            Vector2 uv0 = uvs[i];
            Vector2 uv1 = uvs[i + 1];
            Vector2 uv2 = uvs[i + 2];

            // Setup Colors for interpolation (Task C)
            Color c0, c1, c2;
            if (use_interpolation) {
                c0 = Color::RED; c1 = Color::GREEN; c2 = Color::BLUE;
            }
            else {
                c0 = Color::WHITE; c1 = Color::WHITE; c2 = Color::WHITE;
            }

            framebuffer->DrawTriangleInterpolated(
                Vector3(s0.x, s0.y, p0.z),
                Vector3(s1.x, s1.y, p1.z),
                Vector3(s2.x, s2.y, p2.z),
                c0, c1, c2, // Pass configured colors
                use_zbuffer ? zBuffer : nullptr,  // 'Z' key toggles this
                use_texture ? this->texture : nullptr, // 'T' key toggles this
                uv0, uv1, uv2
            );
        }
    }
}

void Entity::Update(float seconds_elapsed)
{
    if (!mesh) return;

    phase += seconds_elapsed;

    float angle = phase * rotation_speed;
    float s = scale_base + scale_amp * (0.5f + 0.5f * sinf(phase));

    Vector3 pos = base_position;
    pos.y += 0.25f * sinf(phase * 1.3f);

    Matrix44 T;
    T.MakeTranslationMatrix(pos.x, pos.y, pos.z);

    Matrix44 R;
    R.MakeRotationMatrix(angle, Vector3(0.0f, 1.0f, 0.0f));

    Matrix44 S;
    S.MakeScaleMatrix(s, s, s);

    model = T * R * S;
}