#include "ProjectileScript.h"

#include "Application.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "imgui.h"
#include "JSON.h"

ProjectileScript_API Script* CreateScript()
{
	ProjectileScript* instance = new ProjectileScript;
	return instance;
}

void ProjectileScript::Start()
{
	transform = gameobject->transform;
	if (transform == nullptr)
	{
		LOG("The transform of the GameObject %s couldn't be found. \n", gameobject->name);
	}
}

void ProjectileScript::Update()
{
	// Move forward
	math::float3 movement = transform->front.Normalized() * -speed * App->time->gameDeltaTime;
	transform->SetPosition(gameobject->transform->GetPosition() + movement);
}

void ProjectileScript::Expose(ImGuiContext* context)
{
	ImGui::InputFloat("Projectile speed", &speed);
	ImGui::InputFloat("Life time", &lifeTime);
}

void ProjectileScript::Serialize(JSON_value* json) const
{
	json->AddFloat("speed", speed);
	json->AddFloat("lifeTime", lifeTime);
}

void ProjectileScript::DeSerialize(JSON_value* json)
{
	speed = json->GetFloat("speed");
	lifeTime = json->GetFloat("lifeTime");
}
