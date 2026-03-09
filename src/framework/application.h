#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "button.h"
#include "shader.h"
#include "texture.h"
#include "material.h"
#include <vector>

class Entity;
class Camera;
class Mesh;

class Application
{
public:
    SDL_Window* window = nullptr;
    int window_width;
    int window_height;

    float time;

    const Uint8* keystate;
    int mouse_state;
    Vector2 mouse_position;
    Vector2 mouse_delta;

    void OnKeyPressed(SDL_KeyboardEvent event);
    void OnMouseButtonDown(SDL_MouseButtonEvent event);
    void OnMouseButtonUp(SDL_MouseButtonEvent event);
    void OnMouseButtonDoubleClick(SDL_MouseButtonEvent event);
    void OnMouseMove(SDL_MouseButtonEvent event);
    void OnWheel(SDL_MouseWheelEvent event);
    void OnFileChanged(const char* filename);

    Image framebuffer;
    Image canvas;
	FloatImage zBuffer;

    // 3D scene
    Mesh* shared_mesh = nullptr;
    std::vector<Entity*> entities;
    std::vector<Color> entity_colors;
    Camera* camera = nullptr;

    // Camera orbit
    float cam_near = 0.1f;
    float cam_far  = 1000.0f;
    float cam_fov  = 45.0f;
    float orbit_yaw      = 0.0f;
    float orbit_pitch    = 0.0f;
    float orbit_distance = 6.0f;
    bool is_orbiting = false;
    bool is_panning  = false;
    void UpdateCameraFromOrbit();
    void UpdateCameraProjection();

    int current_lab     = 4;
    int current_task    = 1;
    int current_subtask = 0;

    Mesh*    quad_mesh        = nullptr;
    Shader*  formula_shader   = nullptr;
    Shader*  filter_shader    = nullptr;
    Shader*  transform_shader = nullptr;
    Shader*  raster_shader    = nullptr;
    Texture* filter_texture   = nullptr;

    enum ShadingMode { GOURAUD, PHONG };
    ShadingMode shading_mode = GOURAUD;

    bool use_color_tex  = false;
    bool use_spec_tex   = false;
    bool use_normal_tex = false;

    int num_lights = 1;

    Material* gouraud_material = nullptr;
    Material* phong_material   = nullptr;

    static const int MAX_SCENE_LIGHTS = 8;
    sLight scene_lights[MAX_SCENE_LIGHTS];
    Vector3 ambient_light;

    sUniformData uniform_data;

    Application(const char* caption, int width, int height);
    ~Application();

    void Init(void);
    void Render(void);
    void Update(float dt);

    void SetWindowSize(int width, int height) {
        glViewport(0, 0, width, height);
        this->window_width = width;
        this->window_height = height;
        this->framebuffer.Resize(width, height);
        UpdateCameraProjection();
    }

    Vector2 GetWindowSize()
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return Vector2(float(w), float(h));
    }
};
