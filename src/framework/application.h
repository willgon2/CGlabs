/*
    + This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "button.h"
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

    Mesh* shared_mesh = nullptr;
    std::vector<Entity*> entities;
    std::vector<Color> entity_colors;
    Camera* camera = nullptr;

    // 2.5 - Interactivity state
    enum SceneMode { MODE_SINGLE = 0, MODE_MULTI = 1 };
    SceneMode scene_mode = MODE_MULTI;

    enum Property { PROP_NONE = 0, PROP_NEAR, PROP_FAR, PROP_FOV };
    Property current_property = PROP_NONE;

    float cam_near = 0.1f;
    float cam_far = 1000.0f;
    float cam_fov = 45.0f;

    float orbit_yaw = 0.0f;
    float orbit_pitch = 0.0f;
    float orbit_distance = 6.0f;

    bool is_orbiting = false; // left mouse
    bool is_panning = false;  // right mouse

    void UpdateCameraFromOrbit();
    void UpdateCameraProjection();

    Image iconLine, iconRect, iconTri;
    Image iconPencil, iconEraser;
    Image iconBlack, iconWhite, iconRed, iconGreen, iconBlue, iconYellow;
    bool icons_loaded = false;

    Button btnLine;
    Button btnRect;
    Button btnTri;
    Button btnPencil;
    Button btnEraser;
    Button btnBlack;
    Button btnWhite;
    Button btnRed;
    Button btnGreen;
    Button btnBlue;
    Button btnYellow;

    enum Tool { TOOL_LINE, TOOL_RECT, TOOL_TRIANGLE, TOOL_PENCIL, TOOL_ERASER };
    Tool current_tool = TOOL_LINE;

    Color current_color = Color::WHITE;
    int brush_size = 1;

    static constexpr int TOOLBAR_H = 48;

    bool is_drawing = false;
    Vector2 start_pos;

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
    }

    Vector2 GetWindowSize()
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return Vector2(float(w), float(h));
    }
};