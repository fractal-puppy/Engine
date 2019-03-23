#include "EnemyMovementScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "ComponentTransform.h"
#include "GameObject.h"

#include "JSON.h"
#include <assert.h>
#include "imgui.h"
#include <iostream>
#include <stack>
#include "Globals.h"
#include "Math/MathFunc.h"

EnemyMovementScript_API EnemyMovementScript* CreateScript()
{
	EnemyMovementScript* instance = new EnemyMovementScript;
	return instance;
}

void EnemyMovementScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);

	char* goName = new char[64];
	strcpy_s(goName, strlen(playerName.c_str()) + 1, playerName.c_str());
	ImGui::InputText("playerName", goName, 64);
	playerName = goName;
	delete[] goName;
}

void EnemyMovementScript::Start()
{
	LOG("Started player movement script");
	player = GetGameObjectRecursiveByName(App->scene->root, playerName.c_str());
}

void EnemyMovementScript::Update()
{
	if (player != nullptr)
	{
		//enemy-player collision check
		if (gameObject->BboxIntersects(player))
		{
			LOG("gotcha madafaka!");
		}
		float3 enemyCurrentPosition = gameObject->transform->GetGlobalPosition();
		float3 playerCurrentPosition = player->transform->GetGlobalPosition();

		float3 dir = (enemyCurrentPosition - playerCurrentPosition);

		Quat currentRotation = gameObject->transform->GetRotation();

		Quat rotation = currentRotation.LookAt(gameObject->transform->front.Normalized(), dir.Normalized(), float3::unitY, float3::unitY);

		gameObject->transform->SetRotation(rotation.Mul(currentRotation));

		float3 movement = gameObject->transform->front.Normalized() * -speed * App->time->gameDeltaTime;
		gameObject->transform->SetPosition(gameObject->transform->GetPosition() + movement);
	}
}

void EnemyMovementScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
	json->AddString("playerName", playerName.c_str());
}

void EnemyMovementScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
	playerName = json->GetString("playerName");
}

GameObject* EnemyMovementScript::GetGameObjectRecursiveByName(GameObject* gameObject, const char* name)
{
	if (strcmp(gameObject->name.c_str(), name) == 0)
	{
		return gameObject;
	}

	std::stack<GameObject*> stack;
	for (GameObject* childre : gameObject->children)
	{
		stack.push(childre);
	}

	while (!stack.empty())
	{
		GameObject* gameObject = stack.top();
		stack.pop();

		if (strcmp(gameObject->name.c_str(), name) == 0)
		{
			return gameObject;
		}

		for (GameObject* childre : gameObject->children)
		{
			stack.push(childre);
		}
	}
}