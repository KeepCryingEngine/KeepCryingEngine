#include "ModuleEditorUI.h"

#include <ImGuizmo.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <GL/glew.h>
#include <Windows.h>
#include <fstream>

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleEntity.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "AudioSource.h"
#include "AudioListener.h"
#include "ModuleTexture.h"
#include "ModuleAnim.h"
#include "Animator.h"
#include "MeshRenderer.h"
#include "ReverbZone.h"
#include "ModuleGameUI.h"
#include "Canvas.h"
#include "Image.h"
#include "Button.h"
#include "Text.h"
#include "InputText.h"
#include "GridRenderer.h"

using namespace std;

ModuleEditorUI::ModuleEditorUI()
{ }

ModuleEditorUI::~ModuleEditorUI()
{ }

bool ModuleEditorUI::Init()
{
	ImGui_ImplSdlGL3_Init(App->window->window);
	ImGui::StyleColorsClassic();
	return true;
}

bool ModuleEditorUI::Start()
{
	nearPlane = App->camera->camera->GetNearPlane();
	farPlane = App->camera->camera->GetFarPlane();
	aspectRatio = App->camera->camera->GetAspectRatio();
	verticalFOV = App->camera->camera->GetFOV();
	position = App->camera->camera->GetPosition();
	up = App->camera->camera->GetUpVector();
	front = App->camera->camera->GetFrontVector();
	side = App->camera->camera->GetSideVector();
	movementSpeed = App->camera->GetMoveSpeed();
	rotationSpeed = App->camera->GetRotationSpeed();
	dragSpeed = App->camera->GetDragSpeed();
	orbitSpeed = App->camera->GetOrbitSpeed();
	zoomSpeed = App->camera->GetZoomSpeed();

	SetUpTextEditor();

	return true;
}

update_status ModuleEditorUI::Update()
{	
	DrawMainMenu();

	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditorUI::PostUpdate()
{
	ImGui::Render();

	return update_status::UPDATE_CONTINUE;
}

bool ModuleEditorUI::GetFrustumCulling() const
{
	return frustumCulling;
}

bool ModuleEditorUI::GetDebugMode() const
{
	return debugMode;
}

ClickMode ModuleEditorUI::GetClickMode() const
{
	return clickMode;
}

void ModuleEditorUI::SetSelectedNodeID(int id)
{
	selectedNodeID = id;
}

int ModuleEditorUI::GetSelectedNode() const
{
	return selectedNodeID;
}

bool ModuleEditorUI::CleanUp()
{
	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleEditorUI::DrawMainMenu()
{
	ImVec2 buttonSize = { 120.0f, 20.0f };

	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGuizmo::BeginFrame();

	CallWindows();
	CallEntityCreation();

	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("Scene"))
		{
			if(ImGui::Button("Save"))
			{
				App->scene->Save();
			}

			if(ImGui::Button("Restore"))
			{
				App->scene->Restore();
			}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Windows"))
		{
			if (ImGui::Selectable("Hierarchy"))
			{
				hierarchyWindow ^= 1;
			}

			if(ImGui::BeginMenu("Control Panel"))
			{
				if(ImGui::Selectable("Camera"))
				{
					cameraWindow ^= 1;
				}
				if(ImGui::Selectable("Movement"))
				{
					speedWindow ^= 1;
				}
				if(ImGui::Selectable("Space Partitioning"))
				{
					spacePartitioningWindow ^= 1;
				}
				ImGui::EndMenu();
			}
			if(ImGui::Selectable("Editor Controler"))
			{
				editorControler ^= 1;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Load"))
		{
			ImGui::LabelText("##LoadMeshLabel", "Mesh");
			
			// TMP
			static char pathToMesh[180] = "Assets/ArmyPilot/ArmyPilot.dae";
			
			ImGui::InputText("##PathToMesh", pathToMesh, sizeof(pathToMesh)); ImGui::SameLine();
			if (ImGui::Button("Add mesh"))
			{
				std::experimental::filesystem::path path(pathToMesh);
				App->entity->Load3DFile(path);
			}

			ImGui::EndMenu();
		}
		static GLfloat color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		if(ImGui::BeginMenu("Parameters"))
		{
			ImGui::Checkbox(" Frustum Culling", &frustumCulling);
			ImGui::Checkbox(" Wireframe Mode", &wireframeEnabled);
			ImGui::Checkbox(" Textures", &textureEnabled);
			ImGui::Checkbox(" Depth Test", &depthEnabled);
			ImGui::Checkbox(" Cull Face", &cullEnabled);
			ImGui::Checkbox(" Color Material", &colormaterialEnabled);
			ImGui::Checkbox(" Antialiasing", &antialiasingEnabled);				
			ImGui::Checkbox(" Fog", &fogEnabled);
			ImGui::Checkbox(" Debug Mode", &debugMode);

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

		if(ImGui::BeginMenu("Personalization"))
		{
			if(ImGui::BeginMenu("Themes"))
			{
				if(ImGui::Selectable("Default Theme"))
				{
					ImGui::StyleColorsClassic();
				}
				if(ImGui::Selectable("Dark Theme"))
				{
					ImGui::StyleColorsDark();
				}
				if(ImGui::Selectable("Light Theme"))
				{
					ImGui::StyleColorsLight();
				}
				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("Background Color"))
			{
				ImGui::DragFloat3("##backgroundcolor", clearColor, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::EndMenu();
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

void ModuleEditorUI::SetAllParameters()
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

void ModuleEditorUI::DrawEditorControler()
{
	if(ImGui::Button("Play"))
	{
		App->Play();
	} ImGui::SameLine();
	if(ImGui::Button("Pause"))
	{
		App->Pause();
	} ImGui::SameLine();
	if(ImGui::Button("Stop"))
	{
		App->Stop();
	}

	float tempScale = App->time->GetTimeScale();
	if(ImGui::DragFloat("Time Scale",&tempScale,0.01f,0.0f,1.0f))
	{
		App->time->SetTimeScale(tempScale);
	}
}

void ModuleEditorUI::SetUpTextEditor()
{
	editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

void ModuleEditorUI::SetTextOnEditor(int shaderMode)
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

void ModuleEditorUI::OpenInspectorWindow()
{
	inspectorWindow = true;
}

void ModuleEditorUI::CloseInspectorWindow()
{
	inspectorWindow = false;
}

void ModuleEditorUI::CallEntityCreation()
{
	if(addEmptyGameObject)
	{
		App->scene->AddEmpty(*App->scene->Get(selectedNodeID));
		addEmptyGameObject = false;
	}
	if(addCubeGameObject)
	{
		App->scene->AddCube(*App->scene->Get(selectedNodeID));
		addCubeGameObject = false;
	}
	if(addSphereGameObject)
	{
		App->scene->AddSphere(*App->scene->Get(selectedNodeID));
		addSphereGameObject = false;
	}
	if(addCameraGameObject)
	{
		App->scene->AddCamera(*App->scene->Get(selectedNodeID));
		addCameraGameObject = false;
	}
	if(addCanvasGameObject)
	{
		App->scene->AddCanvas(*App->scene->Get(selectedNodeID));
		addCanvasGameObject = false;
	}
	if(addImageGameObject)
	{
		App->scene->AddImage(*App->scene->Get(selectedNodeID));
		addImageGameObject = false;
	}
	if(addButtonGameObject)
	{
		App->scene->AddButton(*App->scene->Get(selectedNodeID));
		addButtonGameObject = false;
	}
	if(addTextGameObject)
	{
		App->scene->AddText(*App->scene->Get(selectedNodeID));
		addTextGameObject = false;
	}
	if(addInputTextGameObject)
	{
		App->scene->AddInputText(*App->scene->Get(selectedNodeID));
		addInputTextGameObject = false;
	}
}

void ModuleEditorUI::CallWindows()
{
	if(cameraWindow)
	{
		DrawCameraWindow();
	}	
	if(speedWindow)
	{
		DrawSpeedWindow();
	}
	if(shaderEditorWindow)
	{
		DrawShaderWindow();
	}
	if (hierarchyWindow)
	{
		DrawHierarchyWindow();
	}
	if(inspectorWindow)
	{
		DrawInspectorWindow();
	}
	if(generateGameObjectWindow)
	{
		DrawGenerateGameObjectWindow();
	}
	if(spacePartitioningWindow)
	{
		DrawSpacePartitioningWindow();
	}
	if(loadedTexturesInfoWindow)
	{
		DrawLoadedTexturesInfoWindow();
	}
	if(editorControler)
	{
		DrawEditorControler();
	}
}

void ModuleEditorUI::CallGuizmo()
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };

	if (ImGui::RadioButton("Drag", clickMode == ClickMode::Drag))
	{
		clickMode = ClickMode::Drag;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("Pick", clickMode == ClickMode::Pick))
	{
		clickMode = ClickMode::Pick;
	}

	ImGui::Separator();

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
	{
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
	{
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
	{
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	//Xavo: Do not delete this or I'll haunt your dreams -----------------------
	//if(mCurrentGizmoOperation != ImGuizmo::SCALE)
	//{
	//	if(ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
	//		mCurrentGizmoMode = ImGuizmo::LOCAL;
	//	ImGui::SameLine();
	//	if(ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
	//		mCurrentGizmoMode = ImGuizmo::WORLD;
	//}
	//ImGui::Checkbox("", &useSnap);
	//--------------------------------------------------------------------------

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	if (selectedNodeID != 0)
	{
		Transform* transform = (Transform*)App->scene->Get(selectedNodeID)->GetTransform();
		float4x4 modelMatrix = transform->GetModelMatrix().Transposed();
		float4x4 viewMatrix = App->camera->camera->GetViewMatrix();
		float4x4 projectionMatrix = App->camera->camera->GetProyectionMatrix();

		ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, modelMatrix.ptr(), nullptr, useSnap ? &snap[0] : NULL);
				
		if(ImGuizmo::IsUsing())
		{ 
			transform->SetModelMatrix(modelMatrix.Transposed());
		}	
	}
}

void ModuleEditorUI::DrawCameraWindow()
{
	ImGui::Begin("Camera Controls", &cameraWindow, ImGuiWindowFlags_MenuBar);
	ImGui::CloseCurrentPopup();
	ImGui::InputFloat3("Front", front.ptr(), 2);
	front = App->camera->camera->GetFrontVector();
	ImGui::InputFloat3("Up", up.ptr(), 2);
	up = App->camera->camera->GetUpVector();
	ImGui::InputFloat3("Side", side.ptr(), 2);
	side = App->camera->camera->GetSideVector();
	ImGui::InputFloat3("Position", position.ptr(), 2);

	ImGui::NewLine();

	position = App->camera->camera->GetPosition();
	ImGui::DragFloat("Near plane", &nearPlane, 0.01f, 0.01f, 2.0f, "%.2f");
	App->camera->camera->SetNearPlane(nearPlane);
	ImGui::DragFloat("Far plane", &farPlane, 2.0f, 10.0f, 1000.0f, "%.2f");
	App->camera->camera->SetFarPlane(farPlane);
	ImGui::SliderFloat("Field of View", &verticalFOV, 0.1f, pi);
	App->camera->camera->SetFOV(verticalFOV);
	aspectRatio = App->camera->camera->GetAspectRatio();
	ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.1f, 0.1f, 5.0f, "%.2f");
	App->camera->camera->SetAspectRatio(aspectRatio);
	ImGui::End();
}

void ModuleEditorUI::DrawSpeedWindow()
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

void ModuleEditorUI::DrawShaderWindow()
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

void ModuleEditorUI::DrawHierarchyWindow()
{
	ImGui::Begin("Game Object Hierarchy", &hierarchyWindow);

	CallGuizmo();

	ImGui::Separator();

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;

	if(selectedNodeID == App->scene->GetRoot()->GetId())
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	else
	{
		nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
	}

	bool opened = ImGui::TreeNodeEx(App->scene->GetRoot()->GetName().c_str(), nodeFlags);

	ImGui::PushStyleColor(0, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
	if(ImGui::BeginPopupContextItem("Add Item ..."))
	{
		if(ImGui::BeginMenu("Add..."))
		{
			if(ImGui::Selectable("Empty"))
			{
				addEmptyGameObject = true;
			}
			if(ImGui::Selectable("Cube"))
			{
				addCubeGameObject = true;
			}
			if(ImGui::Selectable("Sphere"))
			{
				addSphereGameObject = true;
			}
			if(ImGui::Selectable("Camera"))
			{
				addCameraGameObject = true;
			}
			if(ImGui::BeginMenu("UI"))
			{
				if(App->uiGame->GetCanvas() == nullptr)
				{
					if(ImGui::Selectable("Canvas"))
					{
						addCanvasGameObject = true;
					}
				}
				if(ImGui::Selectable("Image"))
				{
					addImageGameObject = true;
				}
				if(ImGui::Selectable("Button"))
				{
					addButtonGameObject = true;
				}
				if(ImGui::Selectable("Text"))
				{
					addTextGameObject = true;
				}
				if(ImGui::Selectable("InputText"))
				{
					addInputTextGameObject = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleColor();

	if(ImGui::IsItemClicked())
	{
		inspectorWindow = false;
		selectedNodeID = App->scene->GetRoot()->GetId();
	}

	if(ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* test = ImGui::AcceptDragDropPayload("GameObject");
		if(test)
		{
			(*(GameObject**)test->Data)->SetParent(*App->scene->GetRoot());
		}
		ImGui::EndDragDropTarget();
	}

	if(opened)
	{
		PrintChildrenOnHierarchy(App->scene->GetRoot()->GetChildren());
		ImGui::TreePop();
	}

	ImGui::End();
}

void ModuleEditorUI::DrawInspectorWindow()
{
	GameObject* temp = App->scene->Get(selectedNodeID);

	ImGui::Begin("Inspector", &inspectorWindow, ImGuiWindowFlags_MenuBar);

	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("Add"))
		{
			static int selectedComponent = 0;
			if(ImGui::Selectable("MeshRenderer"))
			{
				temp->AddComponent<MeshRenderer>();
			}
			if(ImGui::Selectable("Camera"))
			{
				temp->AddComponent<Camera>();
			}
			if(ImGui::Selectable("Animator"))
			{
				temp->AddComponent<Animator>();
			}
			if(ImGui::Selectable("GridRenderer"))
			{
				temp->AddComponent<GridRenderer>();
			}
			if(ImGui::BeginMenu("Audio"))
			{
				if(ImGui::Selectable("Audio Source"))
				{
					temp->AddComponent<AudioSource>();
				}
				if(ImGui::Selectable("Audio Listener"))
				{
					temp->AddComponent<AudioListener>();
				}
				if(ImGui::Selectable("Reverb Zone"))
				{
					temp->AddComponent<ReverbZone>();
				}
				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("UI"))
			{
				if(ImGui::Selectable("Image"))
				{
					temp->AddComponent<Image>();
				}
				if(ImGui::Selectable("Button"))
				{
					temp->AddComponent<Button>();
				}
				if(ImGui::Selectable("Text"))
				{
					temp->AddComponent<Text>();
				}
				if(ImGui::Selectable("InputText"))
				{
					temp->AddComponent<InputText>();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::PushStyleColor(0, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
		if(ImGui::BeginMenu("Delete"))
		{
			ImGui::PushStyleColor(0, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
			if(ImGui::Selectable("Confirm"))
			{
				inspectorWindow = false;
				App->scene->Destroy(*App->scene->Get(selectedNodeID));
			}
			ImGui::PopStyleColor();
			ImGui::EndMenu();
		}
		ImGui::PopStyleColor();
		ImGui::EndMenuBar();
	}

	if(selectedNodeID != 0)
	{
		temp->DrawUI();
	}
	
	ImGui::End();
}

void ModuleEditorUI::DrawGenerateGameObjectWindow()
{
	ImGui::Begin("Generate GameObject Controls", &generateGameObjectWindow, ImGuiWindowFlags_MenuBar);

	static int generateGameObjectWindowCount = 100;
	ImGui::DragInt("Count", &generateGameObjectWindowCount);

	static float generateGameObjectWindowStaticPercentage = 100.0f;
	ImGui::DragFloat("Static percentage", &generateGameObjectWindowStaticPercentage);

	static float generateGameObjectWindowLimitX[2] = { -100.0f, 100.0f };
	ImGui::DragFloat2("Limit X", generateGameObjectWindowLimitX);

	static float generateGameObjectWindowLimitY[2] = { -5.0f, 5.0f };
	ImGui::DragFloat2("Limit Y", generateGameObjectWindowLimitY);

	static float generateGameObjectWindowLimitZ[2] = { -100.0f, 100.0f };
	ImGui::DragFloat2("Limit Z", generateGameObjectWindowLimitZ);

	if(ImGui::Button("Generate"))
	{
		App->scene->Generate(generateGameObjectWindowCount, generateGameObjectWindowStaticPercentage, generateGameObjectWindowLimitX[0], generateGameObjectWindowLimitX[1], generateGameObjectWindowLimitY[0], generateGameObjectWindowLimitY[1], generateGameObjectWindowLimitZ[0], generateGameObjectWindowLimitZ[1]);
	}

	ImGui::End();
}

void ModuleEditorUI::DrawSpacePartitioningWindow()
{
	ImGui::Begin("Space Partitioning Controls", &spacePartitioningWindow, ImGuiWindowFlags_MenuBar);

	static int spacePartitioningWindowStructure = 0;
	
	if(ImGui::Combo("Structure", &spacePartitioningWindowStructure, "None\0Quadtree\0Octree\0KDTree"))
	{
		App->scene->SetSpacePartitioningStructure(spacePartitioningWindowStructure);
	}

	ImGui::End();
}

void ModuleEditorUI::DrawLoadedTexturesInfoWindow()
{
	ImGui::Begin("Loaded Textures Information", &loadedTexturesInfoWindow, ImGuiWindowFlags_MenuBar);

	ImGui::Text("Texture count: %u", App->texture->Size());
	ImGui::Text("Texture total size (bytes): %u", App->texture->TotalTextureSize());
	ImGui::Text("Loaded textures:");

	ImGui::Indent();

	for(const string& texturePath : App->texture->TexturePaths())
	{
		ImGui::Text("%s", texturePath.c_str());
	}

	ImGui::Unindent();

	ImGui::End();
}

void ModuleEditorUI::PrintChildrenOnHierarchy(std::vector<GameObject*> children)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	for(GameObject* child : children)
	{
		nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if(selectedNodeID == child->GetId())
		{
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}
		if(child->ChildCount() == 0)
		{
			nodeFlags |= ImGuiTreeNodeFlags_Bullet;
		}

		if(!child->IsEnabled())
		{
			ImGui::PushStyleColor(0, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		}

		ImGui::PushID(child->GetId());
		bool opened = ImGui::TreeNodeEx(child->GetName().c_str(), nodeFlags);				
		if(ImGui::BeginPopupContextItem("Add Item ..."))
		{
			if(ImGui::BeginMenu("Add..."))
			{
				if(ImGui::Selectable("Empty"))
				{
					addEmptyGameObject = true;
				}
				if(ImGui::Selectable("Cube"))
				{
					addCubeGameObject = true;
				}
				if(ImGui::Selectable("Sphere"))
				{
					addSphereGameObject = true;
				}
				if(ImGui::Selectable("Camera"))
				{
					addCameraGameObject = true;
				}
				if(ImGui::BeginMenu("UI"))
				{
					if(App->uiGame->GetCanvas() == nullptr)
					{
						if(ImGui::Selectable("Canvas"))
						{
							addCanvasGameObject = true;
						}
					}
					if(ImGui::Selectable("Image"))
					{
						addImageGameObject = true;
					}
					if(ImGui::Selectable("Button"))
					{
						addButtonGameObject = true;
					}
					if(ImGui::Selectable("Text"))
					{
						addTextGameObject = true;
					}
					if(ImGui::Selectable("InputText"))
					{
						addInputTextGameObject = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();

		if(ImGui::IsItemClicked())
		{
			if(!inspectorWindow)
			{
				inspectorWindow = true;
			}
			selectedNodeID = child->GetId();
		}

		if (ImGui::BeginDragDropSource())
		{		
			ImGui::SetDragDropPayload("GameObject", &child, sizeof(GameObject*));
			ImGui::EndDragDropSource();
		}

		if(ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* test = ImGui::AcceptDragDropPayload("GameObject");
			if(test)
			{
				(*(GameObject**)test->Data)->SetParent(*child);
			}
			ImGui::EndDragDropTarget();
		}

		if(opened)
		{
			if(child->ChildCount() > 0)
			{
				PrintChildrenOnHierarchy(child->GetChildren());
			}
			ImGui::TreePop();
		}

		if(!child->IsEnabled())
		{
			ImGui::PopStyleColor();
		}
	}
}

void ModuleEditorUI::DrawAboutMenu()
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