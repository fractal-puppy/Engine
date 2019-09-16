#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentBoxTrigger.h"
#include "ComponentTransform.h"

#include "BossProjectileScript.h"
#include "PlayerMovement/PlayerMovement.h"

#include "imgui.h"
#include "JSON.h"

BossProjectileScript_API Script* CreateScript()
{
	BossProjectileScript* instance = new BossProjectileScript;
	return instance;
}

void BossProjectileScript::Awake()
{
	playerGO = App->scene->FindGameObjectByTag("Player", gameobject->parent);
	if (playerGO == nullptr)
	{
		LOG("Player in bossProjectile not found");
	}
	else
	{
		playerScript = playerGO->GetComponent<PlayerMovement>();
		if (playerScript == nullptr)
		{
			LOG("Playerscript in bossProjectile not found");
		}
	}

	render = (ComponentRenderer*)gameobject->GetComponentInChildren(ComponentType::Renderer);
	if (render == nullptr)
	{
		LOG("Render in bossProjectile not found")
	}
	trigger = (ComponentBoxTrigger*)gameobject->GetComponentInChildren(ComponentType::BoxTrigger);
	if (trigger == nullptr)
	{
		LOG("BoxTrigger in bossProjectile not found");
	}
}

void BossProjectileScript::Start()
{
	trigger->Enable(false);
}

void BossProjectileScript::Update()
{
	math::float3 playerPosition = playerGO->transform->GetPosition();

	
	//Having a switch is faster but this also works :p
	if (!fadeInComplete)
	{
		render->dissolveAmount -= App->time->gameDeltaTime;
		if (render->dissolveAmount <= 0.0f)
		{
			fadeInComplete = true;
		}
	}
	else if (fadeInComplete && !waitingComplete)
	{
		gameobject->transform->LookAt(playerPosition + math::float3(0.0f, 90.0f, 0.0f));
		waitTimer += App->time->gameDeltaTime;
		if (waitTimer > waitingTime)
		{
			shootingDirection = (playerPosition + math::float3(0.0f, 90.0f, 0.0f)) - gameobject->transform->GetPosition();
			shootingDirection.Normalize();

			waitingComplete = true;
			trigger->Enable(true);
		}
	}
	else
	{
		//Go to the player
		shotTimer += App->time->gameDeltaTime;

		math::float3 currentPosition = gameobject->transform->GetPosition();
		gameobject->transform->SetPosition(currentPosition + shootingDirection * projectileSpeed * App->time->gameDeltaTime);
		if (shotTimer > projectileLife || hasDamaged)
		{
			gameobject->deleteFlag = true;
		}
		
	}
}

void BossProjectileScript::OnTriggerEnter(GameObject * go)
{
	if (go == playerGO && !hasDamaged)
	{
		playerScript->Damage(damageAmount);
		hasDamaged = true;
	}
}

void BossProjectileScript::Expose(ImGuiContext * context)
{
	ImGui::DragFloat("Waiting time", &waitingTime, 0.1f, 0.1f, 10.0f);
	ImGui::DragFloat("Projectile speed", &projectileSpeed, 10.0f, 10.0f, 5000.0f);
	ImGui::DragFloat("Projectile life", &projectileLife, 0.1f, 0.1f, 10.0f);
	ImGui::InputFloat("Damage", &damageAmount);
}

void BossProjectileScript::Serialize(JSON_value * json) const
{
	json->AddFloat("waitingTime", waitingTime);
	json->AddFloat("projectileSpeed", projectileSpeed);
	json->AddFloat("projectileLife", projectileLife);
	json->AddFloat("damageAmount", damageAmount);
}

void BossProjectileScript::DeSerialize(JSON_value * json)
{
	waitingTime = json->GetFloat("waitingTime",0.0f);
	projectileSpeed = json->GetFloat("projectileSpeed", 0.0f);
	projectileLife = json->GetFloat("projectileLife", 0.0f);
	damageAmount = json->GetFloat("damageAmount", 0.0f);
}
