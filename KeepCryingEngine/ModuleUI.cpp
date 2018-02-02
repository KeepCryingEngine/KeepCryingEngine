#include "ModuleUI.h"

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <GL/glew.h>
#include <Windows.h>
#include <fstream>

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleScene.h"

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

	magFilter = App->renderer->getMagFilter();
	minFilter = App->renderer->getMinFilter();
	mipmap = App->renderer->getMipmap();
	anisotropicFilter = App->renderer->getAnisotropicFilter();

	SetUpTextEditor();

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
	ImVec2 buttonSize = { 120.0f, 20.0f };

	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	{
		CallWindows();

		if(ImGui::BeginMainMenuBar())
		{
			if(ImGui::BeginMenu("Windows"))
			{
				if (ImGui::Button("Inspector", buttonSize))
				{
					inspectorWindow ^= 1;
				}
				if(ImGui::Button("Camera Controls", buttonSize))
				{
					cameraWindow ^= 1;
				}
				if(ImGui::Button("Speed Controls", buttonSize))
				{
					speedWindow ^= 1;
				}
				if(ImGui::Button("Style Controls", buttonSize))
				{
					styleWindow ^= 1;
				}
				ImGui::EndMenu();
			}

			static int textureMode = 0;
			if(ImGui::BeginMenu("Texture"))
			{
				ImGui::Combo("Texture", &textureMode, "None\0Debug\0Lenna\0Exodia\0Rock");
				if(ImGui::Button("Texture Controls", buttonSize))
				{
					imageInfoWindow ^= 1;
				}
				ImGui::EndMenu();
			}

			App->renderer->SetCurrentTexture(textureMode);
			static GLfloat color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			if(ImGui::BeginMenu("Parameters"))
			{
				ImGui::Checkbox(" Wireframe Mode", &wireframeEnabled);
				ImGui::Checkbox(" Textures", &textureEnabled);
				ImGui::Checkbox(" Depth Test", &depthEnabled);
				ImGui::Checkbox(" Cull Face", &cullEnabled);
				ImGui::Checkbox(" Color Material", &colormaterialEnabled);
				ImGui::Checkbox(" Antialiasing", &antialiasingEnabled);				
				ImGui::Checkbox(" Fog", &fogEnabled);

				if(ImGui::BeginMenu("Fog parameters"))
				{
					ImGui::SliderFloat(" Density", &fogDensity, 0.0f, 1.0f);
					ImGui::ColorPicker3(" Color", color);
					ImGui::EndMenu();
				}

				//FOG Parameters
				glFogf(GL_FOG_DENSITY, fogDensity);
				glFogfv(GL_FOG_COLOR, color);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light"))
			{
				ImGui::Checkbox(" Lightning", &lightningEnabled);
				if (ImGui::ColorPicker3(" Ambient Light", App->renderer->globalAmbient))
				{
					if (!wireframeEnabled)
					{
						glLightModelfv(GL_LIGHT_MODEL_AMBIENT, App->renderer->globalAmbient);
					}				
				}
				ImGui::EndMenu();
			}

			SetAllParameters();

			static int shaderMode = 0;
			if(ImGui::BeginMenu("Shader Editor"))
			{
				ImGui::Combo("Shaders", &shaderMode, "None\0Fragment\0Vertex");
				if(ImGui::Button("Edit", buttonSize))
				{
					SetTextOnEditor(shaderMode);
					shaderEditorWindow ^= 1;
				}
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

void ModuleUI::SetAllParameters()
{
	//glEnable Parameters
	if(textureEnabled)
	{
		if(glIsEnabled(GL_TEXTURE_2D) == GL_FALSE)
		{
			glEnable(GL_TEXTURE_2D);
		}
	}
	else
	{
		if(glIsEnabled(GL_TEXTURE_2D) == GL_TRUE)
		{
			glDisable(GL_TEXTURE_2D);
		}
	}

	if(depthEnabled)
	{
		if(glIsEnabled(GL_DEPTH_TEST) == GL_FALSE)
		{
			glEnable(GL_DEPTH_TEST);
		}
	}
	else
	{
		if(glIsEnabled(GL_DEPTH_TEST) == GL_TRUE)
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	if(cullEnabled)
	{
		if(glIsEnabled(GL_CULL_FACE) == GL_FALSE)
		{
			glEnable(GL_CULL_FACE);
		}
	}
	else
	{
		if(glIsEnabled(GL_CULL_FACE) == GL_TRUE)
		{
			glDisable(GL_CULL_FACE);
		}
	}

	if(lightningEnabled)
	{
		if(glIsEnabled(GL_LIGHTING) == GL_FALSE)
		{
			glEnable(GL_LIGHTING);
		}
	}
	else
	{
		if(glIsEnabled(GL_LIGHTING) == GL_TRUE)
		{
			glDisable(GL_LIGHTING);
		}
	}

	if(colormaterialEnabled)
	{
		if(glIsEnabled(GL_COLOR_MATERIAL) == GL_FALSE)
		{
			glEnable(GL_COLOR_MATERIAL);
		}
	}
	else
	{
		if(glIsEnabled(GL_COLOR_MATERIAL) == GL_TRUE)
		{
			glDisable(GL_COLOR_MATERIAL);
		}
	}

	if(fogEnabled)
	{
		if(glIsEnabled(GL_FOG) == GL_FALSE)
		{
			glEnable(GL_FOG);
		}
	}
	else
	{
		if(glIsEnabled(GL_FOG) == GL_TRUE)
		{
			glDisable(GL_FOG);
		}
	}

	if(antialiasingEnabled)
	{
		if(glIsEnabled(GL_LINE_SMOOTH) == GL_FALSE && glIsEnabled(GL_BLEND) == GL_FALSE)
		{
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
		}
	}
	else
	{
		if(glIsEnabled(GL_LINE_SMOOTH) == GL_TRUE && glIsEnabled(GL_BLEND) == GL_TRUE)
		{
			glDisable(GL_BLEND);
			glDisable(GL_LINE_SMOOTH);
		}
	}

	static GLint polygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
	if(wireframeEnabled)
	{
		if(polygonMode == GL_FILL)
		{
			GLfloat brightYellow[] = {255.0f, 255.0f, 0.1f, 1.0f};
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, brightYellow);
			cullEnabled = false;
			textureEnabled = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}		
	}
	else
	{
		if(polygonMode == GL_LINE)
		{
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, App->renderer->globalAmbient);
			cullEnabled = true;
			textureEnabled = true;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

void ModuleUI::SetUpTextEditor()
{
	editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

void ModuleUI::SetTextOnEditor(int shaderMode)
{
	const char* shaderPath = "";

	switch(shaderMode)
	{
		case 0:
			shaderSavePath = nullptr;
			return;
		case 1:
			shaderPath = ".\\Assets\\Shaders\\fragmentShader.frag";
			break;
		case 2:
			shaderPath = ".\\Assets\\Shaders\\vertexShader.vert";
			break;
	}

	shaderSavePath = shaderPath;

	std::ifstream t(shaderPath);
	if(t.good())
	{
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		editor.SetText(str);
	}

	t.close();
}

void ModuleUI::CallWindows()
{
	if(cameraWindow)
	{
		DrawCameraWindow();
	}	
	if(speedWindow)
	{
		DrawSpeedWindow();
	}
	if(styleWindow)
	{
		DrawStyleWindow();
	}
	if(imageInfoWindow)
	{
		DrawTextureInfoWindow();
	}
	if(shaderEditorWindow)
	{
		DrawShaderWindow();
	}
	if (inspectorWindow)
	{
		DrawInspectorWindow();
	}
}

void ModuleUI::DrawCameraWindow()
{
	ImGui::Begin("Camera Controls", &cameraWindow, ImGuiWindowFlags_MenuBar);
	ImGui::CloseCurrentPopup();
	ImGui::InputFloat3("Front", front.ptr(), 2);
	front = App->camera->GetFrontVector();
	ImGui::InputFloat3("Up", up.ptr(), 2);
	up = App->camera->GetUpVector();
	ImGui::InputFloat3("Side", side.ptr(), 2);
	side = App->camera->GetSideVector();
	ImGui::InputFloat3("Position", position.ptr(), 2);

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
	ImGui::End();
}

void ModuleUI::DrawSpeedWindow()
{
	ImGui::Begin("Speed Controls", &speedWindow, ImGuiWindowFlags_MenuBar);
	ImGui::DragFloat("Movement", &movementSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
	App->camera->SetMoveSpeed(movementSpeed);
	ImGui::DragFloat("Rotation", &rotationSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
	App->camera->SetRotationSpeed(rotationSpeed);
	ImGui::DragFloat("Drag", &dragSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
	App->camera->SetDragSpeed(dragSpeed);
	ImGui::DragFloat("Orbit", &orbitSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
	App->camera->SetOrbitSpeed(orbitSpeed);
	ImGui::DragFloat("Zoom", &zoomSpeed, 0.5f, 0.0f, 100.0f, "%.2f");
	App->camera->SetZoomSpeed(zoomSpeed);
	ImGui::End();
}

void ModuleUI::DrawStyleWindow()
{
	ImGui::Begin("Style Controls", &styleWindow, ImGuiWindowFlags_MenuBar);
	ImGui::DragFloat3("Background", clearColor, 0.01f, 0.0f, 1.0f, "%.2f");
	ImGui::End();
}

void ModuleUI::DrawTextureInfoWindow()
{
	static int wrapModeS = 0;
	static int wrapModeT = 0;
	static int magFilterMode = 0;
	static int minFilterMode = 0;

	const char* wrapModeContent = "GL_CLAMP\0GL_REPEAT\0GL_MIRRORED_REPEAT\0GL_CLAMP_TO_EDGE\0GL_CLAMP_TO_BORDER";
	const char* magMinFilterModeContent = "GL_LINEAR\0GL_NEAREST\0GL_NEAREST_MIPMAP_NEAREST\0GL_LINEAR_MIPMAP_NEAREST\0GL_NEAREST_MIPMAP_LINEAR\0GL_LINEAR_MIPMAP_LINEAR";

	int previousWrapModeS = wrapModeS;
	int previousWrapModeT = wrapModeT;
	int previousMagFilterMode = magFilterMode;
	int previousMinFilterMode = minFilterMode;

	ImGui::Begin("Texture Controls", &textureEnabled, ImGuiWindowFlags_MenuBar);

	int width = App->renderer->getImageWidth();
	int height = App->renderer->getImageHeight();
	int sizeBytes = App->renderer->getImageSizeBytes();
	int pixelDepth = App->renderer->getImagePixelDepth();
	int format = App->renderer->getImageFormat();

	ImGui::InputInt("Width", &width);
	ImGui::InputInt("Height", &height);
	ImGui::InputInt("SizeBytes", &sizeBytes);
	ImGui::InputInt("PixelDepth", &pixelDepth);
	ImGui::InputInt("Format", &format);

	ImGui::Combo("WrapModeS", &wrapModeS, wrapModeContent);
	ImGui::Combo("WrapModeT", &wrapModeT, wrapModeContent);

	if(ImGui::Checkbox("MagFilter", &magFilter))
	{
		App->renderer->setMagFilter(magFilter);
	}
	if(ImGui::Checkbox("MinFilter", &minFilter))
	{
		App->renderer->setMinFilter(minFilter);
	}

	ImGui::Combo("MagFilterMode", &magFilterMode, magMinFilterModeContent);
	ImGui::Combo("MinFilterMode", &minFilterMode, magMinFilterModeContent);

	if(ImGui::Checkbox("Mipmap", &mipmap))
	{
		App->renderer->setMipmap(mipmap);
	}
	if(ImGui::Checkbox("AnisotropicFilter", &anisotropicFilter))
	{
		App->renderer->setAnisotropicFilter(anisotropicFilter);
	}

	ImGui::End();
	
	UpdateWrapModeS(wrapModeS, previousWrapModeS);
	UpdateWrapModeT(wrapModeT, previousWrapModeT);

	UpdateMagFilterMode(magFilterMode, previousMagFilterMode);
	UpdateMinFilterMode(minFilterMode, previousMinFilterMode);
}

void ModuleUI::DrawShaderWindow()
{
	ImGui::Begin("Text Editor Demo", &shaderEditorWindow, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Save"))
			{
				std::string textToSave = editor.GetText();
				if(shaderSavePath != nullptr)
				{
					std::ofstream shaderSave(shaderSavePath);
					shaderSave << textToSave;
					shaderSave.close();
				}
			}
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("View"))
		{
			if(ImGui::MenuItem("Dark palette"))
				editor.SetPalette(TextEditor::GetDarkPalette());
			if(ImGui::MenuItem("Light palette"))
				editor.SetPalette(TextEditor::GetLightPalette());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("BONES TARDES TINGUIN VOSTES :)");

	editor.Render("TextEditor");
	ImGui::End();
}

void ModuleUI::DrawInspectorWindow()
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected;

	ImGui::Begin("Game Object Inspector", &inspectorWindow, ImGuiWindowFlags_MenuBar);

	//if (App->scene->GetRoot().GetChilds().size() > 0)
	//{
		if (ImGui::TreeNodeEx(App->scene->GetRoot().GetName().c_str(), nodeFlags))
		{
			PrintChildrenOnHierarchy(App->scene->GetRoot().GetChilds());
			ImGui::TreePop();
		}
	//}
	//else
	//{
	//	ImGui::BulletText(App->scene->GetRoot().GetName().c_str());
	//}

	ImGui::End();
}

void ModuleUI::PrintChildrenOnHierarchy(std::vector<GameObject*> children)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

	for (GameObject* child : children)
	{
		if (child->GetChilds().size() > 0)
		{
			if (ImGui::TreeNodeEx(child->GetName().c_str(), nodeFlags))
			{
				PrintChildrenOnHierarchy(child->GetChilds());
				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::BulletText(child->GetName().c_str());
		}
	}
}

void ModuleUI::UpdateWrapModeS(uint wrapModeS, uint previousWrapModeS) const
{
	if(wrapModeS != previousWrapModeS)
	{
		switch(wrapModeS)
		{
			case 0: // GL_CLAMP
				App->renderer->setWrapModeS(GL_CLAMP);
				break;
			case 1: // GL_REPEAT
				App->renderer->setWrapModeS(GL_REPEAT);
				break;
			case 2: // GL_MIRRORED_REPEAT
				App->renderer->setWrapModeS(GL_MIRRORED_REPEAT);
				break;
			case 3: // GL_CLAMP_TO_EDGE
				App->renderer->setWrapModeS(GL_CLAMP_TO_EDGE);
				break;
			case 4: // GL_CLAMP_TO_BORDER
				App->renderer->setWrapModeS(GL_CLAMP_TO_BORDER);
				break;
		}
	}
}

void ModuleUI::UpdateWrapModeT(uint wrapModeT, uint previousWrapModeT) const
{
	if(wrapModeT != previousWrapModeT)
	{
		switch(wrapModeT)
		{
			case 0: // GL_CLAMP
				App->renderer->setWrapModeT(GL_CLAMP);
				break;
			case 1: // GL_REPEAT
				App->renderer->setWrapModeT(GL_REPEAT);
				break;
			case 2: // GL_MIRRORED_REPEAT
				App->renderer->setWrapModeT(GL_MIRRORED_REPEAT);
				break;
			case 3: // GL_CLAMP_TO_EDGE
				App->renderer->setWrapModeT(GL_CLAMP_TO_EDGE);
				break;
			case 4: // GL_CLAMP_TO_BORDER
				App->renderer->setWrapModeT(GL_CLAMP_TO_BORDER);
				break;
		}
	}
}

void ModuleUI::UpdateMagFilterMode(uint magFilterMode, uint previousMagFilterMode) const
{
	if(magFilterMode != previousMagFilterMode)
	{
		switch(magFilterMode)
		{
			case 0: // GL_LINEAR
				App->renderer->setMagFilterMode(GL_LINEAR);
				break;
			case 1: // GL_NEAREST
				App->renderer->setMagFilterMode(GL_NEAREST);
				break;
			case 2: // GL_NEAREST_MIPMAP_NEAREST
				App->renderer->setMagFilterMode(GL_NEAREST_MIPMAP_NEAREST);
				break;
			case 3: // GL_LINEAR_MIPMAP_NEAREST
				App->renderer->setMagFilterMode(GL_LINEAR_MIPMAP_NEAREST);
				break;
			case 4: // GL_NEAREST_MIPMAP_LINEAR
				App->renderer->setMagFilterMode(GL_NEAREST_MIPMAP_LINEAR);
				break;
			case 5: // GL_LINEAR_MIPMAP_LINEAR
				App->renderer->setMagFilterMode(GL_LINEAR_MIPMAP_LINEAR);
				break;
		}
	}
}

void ModuleUI::UpdateMinFilterMode(uint minFilterMode, uint previousMinFilterMode) const
{
	if(minFilterMode != previousMinFilterMode)
	{
		switch(minFilterMode)
		{
			case 0: // GL_LINEAR
				App->renderer->setMinFilterMode(GL_LINEAR);
				break;
			case 1: // GL_NEAREST
				App->renderer->setMinFilterMode(GL_NEAREST);
				break;
			case 2: // GL_NEAREST_MIPMAP_NEAREST
				App->renderer->setMinFilterMode(GL_NEAREST_MIPMAP_NEAREST);
				break;
			case 3: // GL_LINEAR_MIPMAP_NEAREST
				App->renderer->setMinFilterMode(GL_LINEAR_MIPMAP_NEAREST);
				break;
			case 4: // GL_NEAREST_MIPMAP_LINEAR
				App->renderer->setMinFilterMode(GL_NEAREST_MIPMAP_LINEAR);
				break;
			case 5: // GL_LINEAR_MIPMAP_LINEAR
				App->renderer->setMinFilterMode(GL_LINEAR_MIPMAP_LINEAR);
				break;
		}
	}
}

void ModuleUI::DrawAboutMenu()
{
	if(ImGui::Button("KeepCrying Engine"))
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