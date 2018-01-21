#include "ModuleUI.h"

#include <imgui_impl_sdl_gl3.h>
#include "GL/glew.h"
#include <SDL.h>
#include <Windows.h>
#include <shellapi.h>

#include "Application.h"
#include "ModuleWindow.h"

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

update_status ModuleUI::Update(float deltaTimeS, float realDeltaTimeS)
{
	DrawMainFrame();
	
	if(showAboutFrame)
	{
		DrawAboutFrame();
	}

	if(showDemoFrame)
	{
		DrawDemoFrame();
	}

	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();

	return update_status::UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleUI::DrawMainFrame()
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!"); // Some text (you can use a format string too)
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float as a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats as a color
		if(ImGui::Button("Demo Window")) // Use buttons to toggle our bools. We could use Checkbox() as well.
			showDemoFrame ^= 1;
		if(ImGui::Button("About"))
			showAboutFrame ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

void ModuleUI::DrawDemoFrame()
{
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
	ImGui::ShowDemoWindow(&showDemoFrame);
}

void ModuleUI::DrawAboutFrame()
{
	ImGui::Begin("About", &showAboutFrame);

	if(ImGui::Button("KeepCryingEngine"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/KeepCryingEngine/KeepCryingEngine"), nullptr, nullptr, 0);
	}

	ImGui::Text("Improved CryEngine version");

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
	if(ImGui::Button("Pere Senor de los Loros"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/PereViader"), nullptr, nullptr, 0);
	}
	if(ImGui::Button("Xavier Bravo Guillen"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/BravoXavi"), nullptr, nullptr, 0);
	}
	ImGui::Unindent();

	ImGui::Text("3rd Party Libraries used:");

	ImGui::Indent();
	ImGui::BulletText("SLD 2.0");
	ImGui::BulletText("Glew 2.1");
	ImGui::BulletText("ImGui 1.53");
	ImGui::BulletText("MathGeolib 1.5");
	ImGui::BulletText("OpenGL 3.1");
	ImGui::Unindent();

	ImGui::Text("License:");

	ImGui::Indent();
	if(ImGui::Button("MIT License"))
	{
		ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/KeepCryingEngine/KeepCryingEngine/blob/master/LICENSE"), nullptr, nullptr, 0);
	}
	ImGui::Unindent();

	ImGui::End();
}