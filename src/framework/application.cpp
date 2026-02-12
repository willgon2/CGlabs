#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include "entity.h"
#include "camera.h"
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

    Vector3 eye = target + offset;

    camera->LookAt(eye, target, Vector3(0.0f, 1.0f, 0.0f));
    UpdateCameraProjection();
}

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;
    framebuffer.Fill(Color::BLACK);

    shared_mesh = new Mesh();
    shared_mesh->LoadOBJ("meshes/lee.obj");

    zBuffer.Resize(window_width, window_height);

    Image* myTexture = new Image();
    myTexture->LoadTGA("textures/lee_color_specular.tga", true);

    Entity* e0 = new Entity();
    e0->mesh = shared_mesh;
    e0->texture = myTexture; 
    e0->base_position = Vector3(0.0f, 0.0f, 0.0f);
    e0->rotation_speed = 1.0f;
    e0->scale_base = 1.0f;
    e0->scale_amp = 0.15f;
    e0->phase = 0.0f;

    Entity* e1 = new Entity();
    e1->mesh = shared_mesh;
    e1->texture = myTexture;
    e1->base_position = Vector3(-1.6f, 0.0f, 0.0f);
    e1->rotation_speed = -1.6f;
    e1->scale_base = 1.25f;
    e1->scale_amp = 0.20f;
    e1->phase = 1.5f;

    Entity* e2 = new Entity();
    e2->mesh = shared_mesh;
    e2->texture = myTexture;
    e2->base_position = Vector3(1.6f, 0.0f, 0.0f);
    e2->rotation_speed = 2.2f;
    e2->scale_base = 0.85f;
    e2->scale_amp = 0.25f;
    e2->phase = 3.0f;

    entities.clear();
    entities.push_back(e0);
    entities.push_back(e1);
    entities.push_back(e2);

    entity_colors.clear();
    entity_colors.push_back(Color::WHITE);
    entity_colors.push_back(Color::BLUE);
    entity_colors.push_back(Color::RED);

    camera = new Camera();
    camera->center = Vector3(0.0f, 0.25f, 0.0f);

    cam_near = 0.1f;
    cam_far = 1000.0f;
    cam_fov = 45.0f;

    orbit_distance = 6.0f;
    orbit_yaw = 0.0f;
    orbit_pitch = 0.0f;

    UpdateCameraFromOrbit();

    // Cargar iconos
    icons_loaded = true;
    icons_loaded &= iconLine.LoadPNG("images/line.png", true);
    icons_loaded &= iconRect.LoadPNG("images/rectangle.png", true);
    icons_loaded &= iconTri.LoadPNG("images/triangle.png", true);
    icons_loaded &= iconPencil.LoadPNG("images/pencil.png", true);
    icons_loaded &= iconEraser.LoadPNG("images/eraser.png", true);
    icons_loaded &= iconBlack.LoadPNG("images/black.png", true);
    icons_loaded &= iconWhite.LoadPNG("images/white.png", true);
    icons_loaded &= iconRed.LoadPNG("images/red.png", true);
    icons_loaded &= iconGreen.LoadPNG("images/green.png", true);
    icons_loaded &= iconBlue.LoadPNG("images/blue.png", true);
    icons_loaded &= iconYellow.LoadPNG("images/yellow.png", true);

    // Crear botones (posición en toolbar)
    btnLine = Button(&iconLine, Vector2(8, 8), Button::BTN_LINE);
    btnRect = Button(&iconRect, Vector2(48, 8), Button::BTN_RECT);
    btnTri = Button(&iconTri, Vector2(88, 8), Button::BTN_TRIANGLE);
    btnPencil = Button(&iconPencil, Vector2(128, 8), Button::BTN_PENCIL);
    btnEraser = Button(&iconEraser, Vector2(168, 8), Button::BTN_ERASER);

    btnBlack = Button(&iconBlack, Vector2(240, 8), Button::BTN_COLOR_BLACK);
    btnWhite = Button(&iconWhite, Vector2(280, 8), Button::BTN_COLOR_WHITE);
    btnRed = Button(&iconRed, Vector2(320, 8), Button::BTN_COLOR_RED);
    btnGreen = Button(&iconGreen, Vector2(360, 8), Button::BTN_COLOR_GREEN);
    btnBlue = Button(&iconBlue, Vector2(400, 8), Button::BTN_COLOR_BLUE);
    btnYellow = Button(&iconYellow, Vector2(440, 8), Button::BTN_COLOR_YELLOW);
}

// Render one frame
void Application::Render(void)
{
    // 1) Base: lo persistente
    framebuffer = canvas;
	zBuffer.Fill(10000.0f);

    if (camera)
    {
        if (scene_mode == MODE_SINGLE)
        {
            if (!entities.empty() && entities[0])
                entities[0]->Render(&framebuffer, camera, &zBuffer);
        }
        else
        {
            for (size_t i = 0; i < entities.size(); ++i)
            {
                if (entities[i])
                {
                    Color c = Color::WHITE;
                    if (i < entity_colors.size())
                        c = entity_colors[i];
                    entities[i]->Render(&framebuffer, camera, &zBuffer);
                }
            }
        }
    }

    // 2) Preview mientras arrastras (no se guarda)
    if (is_drawing && (mouse_state & SDL_BUTTON_LMASK))
    {
        Vector2 end_pos = mouse_position;

        if (current_tool == TOOL_LINE)
        {
            framebuffer.DrawLineDDA((int)start_pos.x, (int)start_pos.y,
                (int)end_pos.x, (int)end_pos.y, current_color);
        }
        else if (current_tool == TOOL_RECT)
        {
            int x0 = (int)start_pos.x, y0 = (int)start_pos.y;
            int x1 = (int)end_pos.x, y1 = (int)end_pos.y;

            int rx = std::min(x0, x1);
            int ry = std::min(y0, y1);
            int rw = std::abs(x1 - x0) + 1;
            int rh = std::abs(y1 - y0) + 1;

            framebuffer.DrawRect(rx, ry, rw, rh, current_color, 2, false, Color::BLACK);
        }
        else if (current_tool == TOOL_TRIANGLE)
        {
            int x0 = (int)start_pos.x, y0 = (int)start_pos.y;
            int x1 = (int)end_pos.x, y1 = (int)end_pos.y;

            int left = std::min(x0, x1);
            int right = std::max(x0, x1);
            int top = std::min(y0, y1);
            int bottom = std::max(y0, y1);

            int cx = (left + right) / 2;

            Vector2 p0((float)cx, (float)top);
            Vector2 p1((float)left, (float)bottom);
            Vector2 p2((float)right, (float)bottom);

            framebuffer.DrawTriangle(p0, p1, p2, current_color, false, Color::BLACK);
        }
    }


    // 4) Presentar
    framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{
    time += seconds_elapsed;

    if (scene_mode == MODE_MULTI)
    {
        for (size_t i = 0; i < entities.size(); ++i)
            if (entities[i])
                entities[i]->Update(seconds_elapsed);
    }
}

//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym) {
    case SDLK_ESCAPE: exit(0); break;

       
        // T: Toggle Texture
    case SDLK_t:
        for (auto e : entities) {
            if (e) e->use_texture = !e->use_texture;
        }
        std::cout << "Texture toggled" << std::endl;
        break;

        // Z: Toggle Z-Buffer (Occlusions)
    case SDLK_z:
        for (auto e : entities) {
            if (e) e->use_zbuffer = !e->use_zbuffer;
        }
        std::cout << "Z-Buffer toggled" << std::endl;
        break;

        // C: Toggle Interpolated Colors vs Plain
    case SDLK_c:
        for (auto e : entities) {
            if (e) e->use_interpolation = !e->use_interpolation;
        }
        std::cout << "Interpolation toggled" << std::endl;
        break;

        // W: Toggle Wireframe Mode
    case SDLK_w:
        for (auto e : entities) {
            if (e) {
                // Toggle between WIREFRAME and TRIANGLES_INTERPOLATED
                if (e->mode == eRenderMode::WIREFRAME)
                    e->mode = eRenderMode::TRIANGLES_INTERPOLATED;
                else
                    e->mode = eRenderMode::WIREFRAME;
            }
        }
        std::cout << "Wireframe toggled" << std::endl;
        break;

        // 1: Draw Single Entity
    case SDLK_1:
        scene_mode = MODE_SINGLE;
        std::cout << "Mode: Single Entity" << std::endl;
        break;

        // 2: Draw Multiple Animated Entities
    case SDLK_2:
        scene_mode = MODE_MULTI;
        std::cout << "Mode: Multi Entity" << std::endl;
        break;

        // N: Select Camera Near Plane
    case SDLK_n:
        current_property = PROP_NEAR;
        std::cout << "Property Selected: Camera Near" << std::endl;
        break;

        // F: Select Camera Far Plane
    case SDLK_f:
        current_property = PROP_FAR;
        std::cout << "Property Selected: Camera Far" << std::endl;
        break;

        // V: Select Field of View (FOV)
    case SDLK_v:
        current_property = PROP_FOV;
        std::cout << "Property Selected: Camera FOV" << std::endl;
        break;

        // +: Increase Current Property
    case SDLK_PLUS:
    case SDLK_KP_PLUS:
    case SDLK_EQUALS:
        if (current_property == PROP_NEAR) {
            cam_near = std::min(cam_near + 0.1f, cam_far - 1.0f);
            std::cout << "Near Plane: " << cam_near << std::endl;
        }
        else if (current_property == PROP_FAR) {
            cam_far = cam_far + 10.0f;
            std::cout << "Far Plane: " << cam_far << std::endl;
        }
        else if (current_property == PROP_FOV) {
            cam_fov = std::min(cam_fov + 1.0f, 170.0f);
            std::cout << "FOV: " << cam_fov << std::endl;
        }
        UpdateCameraProjection(); 
        break;

       
    case SDLK_MINUS:
    case SDLK_KP_MINUS:
        if (current_property == PROP_NEAR) {
            cam_near = std::max(cam_near - 0.1f, 0.01f);
            std::cout << "Near Plane: " << cam_near << std::endl;
        }
        else if (current_property == PROP_FAR) {
            cam_far = std::max(cam_far - 10.0f, cam_near + 1.0f);
            std::cout << "Far Plane: " << cam_far << std::endl;
        }
        else if (current_property == PROP_FOV) {
            cam_fov = std::max(cam_fov - 1.0f, 10.0f);
            std::cout << "FOV: " << cam_fov << std::endl;
        }
        UpdateCameraProjection(); 
        break;
    }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    int fx = event.x;
    int fy = (int)framebuffer.height - 1 - event.y;

    mouse_position = Vector2((float)fx, (float)fy);
    mouse_delta = Vector2(0.0f, 0.0f);

    // clicks en toolbar (solo izquierda, como antes)
    if (event.button == SDL_BUTTON_LEFT && fy < TOOLBAR_H)
    {
        Vector2 mp((float)fx, (float)fy);

        if (btnLine.IsMouseInside(mp)) { current_tool = TOOL_LINE; is_drawing = false; return; }
        if (btnRect.IsMouseInside(mp)) { current_tool = TOOL_RECT; is_drawing = false; return; }
        if (btnTri.IsMouseInside(mp)) { current_tool = TOOL_TRIANGLE; is_drawing = false; return; }
        if (btnPencil.IsMouseInside(mp)) { current_tool = TOOL_PENCIL; is_drawing = false; return; }
        if (btnEraser.IsMouseInside(mp)) { current_tool = TOOL_ERASER; is_drawing = false; return; }

        if (btnBlack.IsMouseInside(mp)) { current_color = Color::BLACK; is_drawing = false; return; }
        if (btnWhite.IsMouseInside(mp)) { current_color = Color::WHITE; is_drawing = false; return; }
        if (btnRed.IsMouseInside(mp)) { current_color = Color::RED; is_drawing = false; return; }
        if (btnGreen.IsMouseInside(mp)) { current_color = Color::GREEN; is_drawing = false; return; }
        if (btnBlue.IsMouseInside(mp)) { current_color = Color::BLUE; is_drawing = false; return; }
        if (btnYellow.IsMouseInside(mp)) { current_color = Color::YELLOW; is_drawing = false; return; }

        is_drawing = false;
        return;
    }

    // 2.5 - Camera controls in the 3D area
    if (fy >= TOOLBAR_H)
    {
        if (event.button == SDL_BUTTON_LEFT)
        {
            is_orbiting = true;
            mouse_state |= SDL_BUTTON_LMASK;
            return;
        }
        if (event.button == SDL_BUTTON_RIGHT)
        {
            is_panning = true;
            mouse_state |= SDL_BUTTON_RMASK;
            return;
        }
    }

  
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)
    {
        mouse_state &= ~SDL_BUTTON_LMASK;
        is_orbiting = false;
        is_drawing = false;
    }
    else if (event.button == SDL_BUTTON_RIGHT)
    {
        mouse_state &= ~SDL_BUTTON_RMASK;
        is_panning = false;
    }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    int fx = event.x;
    int fy = (int)framebuffer.height - 1 - event.y;

    Vector2 newPos((float)fx, (float)fy);
    mouse_delta = newPos - mouse_position;
    mouse_position = newPos;

    if (fy < TOOLBAR_H)
        return;

    // 2.5 - Orbit (left button)
    if (is_orbiting && (mouse_state & SDL_BUTTON_LMASK))
    {
        float sens = 0.01f;
        orbit_yaw -= mouse_delta.x * sens;
        orbit_pitch += mouse_delta.y * sens;

        float limit = 1.5f;
        if (orbit_pitch > limit) orbit_pitch = limit;
        if (orbit_pitch < -limit) orbit_pitch = -limit;

        UpdateCameraFromOrbit();
    }

    // 2.5 - Pan target (right button)
    if (is_panning && (mouse_state & SDL_BUTTON_RMASK) && camera)
    {
        Vector3 forward = camera->center - camera->eye;
        forward.Normalize();

        Vector3 right = forward.Cross(Vector3(0.0f, 1.0f, 0.0f));
        right.Normalize();

        Vector3 up = right.Cross(forward);
        up.Normalize();

        float sens = 0.002f * orbit_distance;

        camera->center = camera->center - right * (mouse_delta.x * sens) + up * (mouse_delta.y * sens);

        UpdateCameraFromOrbit();
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
    // 2.5 - Zoom with wheel
    float dy = event.preciseY;

    orbit_distance -= dy * 0.5f;
    if (orbit_distance < 0.5f) orbit_distance = 0.5f;
    if (orbit_distance > 50.0f) orbit_distance = 50.0f;

    UpdateCameraFromOrbit();
}

void Application::OnMouseButtonDoubleClick(SDL_MouseButtonEvent event)
{
}

void Application::OnFileChanged(const char* filename)
{
    Shader::ReloadSingleShader(filename);
}