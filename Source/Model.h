#ifndef __Model_h__
#define __Model_h__

#include "Mesh.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <vector>
#include <list>
#include "GL/glew.h"
#include <string>

struct Texture
{
	int id;
	int width;
	int height;
	Texture(int id, int width, int height) : id(id), width(width), height(height)
	{}
};

class Model
{
public:
	Model(const char *file);
	~Model();

	void Draw() const;
	void DrawProperties();
	void UpdateTexture(Texture texture);
	void ProcessNode(const aiNode * node, const aiScene * scene, const aiMatrix4x4 &parentTransform);


private:
	void LoadModel(const char *file);
	void GenerateMaterialData(aiMaterial * material);
	void GetBoundingBox();

public:
	AABB BoundingBox;

private:
	std::list<Mesh> meshes;
	std::vector<Texture> textures;
	const char* path;
};

#endif //__Model_h__