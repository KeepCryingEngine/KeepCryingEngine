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
#include "ModuleLight.h"

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

bool ModuleRender::Start()
{
	////Set up shadowFrame
	glGenFramebuffers(1,&shadowFrameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER,shadowFrameBufferId);
	
	glGenTextures(1,&shadowTextureId);
	glBindTexture(GL_TEXTURE_2D, shadowTextureId);

	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,1024,1024,0,GL_DEPTH_COMPONENT,GL_FLOAT, NULL);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTextureId,0);
	glDrawBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glBindTexture(GL_TEXTURE_2D,0);

	//Set up uniform block
	glGenBuffers(1, &uniformCameraBufferId);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformCameraBufferId);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float4x4) * 2, 0, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, App->shader->LinkUniformBlock("camera"), uniformCameraBufferId);

	return true;
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

		//Update uniform Block for shadow
		glBindBuffer(GL_UNIFORM_BUFFER, uniformCameraBufferId);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), App->light->GetProyectionMatrix().ptr());
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), App->light->GetViewMatrix().ptr());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		DrawShadowTexture();

		//Update uniform Block for second draw
		glBindBuffer(GL_UNIFORM_BUFFER, uniformCameraBufferId);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), App->camera->GetPlayOrEditorCamera()->GetProyectionMatrix().ptr());
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), App->camera->GetPlayOrEditorCamera()->GetViewMatrix().ptr());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//Prepare for real draw(we do this here, because if we do on draw geometry, the grid will disappear)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, App->configuration.screenWidth, App->configuration.screenHeight);
		//glCullFace(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void ModuleRender::DrawCross(const float3& pos, const float3& color, float scale)const
{
	float crossSize = 0.01f * scale;
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
	glLineWidth(3);

	GLfloat previousColor[4];
	glGetFloatv(GL_CURRENT_COLOR, previousColor);

	glBegin(GL_LINES);

	glColor3f(color.x, color.y, color.z);

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

	GLint modelView = glGetUniformLocation(progId, "view");
	glUniformMatrix4fv(modelView, 1, GL_FALSE, App->camera->camera->GetViewMatrix().ptr());

	GLint proyection = glGetUniformLocation(progId, "projection");
	glUniformMatrix4fv(proyection, 1, GL_FALSE, App->camera->camera->GetProyectionMatrix().ptr());

	GLint transformUniformId = glGetUniformLocation(progId, "model");
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

void ModuleRender::DrawLightFrustum()
{
	float lineWidth;

	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	glLineWidth(1.0f);

	uint progId = App->shader->GetProgramId(COLOR, "UberShader");
	glUseProgram(progId);

	glBindBuffer(GL_ARRAY_BUFFER, App->light->GetFrustumBufferId());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(lightVertex), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(lightVertex), (GLvoid*)(3 * sizeof(GLfloat)));

	GLint modelView = glGetUniformLocation(progId, "view");
	glUniformMatrix4fv(modelView, 1, GL_FALSE, App->camera->camera->GetViewMatrix().ptr());

	GLint proyection = glGetUniformLocation(progId, "projection");
	glUniformMatrix4fv(proyection, 1, GL_FALSE, App->camera->camera->GetProyectionMatrix().ptr());

	GLint transformUniformId = glGetUniformLocation(progId, "model");
	float3 rot = App->light->GetDirection().Normalized();
	float4x4 transformMatrix = float4x4::FromTRS(App->light->GetPosition(), Quat::FromEulerXYZ(rot.x,rot.y,rot.z), float3::one);;
	transformMatrix.RemoveScale();
	glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, transformMatrix.Transposed().ptr());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->light->GetFrustumIndicesId());
	glDrawElements(GL_LINES, App->light->GetNumberOfPoints(), GL_UNSIGNED_SHORT, nullptr);

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

	if(shadowTextureId != 0)
	{
		GLint shadowTexture = glGetUniformLocation(progId, "shadowMap");
		glUniform1i(shadowTexture, 2);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowTextureId);
	}

	if(drawInfo.material.GetTextureNormalMap() != nullptr)
	{
		GLuint normalMap = drawInfo.material.GetTextureNormalMap()->GetId();

		GLint normalMapShader = glGetUniformLocation(progId, "normalMap");
		if(normalMapShader != -1)
		{
			glUniform1i(normalMapShader, 1);
		}


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
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

	if(drawInfo.mesh.GetTangentBufferId() != 0)
	{
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetTangentBufferId());
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	if(!drawInfo.mesh.GetBones().empty())
	{
		//indices
		

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetBoneIndicesBufferId());
		glVertexAttribIPointer(5, 4, GL_INT, 0, (void*)0);
		//weights
		

		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetBoneWeightsBufferId());
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		float4x4* palette = App->anim->GetPalette(&drawInfo.gameObject, &drawInfo.mesh);

		//Palete
		GLint paleteId = glGetUniformLocation(progId, "palette");
		glUniformMatrix4fv(paleteId, MAX_BONES, GL_FALSE, palette->ptr());
	}

	GLint transformUniformId = glGetUniformLocation(progId, "model");
	glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, drawInfo.transform.GetModelMatrix().Transposed().ptr());

	GLint lightProjId = glGetUniformLocation(progId, "lightProj");
	glUniformMatrix4fv(lightProjId, 1, GL_FALSE, App->light->GetProyectionMatrix().ptr());

	GLint lightViewId = glGetUniformLocation(progId, "lightView");
	glUniformMatrix4fv(lightViewId, 1, GL_FALSE, App->light->GetViewMatrix().ptr());

	GLint shadowUniformId = glGetUniformLocation(progId, "shadow");
	glUniform1i(shadowUniformId, 0);

	GLint biasId = glGetUniformLocation(progId, "bias");
	glUniform1f(biasId, App->light->GetBias());

	GLint rotation = glGetUniformLocation(progId, "rotation");
	if (rotation != -1)
	{
		glUniformMatrix4fv(rotation, 1, GL_FALSE, drawInfo.transform.GetWorldRotation().ToFloat4x4().Transposed().ptr());
	}

	GLint lightSourcePosition = glGetUniformLocation(progId, "lightSourcePosition");
	if (lightSourcePosition != -1)
	{
		const float3& lightPosition = App->light->GetPosition();
		glUniform3f(lightSourcePosition, lightPosition.x, lightPosition.y, lightPosition.z);
	}

	GLint light = glGetUniformLocation(progId, "lightDir");
	if (light != -1)
	{
		const float3& lightDirection = App->light->GetDirection();
		glUniform3f(light, lightDirection.x, lightDirection.y, lightDirection.z);
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
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
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

void ModuleRender::DrawShadowTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBufferId);
	glViewport(0, 0, 1024, 1024);
	//glCullFace(GL_FRONT);
	glClear(GL_DEPTH_BUFFER_BIT);
	for(const DrawInfo& drawInfo : drawBuffer)
	{
		DrawShadowTexture(drawInfo);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRender::DrawShadowTexture(const DrawInfo & drawInfo)
{
	GLuint progId = drawInfo.material.GetProgramId();

	glUseProgram(progId);

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

	if(drawInfo.mesh.GetTangentBufferId() != 0)
	{
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetTangentBufferId());
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	if(!drawInfo.mesh.GetBones().empty())
	{
		//indices


		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetBoneIndicesBufferId());
		glVertexAttribIPointer(5, 4, GL_INT, 0, (void*)0);
		//weights


		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, drawInfo.mesh.GetBoneWeightsBufferId());
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		float4x4* palette = App->anim->GetPalette(&drawInfo.gameObject, &drawInfo.mesh);

		//Palete
		GLint paleteId = glGetUniformLocation(progId, "palette");
		glUniformMatrix4fv(paleteId, MAX_BONES, GL_FALSE, palette->ptr());
	}

	GLint transformUniformId = glGetUniformLocation(progId, "model");
	glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, drawInfo.transform.GetModelMatrix().Transposed().ptr());

	GLint shadowUniformId = glGetUniformLocation(progId, "shadow");
	glUniform1i(shadowUniformId, 1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.mesh.GetIndicesBufferId());
	glDrawElements(drawInfo.mesh.GetDrawMode(), drawInfo.mesh.GetIndicesNumber(), GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
