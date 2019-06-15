#include "SpawnTestScript.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "imgui.h"

SpawnTestScript_API Script* CreateScript()
{
	SpawnTestScript* instance = new SpawnTestScript;
	return instance;
}

void SpawnTestScript::Start()
{
	GameObject* instance = App->scene->Spawn(prefab.c_str(), math::float3(100.0f,100.0f,100.f), Quat::identity,gameobject);
	LOG("instance of: %s", instance->name);
}

void SpawnTestScript::Expose(ImGuiContext * context)
{
	ImGui::SetCurrentContext(context);
	char* buffer = new char[64];
	strcpy_s(buffer, 64, prefab.c_str());
	ImGui::InputText("Prefab", buffer, 64);
	prefab = buffer;
	delete buffer;
}
