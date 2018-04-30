#include "ModuleFX.h"

#include <algorithm>

#include "Application.h"
#include "ModuleCamera.h"
#include "Camera.h"

ModuleFX::ModuleFX()
{}

ModuleFX::~ModuleFX()
{}

update_status ModuleFX::Update()
{
	if(App->camera->GetPlayOrEditorCamera() != nullptr)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1f);
		for(const DrawInfo& drawInfo : effectsToDraw)
		{
			Draw(drawInfo);
		}
		glDisable(GL_ALPHA_TEST);
	}
	effectsToDraw.clear();

	return update_status::UPDATE_CONTINUE;
}

void ModuleFX::Draw(const DrawInfo & drawInfo)
{
	GLuint progId = drawInfo.material.GetProgramId();
	GLuint textId = drawInfo.material.GetTexture()->GetId();

	glUseProgram(progId);
	if(textId != 0)
	{
		GLint texture = glGetUniformLocation(progId, "ourTexture");
		glUniform1i(texture, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textId);
	}

	//position
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.buffer.vertexPosId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//color
	/*glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));*/
	//uv
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.buffer.vertexUvId);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


	GLint modelView = glGetUniformLocation(progId, "model_view");
	glUniformMatrix4fv(modelView, 1, GL_FALSE, App->camera->GetPlayOrEditorCamera()->GetViewMatrix().ptr());

	GLint proyection = glGetUniformLocation(progId, "projection");
	glUniformMatrix4fv(proyection, 1, GL_FALSE, App->camera->GetPlayOrEditorCamera()->GetProyectionMatrix().ptr());

	GLint transformUniformId = glGetUniformLocation(progId, "transform");
	glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, drawInfo.transform.GetModelMatrix().Transposed().ptr());

	GLint camera = glGetUniformLocation(progId, "actualCameraModelView");
	if(camera != -1 && App->camera->GetEnabledCamera() != nullptr)
	{
		glUniformMatrix4fv(camera, 1, GL_FALSE, App->camera->GetEnabledCamera()->GetViewMatrix().ptr());
	}

	GLint cameraFar = glGetUniformLocation(progId, "actualCameraFar");
	if(cameraFar != -1 && App->camera->GetEnabledCamera() != nullptr)
	{
		glUniform1f(cameraFar, App->camera->GetEnabledCamera()->GetFarPlane());
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.buffer.indicesId);
	glDrawElements(GL_TRIANGLES, drawInfo.buffer.nIndices, GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);

	glUseProgram(0);
}

void ModuleFX::AddToDraw(Material & material, Transform & trans, BufferInfo & info)
{
	DrawInfo temp = {material,trans,info};
	effectsToDraw.push_back(temp);
}
