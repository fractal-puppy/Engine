#include "EnemyMovementScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "ComponentTransform.h"
#include "GameObject.h"
#include "PlayerMovement.h"

#include "JSON.h"
#include <assert.h>
#include "imgui.h"
#include <iostream>
#include <stack>
#include "Globals.h"
#include "Math/MathFunc.h"

EnemyMovementScript_API Script* CreateScript()
{
	EnemyMovementScript* instance = new EnemyMovementScript;
	return instance;
}

void EnemyMovementScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);
	ImGui::InputFloat("max player distance", &maxPlayerDistance);
	
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
	playerScript = player->GetComponent<PlayerMovement>();
	assert(playerScript != nullptr);

	myBbox = &App->scene->FindGameObjectByName("ZombMesh", gameobject)->bbox;
	targetBbox = &App->scene->FindGameObjectByName("PlayerMesh", player)->bbox;
}

void EnemyMovementScript::Update()
{
	if (player != nullptr && !stop)
	{
		float3 enemyCurrentPosition = gameobject->transform->GetGlobalPosition();
		float3 playerCurrentPosition = player->transform->GetGlobalPosition();

		float distance = enemyCurrentPosition.DistanceSq(playerCurrentPosition);

		if (enemyCurrentPosition.DistanceSq(playerCurrentPosition) < maxPlayerDistance)
		{
			float3 dir = (enemyCurrentPosition - playerCurrentPosition);

			Quat currentRotation = gameobject->transform->GetRotation();

			Quat rotation = currentRotation.LookAt(gameobject->transform->front.Normalized(), dir.Normalized(), float3::unitY, float3::unitY);

			gameobject->transform->SetRotation(rotation.Mul(currentRotation));

			float3 movement = gameobject->transform->front.Normalized() * -speed * App->time->gameDeltaTime;
			gameobject->transform->SetPosition(gameobject->transform->GetPosition() + movement);

			if (myBbox->Intersects(*targetBbox))
			{
				playerScript->isPlayerDead = true;
				stop = true;
			}
		}
	}
}

void EnemyMovementScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
	json->AddFloat("maxPlayerDistance", maxPlayerDistance);
	json->AddString("playerName", playerName.c_str());
}

void EnemyMovementScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
	maxPlayerDistance = json->GetFloat("maxPlayerDistance");
	playerName = json->GetString("playerName");
}

GameObject* EnemyMovementScript::GetGameObjectRecursiveByName(GameObject* gameobject, const char* name)
{
	if (strcmp(gameobject->name.c_str(), name) == 0)
	{
		return gameobject;
	}

	std::stack<GameObject*> stack;
	for (GameObject* childre : gameobject->children)
	{
		stack.push(childre);
	}

	while (!stack.empty())
	{
		GameObject* gameobject = stack.top();
		stack.pop();

		if (strcmp(gameobject->name.c_str(), name) == 0)
		{
			return gameobject;
		}

		for (GameObject* childre : gameobject->children)
		{
			stack.push(childre);
		}
	}
}