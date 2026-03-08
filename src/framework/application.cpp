#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "entity.h"
#include "camera.h"
#include "material.h"
#include "utils.h"
#include <algorithm>
#include <cmath>

Application::Application(const char* caption, int width, int height)
{
    this->window = createWindow(caption, width, height);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    this->mouse_state = 0;
    this->time = 0.f;
    this->window_width = w;
    this->window_height = h;
    this->keystate = SDL_GetKeyboardState(nullptr);

    // We keep the software framebuffer alive (used in old Lab3 mode if needed)
    this->framebuffer.Resize(w, h);
    this->canvas.Resize(w, h);
    this->canvas.Fill(Color::BLACK);
}

Application::~Application()
{
}

void Application::UpdateCameraProjection()
{
    if (!camera) return;
    camera->SetPerspective(cam_fov, (float)window_width / (float)window_height, cam_near, cam_far);
}

void Application::UpdateCameraFromOrbit()
{
    if (!camera) return;

    Vector3 target = camera->center;

    float cy = cosf(orbit_yaw);
    float sy = sinf(orbit_yaw);
    float cp = cosf(orbit_pitch);
    float sp = sinf(orbit_pitch);

    Vector3 offset;
    offset.x = orbit_distance * (sy * cp);
    offset.y = orbit_distance * (sp);
    offset.z = orbit_distance * (cy * cp);

    camera->LookAt(target + offset, target, Vector3(0.0f, 1.0f, 0.0f));
    UpdateCameraProjection();
}

// =============================================================
//  Init
//  Called ONCE at startup. Load ALL resources here.
//  NEVER load textures/meshes/shaders inside Render() or Update()
//  because those run ~60 times per second!
// =============================================================
void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;
    framebuffer.Fill(Color::BLACK);

    // ---- Camera ----
    camera = new Camera();
    camera->center = Vector3(0.0f, 0.25f, 0.0f);
    cam_near = 0.1f;
    cam_far  = 1000.0f;
    cam_fov  = 45.0f;
    orbit_distance = 6.0f;
    orbit_yaw   = 0.0f;
    orbit_pitch = 0.0f;
    UpdateCameraFromOrbit();

    // ---- 3D mesh ----
    shared_mesh = new Mesh();
    shared_mesh->LoadOBJ("meshes/lee.obj");

    // ---- Entities (animated 3D objects) ----
    // Three instances of the same mesh, each with different position/speed/phase
    Entity* e0 = new Entity();
    e0->mesh            = shared_mesh;
    e0->base_position   = Vector3(0.0f, 0.0f, 0.0f);
    e0->rotation_speed  = 1.0f;
    e0->scale_base      = 1.0f;
    e0->scale_amp       = 0.15f;
    e0->phase           = 0.0f;

    Entity* e1 = new Entity();
    e1->mesh            = shared_mesh;
    e1->base_position   = Vector3(-1.6f, 0.0f, 0.0f);
    e1->rotation_speed  = -1.6f;
    e1->scale_base      = 1.25f;
    e1->scale_amp       = 0.20f;
    e1->phase           = 1.5f;

    Entity* e2 = new Entity();
    e2->mesh            = shared_mesh;
    e2->base_position   = Vector3(1.6f, 0.0f, 0.0f);
    e2->rotation_speed  = 2.2f;
    e2->scale_base      = 0.85f;
    e2->scale_amp       = 0.25f;
    e2->phase           = 3.0f;

    entities.push_back(e0);
    entities.push_back(e1);
    entities.push_back(e2);

    // ================================================================
    // LAB 4 - GPU resources
    // ================================================================

    // Task 2.1 - Full-screen quad
    // CreateQuad() builds a rectangle covering [-1,1]x[-1,1] in clip space
    // with UV coordinates (0,0)-(1,1)
    quad_mesh = new Mesh();
    quad_mesh->CreateQuad();

    // Tasks 2.2 / 2.3 / 2.4 / 2.5 - Shaders
    // Shader::Get(vs_path, fs_path) compiles once and caches the shader.
    // Paths are relative to the working directory (= the res/ folder).
    formula_shader   = Shader::Get("shaders/quad.vs",   "shaders/quad.fs");
    filter_shader    = Shader::Get("shaders/quad.vs",   "shaders/filter.fs");
    transform_shader = Shader::Get("shaders/quad.vs",   "shaders/transform.fs");
    raster_shader    = Shader::Get("shaders/raster.vs", "shaders/raster.fs");

    // Task 2.3 / 2.4 - Load the image into a GPU Texture.
    // A CPU Image lives in RAM. A GPU Texture lives in VRAM.
    // We use Texture::Get() which loads and caches it.
    filter_texture = Texture::Get("images/fruits.png");

    // Task 2.5 - Each entity needs the raster shader and the model texture
    Texture* model_texture = Texture::Get("textures/lee_color_specular.tga");
    for (auto e : entities)
    {
        e->shader      = raster_shader;
        e->gpu_texture = model_texture;
    }

    // ================================================================
    // LAB 5 - Materials and lights
    // ================================================================

    // Gouraud material: lighting computed in the VERTEX shader
    gouraud_material = new Material();
    gouraud_material->shader = Shader::Get("shaders/gouraud.vs", "shaders/gouraud.fs");

    // Phong material: lighting computed in the FRAGMENT shader
    phong_material = new Material();
    phong_material->shader        = Shader::Get("shaders/phong.vs", "shaders/phong.fs");
    phong_material->color_texture = model_texture;
    phong_material->normal_texture = Texture::Get("textures/lee_normal.tga");

    // Default: textures are OFF (user turns them on with C / S / N keys)
    phong_material->use_color_texture  = false;
    phong_material->use_spec_texture   = false;
    phong_material->use_normal_texture = false;

    // Scene lights: position + color intensity
    // Light 0: warm white from front-right
    scene_lights[0] = sLight(Vector3(3.0f, 5.0f, 4.0f),  Vector3(1.0f, 1.0f, 1.0f));
    // Light 1: cool blue from the left
    scene_lights[1] = sLight(Vector3(-4.0f, 2.0f, -2.0f), Vector3(0.3f, 0.3f, 1.0f));

    // Ambient: very dim global light so nothing is completely black
    ambient_light = Vector3(0.1f, 0.1f, 0.1f);

    // Assign materials to entities (they start in Gouraud mode)
    for (auto e : entities)
        e->material = gouraud_material;
}

// =============================================================
//  Render  - called every frame (~60fps)
//  RULE: never load files here!
// =============================================================
void Application::Render(void)
{
    // Clear OpenGL's color and depth buffers every frame
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ---- LAB 4 ----
    if (current_lab == 4)
    {
        if (current_task == 1)
        {
            // Task 2.2 - Mathematical formula patterns
            // 1. Enable the shader (tells OpenGL: use these GPU programs)
            formula_shader->Enable();

            // 2. Upload uniforms:
            //    SetFloat / SetInt send CPU variables to the GPU shader.
            //    The string name MUST match the uniform name in the GLSL file.
            formula_shader->SetFloat("u_time",    time);
            formula_shader->SetFloat("u_aspect",  (float)window_width / (float)window_height);
            formula_shader->SetInt("u_subtask",   current_subtask);

            // 3. Draw the quad. The GPU runs quad.vs then quad.fs for each pixel.
            quad_mesh->Render();

            // 4. Disable when done
            formula_shader->Disable();
        }
        else if (current_task == 2)
        {
            // Task 2.3 - Image colour filters
            filter_shader->Enable();
            filter_shader->SetInt("u_subtask", current_subtask);
            // SetTexture binds the GPU texture to a texture slot and
            // tells the shader which slot to sample from (sampler2D)
            filter_shader->SetTexture("u_texture", filter_texture);
            quad_mesh->Render();
            filter_shader->Disable();
        }
        else if (current_task == 3)
        {
            // Task 2.4 - Time-based image transformations
            transform_shader->Enable();
            transform_shader->SetFloat("u_time",    time);
            transform_shader->SetFloat("u_aspect",  (float)window_width / (float)window_height);
            transform_shader->SetInt("u_subtask",   current_subtask);
            transform_shader->SetTexture("u_texture", filter_texture);
            quad_mesh->Render();
            transform_shader->Disable();
        }
        else if (current_task == 4)
        {
            // Task 2.5 - Rasterize the 3D mesh using the GPU
            // GL_DEPTH_TEST ensures closer objects occlude farther ones
            glEnable(GL_DEPTH_TEST);

            for (auto e : entities)
                if (e) e->Render(camera);   // new GPU render method (see entity.cpp)

            glDisable(GL_DEPTH_TEST);
        }
    }

    // ---- LAB 5 ----
    else if (current_lab == 5)
    {
        // Build the sUniformData that travels from Application -> Entity -> Material -> Shader
        sUniformData data;
        data.viewprojection  = camera->GetViewProjectionMatrix();
        data.camera_position = camera->eye;
        data.ambient_light   = ambient_light;
        data.time            = time;
        data.num_lights      = num_lights;
        // Copy scene lights into the struct
        for (int i = 0; i < num_lights; ++i)
            data.lights[i] = scene_lights[i];

        // Choose material based on G / P key
        Material* mat = (shading_mode == PHONG) ? phong_material : gouraud_material;

        // Update phong material texture toggles (C / S / N keys)
        if (phong_material)
        {
            phong_material->use_color_texture  = use_color_tex;
            phong_material->use_spec_texture   = use_spec_tex;
            phong_material->use_normal_texture = use_normal_tex;
        }

        // Assign active material to all entities
        for (auto e : entities)
            if (e) e->material = mat;

        // --- Multipass rendering ---
        // We render the scene ONCE PER LIGHT.
        // First pass: normal depth test, no blending, ambient IS added
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);

        data.first_pass  = true;
        data.lights[0]   = scene_lights[0];  // active light for this pass
        for (auto e : entities)
            if (e) e->Render(data);

        // Additional passes (one per extra light)
        // Each pass ADDS its light contribution on top of the previous result.
        // glBlendFunc(GL_ONE, GL_ONE) = src + dst (pure addition)
        // GL_LEQUAL allows re-drawing pixels at the same depth as the first pass
        for (int li = 1; li < num_lights; ++li)
        {
            data.first_pass = false;          // don't add ambient again
            data.lights[0]  = scene_lights[li];

            glDepthFunc(GL_LEQUAL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            for (auto e : entities)
                if (e) e->Render(data);
        }

        // Restore OpenGL state
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST);

        // Lab 5 spec: "don't forget to disable the material after all lights"
        if (mat) mat->Disable();
    }
}

// =============================================================
//  Update  - called every frame, before Render
// =============================================================
void Application::Update(float seconds_elapsed)
{
    time += seconds_elapsed;

    // Always keep entities animated (their model matrix changes each frame)
    for (auto e : entities)
        if (e) e->Update(seconds_elapsed);
}

// =============================================================
//  Keyboard input
// =============================================================
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym)
    {
    case SDLK_ESCAPE:
        exit(0);
        break;

    // --- L: toggle between Lab 4 and Lab 5 ---
    case SDLK_l:
        current_lab = (current_lab == 4) ? 5 : 4;
        std::cout << "Lab: " << current_lab << std::endl;
        break;

    // --- Keys 1-4: select task (Lab 4) or number of lights (Lab 5) ---
    case SDLK_1:
        if (current_lab == 4) { current_task = 1; std::cout << "Task 2.2: Formulas\n"; }
        else                  { num_lights = 1;   std::cout << "1 light\n"; }
        break;
    case SDLK_2:
        if (current_lab == 4) { current_task = 2; std::cout << "Task 2.3: Filters\n"; }
        else                  { num_lights = 2;   std::cout << "2 lights\n"; }
        break;
    case SDLK_3:
        if (current_lab == 4) { current_task = 3; std::cout << "Task 2.4: Transforms\n"; }
        break;
    case SDLK_4:
        if (current_lab == 4) { current_task = 4; std::cout << "Task 2.5: 3D GPU mesh\n"; }
        break;

    // --- Subtask keys a-f (Lab 4 tasks 1/2/3) ---
    case SDLK_a: current_subtask = 0; std::cout << "Subtask a\n"; break;
    case SDLK_b: current_subtask = 1; std::cout << "Subtask b\n"; break;
    case SDLK_d: current_subtask = 3; std::cout << "Subtask d\n"; break;
    case SDLK_e: current_subtask = 4; std::cout << "Subtask e\n"; break;

    // c: subtask c (Lab 4) OR toggle colour texture (Lab 5 Phong)
    case SDLK_c:
        if (current_lab == 5) { use_color_tex = !use_color_tex; std::cout << "Colour tex: " << use_color_tex << "\n"; }
        else                  { current_subtask = 2; std::cout << "Subtask c\n"; }
        break;

    // f: subtask f (Lab 4). Camera far key removed (not needed for this lab).
    case SDLK_f:
        if (current_lab == 4) { current_subtask = 5; std::cout << "Subtask f\n"; }
        break;

    // --- Lab 5 shading mode ---
    case SDLK_g:
        shading_mode = GOURAUD;
        std::cout << "Gouraud shading\n";
        break;
    case SDLK_p:
        shading_mode = PHONG;
        std::cout << "Phong shading\n";
        break;

    // s: toggle specular texture (Lab 5)
    case SDLK_s:
        if (current_lab == 5) { use_spec_tex = !use_spec_tex; std::cout << "Specular tex: " << use_spec_tex << "\n"; }
        break;

    // n: toggle normal texture (Lab 5)
    case SDLK_n:
        if (current_lab == 5) { use_normal_tex = !use_normal_tex; std::cout << "Normal tex: " << use_normal_tex << "\n"; }
        break;
    }
}

// =============================================================
//  Mouse input  (kept for camera orbit/pan/zoom)
// =============================================================
void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    int fx = event.x;
    int fy = event.y;

    mouse_position = Vector2((float)fx, (float)fy);
    mouse_delta    = Vector2(0.0f, 0.0f);

    if (event.button == SDL_BUTTON_LEFT)
    {
        is_orbiting   = true;
        mouse_state  |= SDL_BUTTON_LMASK;
    }
    if (event.button == SDL_BUTTON_RIGHT)
    {
        is_panning    = true;
        mouse_state  |= SDL_BUTTON_RMASK;
    }
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)
    {
        mouse_state &= ~SDL_BUTTON_LMASK;
        is_orbiting  = false;
    }
    else if (event.button == SDL_BUTTON_RIGHT)
    {
        mouse_state &= ~SDL_BUTTON_RMASK;
        is_panning   = false;
    }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    int fx = event.x;
    int fy = event.y;

    Vector2 newPos((float)fx, (float)fy);
    mouse_delta    = newPos - mouse_position;
    mouse_position = newPos;

    // Orbit with left button
    if (is_orbiting && (mouse_state & SDL_BUTTON_LMASK))
    {
        float sens    = 0.01f;
        orbit_yaw   -= mouse_delta.x * sens;
        orbit_pitch  += mouse_delta.y * sens;
        orbit_pitch   = std::max(-1.5f, std::min(1.5f, orbit_pitch));
        UpdateCameraFromOrbit();
    }

    // Pan target with right button
    if (is_panning && (mouse_state & SDL_BUTTON_RMASK) && camera)
    {
        Vector3 forward = camera->center - camera->eye;
        forward.Normalize();
        Vector3 right = forward.Cross(Vector3(0.0f, 1.0f, 0.0f));
        right.Normalize();
        Vector3 up = right.Cross(forward);
        up.Normalize();

        float sens = 0.002f * orbit_distance;
        camera->center = camera->center
                       - right * (mouse_delta.x * sens)
                       + up    * (mouse_delta.y * sens);
        UpdateCameraFromOrbit();
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
    orbit_distance -= event.preciseY * 0.5f;
    orbit_distance  = std::max(0.5f, std::min(50.0f, orbit_distance));
    UpdateCameraFromOrbit();
}

void Application::OnMouseButtonDoubleClick(SDL_MouseButtonEvent event)
{
}

void Application::OnFileChanged(const char* filename)
{
    // Hot-reload shaders when their file changes on disk
    Shader::ReloadSingleShader(filename);
}
