#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleSceneLoader.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include <assert.h>
#include "GL/glew.h"
#include "Imgui/imgui.h"
#include "JSON.h"

ComponentMesh::ComponentMesh(GameObject* gameobject, char * mesh) : Component(gameobject, ComponentType::Mesh)
{
	if (mesh != nullptr)
	{
		SetMesh(mesh);
	}
}

ComponentMesh::ComponentMesh(const ComponentMesh & component) : Component(component) //TODO: SaveMesh (for sphere, cube, etc)
{
	//TODO: gen new buffers
	VAO = component.VAO;
	VBO = component.VBO;
	EBO = component.EBO;
	materialIndex = component.materialIndex;
	numIndices = component.numIndices;
	vertices.reserve(component.vertices.capacity());
	vertices = component.vertices;
	boundingBox = component.boundingBox;
	meshUID = component.meshUID;
}

ComponentMesh::~ComponentMesh()
{
	DeleteBuffers(); //TODO: clean up components or simply call destructor?
}

Component * ComponentMesh::Clone()
{
	return new ComponentMesh(*this);
}

void ComponentMesh::Draw(unsigned int shaderProgram) const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

	// Desactivem VAO
	glBindVertexArray(0);
	// Desactivem Textura
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ComponentMesh::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}
		ImGui::Text("Num vertices : %d", vertices.size());
		ImGui::Text("Num triangles : %d", numIndices / 3);
		ImGui::Separator();
	}
	ImGui::PopID();
}

void ComponentMesh::SetMesh(char * &mesh) //TODO: pass by reference or know size of mesh
{
	assert(mesh != nullptr);
	DeleteBuffers();
	char *data = mesh;

	unsigned int numIndices = *(int*)mesh;
	mesh += sizeof(int);

	unsigned int numVertices = *(int*)mesh;
	mesh += sizeof(int);

	float* vertices = (float*)mesh;
	mesh += sizeof(float) * 3 * numVertices;
	
	float* normals = (float*)mesh;
	mesh += sizeof(float) * 3 * numVertices;

	float* texCoords = (float*)mesh;
	mesh += sizeof(float) * 2 * numVertices;

	int* indices = (int*)mesh;
	mesh += sizeof(int) * numIndices;
	// VAO Creation
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Buffer Creation with vertex data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*numVertices * 8, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * numVertices, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, sizeof(GLfloat) * 3 * numVertices, normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * numVertices, sizeof(GLfloat) * 2 * numVertices, texCoords);
	//float * pbuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, sizeof(GLfloat) * 2 * numVertices, GL_MAP_WRITE_BIT);
	//memcpy(pbuffer, texCoords, sizeof(float) * 2 * numVertices);
	//glUnmapBuffer(GL_ARRAY_BUFFER);

	//Buffer creation with indices
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*numIndices, indices, GL_STATIC_DRAW);
	//int * pbufferIndex = (int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int)*numIndices, GL_MAP_WRITE_BIT);
	//memcpy(pbufferIndex, indices, sizeof(int) * numIndices);
	//glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 1
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)(sizeof(float) * 3 * numVertices)       // array buffer offset
	);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                  // attribute 2
		2,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)(sizeof(float) * 6 * numVertices)       // array buffer offset
	);

	// Disable VAO
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	

	// Disable VBO and EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	this->numIndices = numIndices;
	this->vertices.reserve(numVertices);
	for (size_t i = 0; i < numVertices; i++)
	{
		this->vertices.emplace_back(vertices);
		vertices += 3;
	}
	//materialIndex = mesh->mMaterialIndex; TODO import/load materials
	ComputeBBox();
	delete[] data;
}

unsigned int ComponentMesh::GetMaterialIndex()
{
	return materialIndex;
}

void ComponentMesh::ComputeBBox()
{

	float3 min, max;
	min = max = vertices[0];

	for (auto &vertice : vertices)
	{
		min.x = MIN(min.x, vertice.x);
		min.y = MIN(min.y, vertice.y);
		min.z = MIN(min.z, vertice.z);

		max.x = MAX(max.x, vertice.x);
		max.y = MAX(max.y, vertice.y);
		max.z = MAX(max.z, vertice.z);

	}
	boundingBox.minPoint = min;
	boundingBox.maxPoint = max;
}

AABB ComponentMesh::GetBoundingBox() const
{
	return boundingBox;
}

void ComponentMesh::DeleteBuffers()
{
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
	}
	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
	}
	if (EBO != 0)
	{
		glDeleteBuffers(1, &EBO);
	}
}

void ComponentMesh::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddUint("UID", meshUID);
}

void ComponentMesh::Load(JSON_value * value)
{
	Component::Load(value);
	meshUID = value->GetUint("UID");
	char *data;
	App->fsystem->Load((MESHES + std::to_string(meshUID) + MESHEXTENSION).c_str(), &data); //TODO: use mini resource maanger to optimize this
	SetMesh(data);
}
