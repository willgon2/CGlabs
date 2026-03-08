#include "material.h"
#include "shader.h"

Material::Material()
{
}

void Material::Enable(const sUniformData& data, bool first_pass)
{
	if (!shader)
		return;

	shader->Enable();

	// --- Matrices ---
	shader->SetMatrix44("u_model",           data.model);
	shader->SetMatrix44("u_viewprojection",  data.viewprojection);

	// --- Scene ---
	shader->SetVector3("u_camera_position",  data.camera_position);
	shader->SetVector3("u_ambient_light",    data.ambient_light);

	// --- Active light (first light in the array for now; multipass loops externally) ---
	// The application ensures the correct light is in lights[0] for each pass
	if (data.num_lights > 0)
	{
		shader->SetVector3("u_light_position", data.lights[0].position);
		shader->SetVector3("u_light_color",    data.lights[0].color);
	}

	// --- Material properties ---
	shader->SetVector3("u_Ka",        Ka);
	shader->SetVector3("u_Kd",        Kd);
	shader->SetVector3("u_Ks",        Ks);
	shader->SetFloat("u_shininess",   shininess);

	// --- Multipass flag ---
	shader->SetInt("u_first_pass", first_pass ? 1 : 0);

	// --- Texture flags ---
	shader->SetInt("u_use_color_texture",  use_color_texture  ? 1 : 0);
	shader->SetInt("u_use_spec_texture",   use_spec_texture   ? 1 : 0);
	shader->SetInt("u_use_normal_texture", use_normal_texture ? 1 : 0);

	// --- Bind textures ---
	if (use_color_texture && color_texture)
		shader->SetTexture("u_color_texture", color_texture);

	if (use_normal_texture && normal_texture)
		shader->SetTexture("u_normal_texture", normal_texture);

	// If spec texture is active, it uses the alpha of the color texture (already bound above)
	// so no extra binding is needed.
}

void Material::Disable()
{
	if (shader)
		shader->Disable();
}
