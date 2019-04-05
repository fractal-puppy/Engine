#ifndef __ComponentParticles_h__
#define __ComponentParticles_h__

#include "Component.h"
#include <string>
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

#include <list>
#include "ComponentTransform.h"

struct Texture;

struct Particle
{
	float speed = rand() % 100;

	math::float3 position = math::float3::zero;
	math::float4x4 global = math::float4x4::zero;
	math::float3 direction = math::float3::unitY;
};


class ComponentParticles :
	public Component
{
	friend class ModuleParticles;

public:
	


	ComponentParticles(GameObject* gameobject);
	ComponentParticles(const ComponentParticles& component);
	~ComponentParticles();


	Component* Clone() const override;
	void DrawProperties() override;
	bool CleanUp() override;

	void Update(float dt, const math::float3& camPos);

	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	Texture* texture = nullptr;

	std::list<Particle> particles;

private:

	std::string textureName = "None Selected";
	std::vector<std::string> textureFiles;

	int xTiles = 1;
	int yTiles = 1;
	float fps = 24.f;

	float timer = 0.f;
	float frameMix = 0.f;
	int f1Xpos;
	int f1Ypos;
	int f2Xpos;
	int f2Ypos;
};


#endif __ComponentParticles_h__