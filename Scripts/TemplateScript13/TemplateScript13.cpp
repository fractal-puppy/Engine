#include "TemplateScript13.h"
#include "Application.h"
#include "ModuleScene.h"

TemplateScript13_API Script* CreateScript()
{
	TemplateScript13* instance = new TemplateScript13;
	return instance;
}

void TemplateScript13::Start()
{
	math::float3 position = math::float3::zero;
	math::Quat rotation = math::Quat::identity;
	App->scene->Spawn("DasBox", position, rotation);

	math::float3 position2(400.f, 400.f, 400.f);
	math::Quat rotation2 = math::Quat::identity;
	App->scene->Spawn("DasBox", position2, rotation2);
}