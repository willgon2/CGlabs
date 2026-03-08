/*
    + This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

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

    // Camera orbit controls (mouse)
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

    // -------------------------------------------------------
    // LAB 4 / 5  state
    // -------------------------------------------------------

    // Which lab are we showing? L key toggles between 4 and 5
    int current_lab     = 4;

    // Lab 4: which task (1=formulas, 2=filters, 3=transforms, 4=3D mesh)
    int current_task    = 1;

    // Lab 4 tasks 1-3: which subtask (0='a' ... 5='f')
    int current_subtask = 0;

    // -------------------------------------------------------
    // Lab 4 GPU resources  (loaded once in Init, used in Render)
    // -------------------------------------------------------

    // Full-screen quad mesh (covers [-1,1] x [-1,1] in clip space)
    Mesh* quad_mesh = nullptr;

    // Shaders: one per task (vertex shader is shared: quad.vs)
    Shader* formula_shader   = nullptr;  // Task 2.2
    Shader* filter_shader    = nullptr;  // Task 2.3
    Shader* transform_shader = nullptr;  // Task 2.4
    Shader* raster_shader    = nullptr;  // Task 2.5

    // GPU texture for filter/transform tasks (fruits.png uploaded to GPU)
    Texture* filter_texture = nullptr;

    // -------------------------------------------------------
    // Lab 5 state  (added in later steps)
    // -------------------------------------------------------

    // Which shading model: G key = Gouraud, P key = Phong
    enum ShadingMode { GOURAUD, PHONG };
    ShadingMode shading_mode = GOURAUD;

    // Texture toggles (C, S, N keys in Lab 5)
    bool use_color_tex  = false;
    bool use_spec_tex   = false;
    bool use_normal_tex = false;

    // Number of lights (keys 1, 2 in Lab 5)
    int num_lights = 1;

    // The two materials for Lab 5
    Material* gouraud_material = nullptr;
    Material* phong_material   = nullptr;

    // Scene lights and ambient (filled in Init)
    static const int MAX_SCENE_LIGHTS = 8;
    sLight scene_lights[MAX_SCENE_LIGHTS];
    Vector3 ambient_light;

    // Struct passed through the rendering chain in Lab 5
    sUniformData uniform_data;

    // -------------------------------------------------------
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
        UpdateCameraProjection();   // keep aspect ratio correct on resize
    }

    Vector2 GetWindowSize()
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return Vector2(float(w), float(h));
    }
};
