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

	shader->SetMatrix44("u_model",          data.model);
	shader->SetMatrix44("u_viewprojection", data.viewprojection);

	shader->SetVector3("u_camera_position", data.camera_position);
	shader->SetVector3("u_ambient_light",   data.ambient_light);

	if (data.num_lights > 0)
	{
		shader->SetVector3("u_light_position", data.lights[0].position);
		shader->SetVector3("u_light_color",    data.lights[0].color);
	}

	shader->SetVector3("u_Ka",      Ka);
	shader->SetVector3("u_Kd",      Kd);
	shader->SetVector3("u_Ks",      Ks);
	shader->SetFloat("u_shininess", shininess);

	shader->SetInt("u_first_pass",          first_pass ? 1 : 0);
	shader->SetInt("u_use_color_texture",   use_color_texture  ? 1 : 0);
	shader->SetInt("u_use_spec_texture",    use_spec_texture   ? 1 : 0);
	shader->SetInt("u_use_normal_texture",  use_normal_texture ? 1 : 0);

	if (use_color_texture && color_texture)
		shader->SetTexture("u_color_texture", color_texture);

	if (use_normal_texture && normal_texture)
		shader->SetTexture("u_normal_texture", normal_texture);

	// spec uses alpha channel of color_texture, already bound above
}

void Material::Disable()
{
	if (shader)
		shader->Disable();
}
