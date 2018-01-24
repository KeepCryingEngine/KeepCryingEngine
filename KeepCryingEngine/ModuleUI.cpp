#include "ModuleUI.h"

#include <imgui_impl_sdl_gl3.h>
#include "GL/glew.h"
#include <SDL.h>
#include <Windows.h>
#include <shellapi.h>

#include "Application.h"
#include "ModuleWindow.h"
#include "Globals.h"

ModuleUI::ModuleUI()
{ }

ModuleUI::~ModuleUI()
{ }

bool ModuleUI::Init()
{
	ImGui_ImplSdlGL3_Init(App->window->window);
	ImGui::StyleColorsClassic();

	return true;
}

bool ModuleUI::Start()
{
	nearPlane = App->camera->GetNearPlane();
	farPlane = App->camera->GetFarPlane();
	aspectRatio = App->camera->GetAspectRatio();
	verticalFOV = App->camera->GetFOV();
	position = App->camera->GetPosition();
	up = App->camera->GetUpVector();
	front = App->camera->GetFrontVector();
	side = App->camera->GetSideVector();
	movementSpeed = App->camera->GetMoveSpeed();
	rotationSpeed = App->camera->GetRotationSpeed();
	dragSpeed = App->camera->GetDragSpeed();
	orbitSpeed = App->camera->GetOrbitSpeed();
	zoomSpeed = App->camera->GetZoomSpeed();

	return true;
}

update_status ModuleUI::Update(float deltaTimeS, float realDeltaTimeS)
{	
	DrawMainMenu();

	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float deltaTimeS, float realDeltaTimeS)
{
	ImGui::Render();

	return update_status::UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleUI::DrawMainMenu()
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	{
		if(ImGui::BeginMainMenuBar())
		{
			if(ImGui::BeginMenu("Windows"))
			{
				DrawToolsMenu();
				ImGui::EndMenu();
			}

			if(ImGui::BeginMenu("About"))
			{
				DrawAboutMenu();
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}

void ModuleUI::DrawToolsMenu()
{
	if(ImGui::BeginMenu("Tools"))
	{
		ImGui::InputFloat3("Front", front.ptr(), 2);
		front = App->camera->GetFrontVector();
		ImGui::InputFloat3("Up", up.ptr(), 2);
		up = App->camera->GetUpVector();
		ImGui::InputFloat3("Side", side.ptr(), 2);
		side = App->camera->GetSideVector();
		ImGui::InputFloat3("Camera Pos.", position.ptr(), 2);
		
		ImGui::NewLine();

		position = App->camera->GetPosition();
		ImGui::DragFloat("Near plane", &nearPlane, 0.01f, 0.01f, 2.0f, "%.2f");
		App->camera->SetNearPlane(nearPlane);
		ImGui::DragFloat("Far plane", &farPlane, 2.0f, 50.0f, 300.0f, "%.2f");
		App->camera->SetFarPlane(farPlane);
		ImGui::SliderFloat("Field of View", &verticalFOV, 0.1f, pi);
		App->camera->SetFOV(verticalFOV);
		aspectRatio = App->camera->GetAspectRatio();
		ImGui::InputFloat("Aspect ratio", &aspectRatio);	
		ImGui::EndMenu();
	}
	if(ImGui::BeginMenu("Speeds"))
	{
		ImGui::DragFloat("Mov. speed", &movementSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
		App->camera->SetMoveSpeed(movementSpeed);
		ImGui::DragFloat("Rot. speed", &rotationSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
		App->camera->SetRotationSpeed(rotationSpeed);
		ImGui::DragFloat("Drag speed", &dragSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
		App->camera->SetDragSpeed(dragSpeed);
		ImGui::DragFloat("Orbit speed", &orbitSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
		App->camera->SetOrbitSpeed(orbitSpeed);
		ImGui::DragFloat("Zoom speed", &zoomSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
		App->camera->SetZoomSpeed(zoomSpeed);
		ImGui::EndMenu();
	}
	if(ImGui::BeginMenu("Style"))
	{
		ImGui::DragFloat3("Background color", clearColor, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::EndMenu();
	}
}

void ModuleUI::DrawAboutMenu()
{
	if(ImGui::Button("KeepCryingEngine"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/KeepCryingEngine/KeepCryingEngine"), nullptr, nullptr, 0);
	}

	ImGui::Text("Improved CryEngine version");

	ImGui::NewLine();

	ImGui::Text("Authors:");

	ImGui::Indent();
	if(ImGui::Button("Adrian Leroy Calle"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/Moaif"), nullptr, nullptr, 0);
	}
	if(ImGui::Button("Gerard Pros Figueras"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/gerardpf2"), nullptr, nullptr, 0);
	}
	if(ImGui::Button("Pere Viader Masuet"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/PereViader"), nullptr, nullptr, 0);
	}
	if(ImGui::Button("Xavier Bravo Guillen"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/BravoXavi"), nullptr, nullptr, 0);
	}
	ImGui::Unindent();

	ImGui::NewLine();

	ImGui::Text("3rd Party Libraries used:");

	ImGui::Indent();
	ImGui::BulletText("SDL 2.0");
	ImGui::BulletText("Glew 2.1");
	ImGui::BulletText("ImGui 1.53");
	ImGui::BulletText("MathGeolib 1.5");
	ImGui::BulletText("OpenGL 3.1");
	ImGui::Unindent();

	ImGui::NewLine();

	ImGui::Text("License:");

	ImGui::Indent();
	if(ImGui::Button("MIT License"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/KeepCryingEngine/KeepCryingEngine/blob/master/LICENSE"), nullptr, nullptr, 0);
	}
	ImGui::Unindent();
}