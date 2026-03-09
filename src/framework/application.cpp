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

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;
    framebuffer.Fill(Color::BLACK);

    // camera
    camera = new Camera();
    camera->center = Vector3(0.0f, 0.25f, 0.0f);
    cam_near = 0.1f;
    cam_far  = 1000.0f;
    cam_fov  = 45.0f;
    orbit_distance = 6.0f;
    orbit_yaw   = 0.0f;
    orbit_pitch = 0.0f;
    UpdateCameraFromOrbit();

    // mesh
    shared_mesh = new Mesh();
    shared_mesh->LoadOBJ("meshes/lee.obj");

    // entities
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

    // Lab 4 GPU resources
    quad_mesh = new Mesh();
    quad_mesh->CreateQuad();

    formula_shader   = Shader::Get("shaders/quad.vs",   "shaders/quad.fs");
    filter_shader    = Shader::Get("shaders/quad.vs",   "shaders/filter.fs");
    transform_shader = Shader::Get("shaders/quad.vs",   "shaders/transform.fs");
    raster_shader    = Shader::Get("shaders/raster.vs", "shaders/raster.fs");

    filter_texture = Texture::Get("images/fruits.png");

    Texture* model_texture = Texture::Get("textures/lee_color_specular.tga");
    for (auto e : entities)
    {
        e->shader      = raster_shader;
        e->gpu_texture = model_texture;
    }

    // Lab 5 materials
    gouraud_material = new Material();
    gouraud_material->shader = Shader::Get("shaders/gouraud.vs", "shaders/gouraud.fs");

    phong_material = new Material();
    phong_material->shader         = Shader::Get("shaders/phong.vs", "shaders/phong.fs");
    phong_material->color_texture  = model_texture;
    phong_material->normal_texture = Texture::Get("textures/lee_normal.tga");

    phong_material->use_color_texture  = false;
    phong_material->use_spec_texture   = false;
    phong_material->use_normal_texture = false;

    // Light 0: warm white from front-right
    scene_lights[0] = sLight(Vector3(3.0f, 5.0f, 4.0f),   Vector3(1.0f, 1.0f, 1.0f));
    // Light 1: cool blue from the left
    scene_lights[1] = sLight(Vector3(-4.0f, 2.0f, -2.0f), Vector3(0.3f, 0.3f, 1.0f));

    ambient_light = Vector3(0.1f, 0.1f, 0.1f);

    for (auto e : entities)
        e->material = gouraud_material;
}

void Application::Render(void)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Lab 4
    if (current_lab == 4)
    {
        if (current_task == 1)
        {
            formula_shader->Enable();
            formula_shader->SetFloat("u_time",   time);
            formula_shader->SetFloat("u_aspect", (float)window_width / (float)window_height);
            formula_shader->SetInt("u_subtask",  current_subtask);
            quad_mesh->Render();
            formula_shader->Disable();
        }
        else if (current_task == 2)
        {
            filter_shader->Enable();
            filter_shader->SetInt("u_subtask", current_subtask);
            filter_shader->SetTexture("u_texture", filter_texture);
            quad_mesh->Render();
            filter_shader->Disable();
        }
        else if (current_task == 3)
        {
            transform_shader->Enable();
            transform_shader->SetFloat("u_time",   time);
            transform_shader->SetFloat("u_aspect", (float)window_width / (float)window_height);
            transform_shader->SetInt("u_subtask",  current_subtask);
            transform_shader->SetTexture("u_texture", filter_texture);
            quad_mesh->Render();
            transform_shader->Disable();
        }
        else if (current_task == 4)
        {
            glEnable(GL_DEPTH_TEST);
            for (auto e : entities)
                if (e) e->Render(camera);
            glDisable(GL_DEPTH_TEST);
        }
    }

    // Lab 5
    else if (current_lab == 5)
    {
        sUniformData data;
        data.viewprojection  = camera->GetViewProjectionMatrix();
        data.camera_position = camera->eye;
        data.ambient_light   = ambient_light;
        data.time            = time;
        data.num_lights      = num_lights;
        for (int i = 0; i < num_lights; ++i)
            data.lights[i] = scene_lights[i];

        Material* mat = (shading_mode == PHONG) ? phong_material : gouraud_material;

        if (phong_material)
        {
            phong_material->use_color_texture  = use_color_tex;
            phong_material->use_spec_texture   = use_spec_tex;
            phong_material->use_normal_texture = use_normal_tex;
        }

        for (auto e : entities)
            if (e) e->material = mat;

        // first pass: normal depth write, ambient included
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);

        data.first_pass = true;
        data.lights[0]  = scene_lights[0];
        for (auto e : entities)
            if (e) e->Render(data);

        // additional passes: additive blending, no ambient
        for (int li = 1; li < num_lights; ++li)
        {
            data.first_pass = false;
            data.lights[0]  = scene_lights[li];

            glDepthFunc(GL_LEQUAL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            for (auto e : entities)
                if (e) e->Render(data);
        }

        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST);

        if (mat) mat->Disable();
    }
}

void Application::Update(float seconds_elapsed)
{
    time += seconds_elapsed;

    for (auto e : entities)
        if (e) e->Update(seconds_elapsed);
}

void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym)
    {
    case SDLK_ESCAPE:
        exit(0);
        break;

    // L: toggle lab
    case SDLK_l:
        current_lab = (current_lab == 4) ? 5 : 4;
        std::cout << "Lab: " << current_lab << std::endl;
        break;

    // 1-4: task (Lab 4) or light count (Lab 5)
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

    // subtask keys
    case SDLK_a: current_subtask = 0; std::cout << "Subtask a\n"; break;
    case SDLK_b: current_subtask = 1; std::cout << "Subtask b\n"; break;
    case SDLK_d: current_subtask = 3; std::cout << "Subtask d\n"; break;
    case SDLK_e: current_subtask = 4; std::cout << "Subtask e\n"; break;

    case SDLK_c:
        if (current_lab == 5) { use_color_tex = !use_color_tex; std::cout << "Colour tex: " << use_color_tex << "\n"; }
        else                  { current_subtask = 2; std::cout << "Subtask c\n"; }
        break;

    case SDLK_f:
        if (current_lab == 4) { current_subtask = 5; std::cout << "Subtask f\n"; }
        break;

    case SDLK_g:
        shading_mode = GOURAUD;
        std::cout << "Gouraud shading\n";
        break;
    case SDLK_p:
        shading_mode = PHONG;
        std::cout << "Phong shading\n";
        break;

    case SDLK_s:
        if (current_lab == 5) { use_spec_tex = !use_spec_tex; std::cout << "Specular tex: " << use_spec_tex << "\n"; }
        break;

    case SDLK_n:
        if (current_lab == 5) { use_normal_tex = !use_normal_tex; std::cout << "Normal tex: " << use_normal_tex << "\n"; }
        break;
    }
}

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

    // orbit
    if (is_orbiting && (mouse_state & SDL_BUTTON_LMASK))
    {
        float sens    = 0.01f;
        orbit_yaw   -= mouse_delta.x * sens;
        orbit_pitch  += mouse_delta.y * sens;
        orbit_pitch   = std::max(-1.5f, std::min(1.5f, orbit_pitch));
        UpdateCameraFromOrbit();
    }

    // pan
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
    Shader::ReloadSingleShader(filename);
}
