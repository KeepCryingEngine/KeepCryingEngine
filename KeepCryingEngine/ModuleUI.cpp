#include "ModuleUI.h"

#include <imgui_impl_sdl_gl3.h>
#include "GL/glew.h"
#include <SDL.h>
#include <Windows.h>
#include <shellapi.h>

#include "Application.h"
#include "ModuleWindow.h"

ModuleUI::ModuleUI()
{

}

ModuleUI::~ModuleUI()
{

}

bool ModuleUI::Init()
{
	ImGui_ImplSdlGL3_Init(App->window->window);
	ImGui::StyleColorsClassic();

	return true;
}

update_status ModuleUI::Update(float deltaTimeS, float realDeltaTimeS)
{
	
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");                           // Some text (you can use a format string too)
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float as a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats as a color
		if(ImGui::Button("Demo Window"))                       // Use buttons to toggle our bools. We could use Checkbox() as well.
			show_demo_window ^= 1;
		if(ImGui::Button("About"))
			show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	if(show_another_window)
	{
		ImGui::Begin("About", &show_another_window);
		ImGui::Text("Authors");
		if(ImGui::Button("Adrian Leroy Calle"))
		{
			ShellExecute(NULL, TEXT("open"), TEXT("https://github.com/Moaif"), NULL, NULL, 0);
		}
		if(ImGui::Button("Gerard Pros Figueras"))
		{
			ShellExecute(NULL, TEXT("open"), TEXT("https://github.com/gerardpf2"), NULL, NULL, 0);
		}
		if(ImGui::Button("Pere Senor de los Loros"))
		{
			ShellExecute(NULL, TEXT("open"), TEXT("https://github.com/PereViader"), NULL, NULL, 0);
		}
		if(ImGui::Button("Xavier Bravo Guillen"))
		{
			ShellExecute(NULL, TEXT("open"), TEXT("https://github.com/BravoXavi"), NULL, NULL, 0);
		}
		ImGui::End();
	}

	if(show_demo_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();

	return update_status::UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	ImGui_ImplSdlGL3_Shutdown();
	return true;
}