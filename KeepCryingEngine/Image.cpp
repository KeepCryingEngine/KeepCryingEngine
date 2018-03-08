#include "Image.h"

#include "Application.h"
#include "ModuleTexture.h"
#include "Texture.h"

using namespace std;

Image::Image(): Component(Image::TYPE)
{}

Image::~Image()
{}

void Image::DrawUI()
{
	if (ImGui::CollapsingHeader("Image"))
	{

		static char texturePath[252] = "Lenna.png";
		ImGui::InputText("##setTexture", texturePath, 252); ImGui::SameLine();
		if (ImGui::Button("Set Texture"))
		{
			std::experimental::filesystem::path path = "Assets/";
			path /= texturePath;

			SetTextureByPath(path);
		}
	}
}

void Image::SetTextureByPath(const std::experimental::filesystem::path & path)
{
	if (texture != nullptr)
		App->texture->Release(texture);

	texture = App->texture->GetAsset(path);
}

Texture * Image::GetTexture() const
{
	return texture;
}
