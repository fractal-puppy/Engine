#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "Geometry/Frustum.h"
#include "pcg_random.hpp"

class GameObject;
class ComponentCamera;
class ComponentLight;
struct Texture;
struct par_shapes_mesh_s;

class ModuleScene :
	public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	bool Start() override;

	update_status Update() override;
	void Draw(const math::Frustum &frustum);
	void DrawHierarchy();

	GameObject * CreateGameObject(const float4x4& transform, const char* filepath, const char* name, GameObject* parent);
	GameObject * CreateGameObject(const char * name, GameObject* parent);

	void CreateSphere(const char * name, const float3 & pos, const Quat & rot, float size, unsigned int slices, unsigned int stacks, const float4 & color);

	unsigned SaveParShapesMesh(const par_shapes_mesh_s & mesh, char** data);

	void SaveScene(const GameObject &rootGO, const char* name);

	void LoadScene(const char * scene);

	unsigned GetNewUID();

public:
	GameObject* root = nullptr;
	GameObject* selected = nullptr;
	ComponentCamera* maincamera = nullptr;
	Texture* camera_notfound_texture = nullptr;
	pcg32 uuid_rng;
	std::string name;

	float ambient = 0.5f;
	ComponentLight* light = nullptr;
};

#endif __ModuleScene_h__