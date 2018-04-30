#include "ModuleRender.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include <DevIL.h>

#include "ModuleEntity.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"
#include "ModuleEditorUI.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "ModuleScene.h"
#include "ModuleAnim.h"

const float3 ModuleRender::LIGHT_DIR = { -1.0,1.0,0.0 };

using namespace std;

ModuleRender::ModuleRender()
{}

ModuleRender::~ModuleRender()
{}

bool ModuleRender::Init()
{
	LOG_DEBUG("Creating Renderer context");
	bool ret = true;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	glcontext = SDL_GL_CreateContext(App->window->window);

	LOG_DEBUG("Using Glew %s", glewGetString(GLEW_VERSION));
	LOG_DEBUG("Vendor: %s", glGetString(GL_VENDOR));
	LOG_DEBUG("Renderer: %s", glGetString(GL_RENDERER));
	LOG_DEBUG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG_DEBUG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	//Antialiasing
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetUpLight();

	GLenum err = glewInit();

	if(GLEW_OK != err)
	{
		LOG_DEBUG("GLEW could not initialize! GLEW_Error: %s", glewGetErrorString(err));
		ret = false;
	}

	//Initialize DevIL
	ilInit();
	iluInit();
	ilutInit();

	if(!ilutRenderer(ILUT_OPENGL))
	{
		LOG_DEBUG("DevIL could not initialize!");
		ret = false;
	}

	if(App->configuration.vsync)
	{
		SDL_GL_SetSwapInterval(1);
	}

	return ret;
}

update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::Update()
{
	Camera* cam = App->camera->GetPlayOrEditorCamera();
	if(cam != nullptr)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(cam->GetProyectionMatrix().ptr());
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(cam->GetViewMatrix().ptr());

		if(App->state == TimeState::STOPED)
		{
			DrawGrid();
		}

		DrawGeometry();

		if(App->uiEditor->GetDebugMode())
		{
			DrawLastRay();
		}
	}
	drawBuffer.clear();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	// DrawGeometry();
	// drawBuffer.clear();

	SDL_GL_SwapWindow(App->window->window);

	return update_status::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	SDL_GL_DeleteContext(glcontext);

	return true;
}

void ModuleRender::DrawGrid() const
{
	glPushMatrix();

	GLfloat previousColor[4];
	glGetFloatv(GL_CURRENT_COLOR, previousColor);

	glBegin(GL_LINES);

	glColor3f(220, 220, 220);

	float step = 1.0f;
	float length = 1000.0f;

	for (float r = -length; r <= length; r += step)
	{
		glVertex3f(-length, 0, r);
		glVertex3f(length, 0, r);

		glVertex3f(r, 0, -length);
		glVertex3f(r, 0, length);
	}

	glEnd();

	glColor3f(previousColor[0], previousColor[1], previousColor[2]);
	glPopMatrix();
}

void ModuleRender::DrawCross(const float3& pos,float scale)const
{
	float crossSize = 0.01f * scale;
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
	glLineWidth(3);

	GLfloat previousColor[4];
	glGetFloatv(GL_CURRENT_COLOR, previousColor);

	glBegin(GL_LINES);

	glColor3f(255, 0, 0);

	glVertex3f(-crossSize, 0, 0);
	glVertex3f(crossSize, 0, 0);

	glVertex3f(0, -crossSize, 0);
	glVertex3f(0, crossSize, 0);

	glEnd();

	glColor3f(previousColor[0], previousColor[1], previousColor[2]);
	glLineWidth(1);

	glPopMatrix();
}

void ModuleRender::AddToDrawBuffer(Mesh & mesh, Material& material, GameObject& gameObject, Transform& transform)
{
	DrawInfo drawCall = { mesh, material, gameObject, transform };
	drawBuffer.push_back(drawCall);
}

void ModuleRender::DrawFrustum(Camera & camera)
{
	float lineWidth;

	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	glLineWidth(1.0f);

	uint progId = App->shader->GetProgramId(COLOR, "UberShader");
	glUseProgram(progId);

	glBindBuffer(GL_ARRAY_BUFFER, camera.GetFrustumBufferId());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(camVertex), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(camVertex), (GLvoid*)(3 * sizeof(GLfloat)));

	GLint modelView = glGetUniformLocation(progId, "model_view");
	glUniformMatrix4fv(modelView, 1, GL_FALSE, App->camera->camera->GetViewMatrix().ptr());

	GLint proyection = glGetUniformLocation(progId, "projection");
	glUniformMatrix4fv(proyection, 1, GL_FALSE, App->camera->camera->GetProyectionMatrix().ptr());

	GLint transformUniformId = glGetUniformLocation(progId, "transform");
	float4x4 transformMatrix = camera.gameObject->GetTransform()->GetModelMatrix();
	transformMatrix.RemoveScale();
	glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, transformMatrix.Transposed().ptr());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, camera.GetFrustumIndicesId());
	glDrawElements(GL_LINES, camera.GetNumberOfPoints(), GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glUseProgram(0);

	glLineWidth(lineWidth);
}

void ModuleRender::DrawCube(const float3& offset, const float3& color, float radius) const
{
	// IMPROVE

	float lineWidth;

	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	float3 positions[8] = 
	{
		float3(-1.0f, -1.0f, -1.0f),
		float3(1.0f, -1.0f, -1.0f),
		float3(1.0f, 1.0f, -1.0f),
		float3(-1.0f, 1.0f, -1.0f),

		float3(-1.0f, -1.0f, 1.0f),
		float3(1.0f, -1.0f, 1.0f),
		float3(1.0f, 1.0f, 1.0f),
		float3(-1.0f, 1.0f, 1.0f)
	};

	for(size_t i = 0; i < 8; ++i)
	{
		positions[i] *= radius;
	}

	glPushMatrix();

	glLineWidth(3.0f);

	glColor3f(color.x, color.y, color.z);

	glTranslatef(offset.x, offset.y, offset.z);

	glBegin(GL_LINE_STRIP);

	glVertex3f(positions[0].x, positions[0].y, positions[0].z);
	glVertex3f(positions[1].x, positions[1].y, positions[1].z);
	glVertex3f(positions[2].x, positions[2].y, positions[2].z);
	glVertex3f(positions[3].x, positions[3].y, positions[3].z);
	glVertex3f(positions[0].x, positions[0].y, positions[0].z);

	glEnd();

	glBegin(GL_LINE_STRIP);

	glVertex3f(positions[4].x, positions[4].y, positions[4].z);
	glVertex3f(positions[5].x, positions[5].y, positions[5].z);
	glVertex3f(positions[6].x, positions[6].y, positions[6].z);
	glVertex3f(positions[7].x, positions[7].y, positions[7].z);
	glVertex3f(positions[4].x, positions[4].y, positions[4].z);

	glEnd();

	glBegin(GL_LINES);

	glVertex3f(positions[0].x, positions[0].y, positions[0].z);
	glVertex3f(positions[4].x, positions[4].y, positions[4].z);

	glVertex3f(positions[1].x, positions[1].y, positions[1].z);
	glVertex3f(positions[5].x, positions[5].y, positions[5].z);

	glVertex3f(positions[3].x, positions[3].y, positions[3].z);
	glVertex3f(positions[7].x, positions[7].y, positions[7].z);

	glVertex3f(positions[2].x, positions[2].y, positions[2].z);
	glVertex3f(positions[6].x, positions[6].y, positions[6].z);

	glEnd();

	glEnd();

	glLineWidth(lineWidth);

	glPopMatrix();
}

void ModuleRender::DrawRectangularBox(const float3& position, const float3& rotation, const float3& scale, const float3& color, float w, float h, float d) const
{
	// IMPROVE
	if(App->state == TimeState::STOPED)
	{
		float lineWidth;

		glGetFloatv(GL_LINE_WIDTH, &lineWidth);

		float3 positions[8] =
		{
			float3(-w, -h, -d),
			float3(w, -h, -d),
			float3(w, h, -d),
			float3(-w, h, -d),

			float3(-w, -h, d),
			float3(w, -h, d),
			float3(w, h, d),
			float3(-w, h, d)
		};

		glPushMatrix();

		glLineWidth(3.0f);

		glColor3f(color.x, color.y, color.z);

		glTranslatef(position.x, position.y, position.z);

		glRotatef(RadToDeg(rotation.x), 1.0f, 0.0f, 0.0f);
		glRotatef(RadToDeg(rotation.y), 0.0f, 1.0f, 0.0f);
		glRotatef(RadToDeg(rotation.z), 0.0f, 0.0f, 1.0f);

		glScalef(scale.x, scale.y, scale.z);

		glBegin(GL_LINE_STRIP);

		glVertex3f(positions[0].x, positions[0].y, positions[0].z);
		glVertex3f(positions[1].x, positions[1].y, positions[1].z);
		glVertex3f(positions[2].x, positions[2].y, positions[2].z);
		glVertex3f(positions[3].x, positions[3].y, positions[3].z);
		glVertex3f(positions[0].x, positions[0].y, positions[0].z);

		glEnd();

		glBegin(GL_LINE_STRIP);

		glVertex3f(positions[4].x, positions[4].y, positions[4].z);
		glVertex3f(positions[5].x, positions[5].y, positions[5].z);
		glVertex3f(positions[6].x, positions[6].y, positions[6].z);
		glVertex3f(positions[7].x, positions[7].y, positions[7].z);
		glVertex3f(positions[4].x, positions[4].y, positions[4].z);

		glEnd();

		glBegin(GL_LINES);

		glVertex3f(positions[0].x, positions[0].y, positions[0].z);
		glVertex3f(positions[4].x, positions[4].y, positions[4].z);

		glVertex3f(positions[1].x, positions[1].y, positions[1].z);
		glVertex3f(positions[5].x, positions[5].y, positions[5].z);

		glVertex3f(positions[3].x, positions[3].y, positions[3].z);
		glVertex3f(positions[7].x, positions[7].y, positions[7].z);

		glVertex3f(positions[2].x, positions[2].y, positions[2].z);
		glVertex3f(positions[6].x, positions[6].y, positions[6].z);

		glEnd();

		glEnd();

		glLineWidth(lineWidth);

		glPopMatrix();
	}
}

void ModuleRender::DrawSphere(const float3& offset, const float3& color, float radius) const
{
	// IMPROVE
	if(App->state == TimeState::STOPED)
	{
		const Mesh* sphere = App->entity->GetSphere();

		float pointSize;

		glGetFloatv(GL_POINT_SIZE, &pointSize);

		glPushMatrix();

		glPointSize(3.0f);

		glColor3f(color.x, color.y, color.z);

		glTranslatef(offset.x, offset.y, offset.z);

		glBegin(GL_POINTS);

		for(Vertex vertex : sphere->GetVertices())
		{
			float3 position = radius * vertex.position;
			glVertex3f(position.x, position.y, position.z);
		}

		glEnd();

		glPointSize(pointSize);

		glPopMatrix();
	}
}

void ModuleRender::SetUpLight() const
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	glEnable(GL_LIGHTING);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void ModuleRender::DrawGeometry()
{
	for (const DrawInfo& drawInfo : drawBuffer) 
	{
		Draw(drawInfo);
	}
}

void ModuleRender::Draw(const DrawInfo & drawInfo)
{
	GLuint progId = drawInfo.material.GetProgramId();
	GLuint textId = drawInfo.material.GetTexture()->GetId();

	glUseProgram(progId);
	if (textId != 0)
	{
		GLint texture = glGetUniformLocation(progId, "ourTexture");
		glUniform1i(texture, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textId);
	}

	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetVertexBufferId());

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	//color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	//uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(10 * sizeof(GLfloat)));
	//normal
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));

	// ...

	if(!drawInfo.mesh.GetBones().empty())
	{
		//indices
		

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetBoneIndicesBufferId());
		glVertexAttribIPointer(4, 4, GL_INT, 0, (void*)0);
		//weights
		

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetBoneWeightsBufferId());
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		float4x4* palette = App->anim->GetPalette(&drawInfo.gameObject, &drawInfo.mesh);

		//Palete
		GLint paleteId = glGetUniformLocation(progId, "palette");
		glUniformMatrix4fv(paleteId, MAX_BONES, GL_FALSE, palette->ptr());
	}

	// ...

	GLint modelView = glGetUniformLocation(progId, "model_view");
	glUniformMatrix4fv(modelView, 1, GL_FALSE, App->camera->GetPlayOrEditorCamera()->GetViewMatrix().ptr());

	GLint proyection = glGetUniformLocation(progId, "projection");
	glUniformMatrix4fv(proyection, 1, GL_FALSE, App->camera->GetPlayOrEditorCamera()->GetProyectionMatrix().ptr());

	GLint transformUniformId = glGetUniformLocation(progId, "transform");
	glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, drawInfo.transform.GetModelMatrix().Transposed().ptr());

	GLint rotation = glGetUniformLocation(progId, "rotation");
	if (rotation != -1)
	{
		glUniformMatrix4fv(rotation, 1, GL_FALSE, drawInfo.transform.GetWorldRotation().ToFloat4x4().Transposed().ptr());
	}

	GLint lightSourcePosition = glGetUniformLocation(progId, "lightSourcePosition");
	if (lightSourcePosition != -1)
	{
		glUniform3f(lightSourcePosition, -100, 0, 0);
	}

	GLint light = glGetUniformLocation(progId, "lightDir");
	if (light != -1)
	{
		glUniform3f(light, LIGHT_DIR.x, LIGHT_DIR.y, LIGHT_DIR.z);
	}

	GLint camera = glGetUniformLocation(progId, "actualCameraModelView");
	if(camera != -1 && App->camera->GetEnabledCamera() != nullptr)
	{
		glUniformMatrix4fv(camera, 1, GL_FALSE, App->camera->GetEnabledCamera()->GetViewMatrix().ptr());
	}

	GLint cameraPosition = glGetUniformLocation(progId, "cameraPosition");
	if (cameraPosition != -1)
	{
		float3 cPos = App->camera->GetPlayOrEditorCamera()->gameObject->GetTransform()->GetWorldPosition();
		glUniform3f(cameraPosition, cPos.x, cPos.y, cPos.z);
	}

	GLint cameraFar = glGetUniformLocation(progId, "actualCameraFar");
	if(cameraFar != -1 && App->camera->GetEnabledCamera() != nullptr)
	{
		glUniform1f(cameraFar,App->camera->GetEnabledCamera()->GetFarPlane());
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.mesh.GetIndicesBufferId());
	glDrawElements(drawInfo.mesh.GetDrawMode(), drawInfo.mesh.GetIndicesNumber(), GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	if(!drawInfo.mesh.GetBones().empty())
	{
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);
	}

	glUseProgram(0);
}

void ModuleRender::DrawLastRay()
{
	LineSegment ray = App->camera->GetLastRay();

	float lineWidth;

	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	glLineWidth(200);
	glBegin(GL_LINES);
	glVertex3f(ray.a.x,ray.a.y,ray.a.z);
	glVertex3f(ray.b.x,ray.b.y,ray.b.z);
	glEnd();

	glLineWidth(lineWidth);
}
