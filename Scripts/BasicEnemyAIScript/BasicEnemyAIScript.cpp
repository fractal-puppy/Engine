#include "BasicEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ComponentRenderer.h"

#include "EnemyControllerScript.h"
#include "PlayerMovement.h"

#include "Geometry/AABB.h"
#include "Math/Quat.h"
#include "Math/float4x4.h"
#include "imgui.h"
#include <stack>
#include "JSON.h"

BasicEnemyAIScript_API Script* CreateScript()
{
	BasicEnemyAIScript* instance = new BasicEnemyAIScript;
	return instance;
}

void BasicEnemyAIScript::Start()
{
	startPosition = gameObject->transform->position;
	startPosition.y += yTranslation;

	enemyController = (EnemyControllerScript*)gameObject->FindScriptByName("EnemyControllerScript");
	playerScript = (PlayerMovement*)(App->scene->FindGameObjectByName("Player")->GetScript());

	//anim = (ComponentAnimation*)gameObject->GetComponent(ComponentType::Animation);
	//if (anim == nullptr) LOG("The GameObject %s has no Animation component attached \n", gameObject->name);
}

void BasicEnemyAIScript::Update()
{
	if (enemyController->player == nullptr)
		return;

	EnemyState previous = enemyController->enemyState;

	switch (enemyController->enemyState)
	{
	case EnemyState::WAIT:		Wait();					break;
	case EnemyState::STANDUP:	StandUp();				break;
	case EnemyState::CHASE:		Chase();				break;
	case EnemyState::RETURN:	ReturnToStartPosition();break;
	case EnemyState::LAYDOWN:	Laydown();				break;
	case EnemyState::ATTACK:	Attack();				break;
	case EnemyState::COOLDOWN:	Cooldown();				break;
	case EnemyState::DEAD:		Die();					break;
	default:
		break;
	}

	// Check animation to play
	//if(anim != nullptr)
	CheckStateChange(previous, enemyController->enemyState);
}

void BasicEnemyAIScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("Enemy:");

	ImGui::Text("Stand up:");
	ImGui::InputFloat("Distance to activate", &activationDistance);
	ImGui::InputFloat("Stand-up Speed", &standupSpeed);
	ImGui::Text("Chase:");
	ImGui::InputFloat("Y Translation", &yTranslation);
	ImGui::InputFloat("Chase Speed", &chaseSpeed);
	ImGui::Text("Return:");
	ImGui::InputFloat("Return Distance", &returnDistance);
	ImGui::InputFloat("Return Speed", &returnSpeed);
	ImGui::Text("Cooldown:");
	ImGui::InputFloat("Cooldown Time", &cooldownTime);
}

void BasicEnemyAIScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

	//Wait variables
	json->AddFloat("activationDistance", activationDistance);

	// Stand-Up variables
	json->AddFloat("standupSpeed", standupSpeed);
	json->AddFloat("yTranslation", yTranslation);

	// Chase variables
	json->AddFloat("chaseSpeed", chaseSpeed);

	// Return variables
	json->AddFloat("returnSpeed", returnSpeed);
	json->AddFloat("returnDistance", returnDistance);

	// Cooldown variables
	json->AddFloat("cooldownTime", cooldownTime);
}

void BasicEnemyAIScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);

	//Wait variables
	activationDistance = json->GetFloat("activationDistance");

	// Stand-Up variables
	standupSpeed = json->GetFloat("standupSpeed");
	yTranslation = json->GetFloat("yTranslation");

	// Chase variables
	chaseSpeed = json->GetFloat("chaseSpeed");

	// Return variables
	returnSpeed = json->GetFloat("returnSpeed");
	returnDistance = json->GetFloat("returnDistance");
	
	// Cooldown variables
	cooldownTime = json->GetFloat("cooldownTime");
}

void BasicEnemyAIScript::Wait()
{
	float3 enemyCurrentPosition = gameObject->transform->GetPosition();
	enemyCurrentPosition.y = 0.0f;
	float3 playerCurrentPosition = enemyController->player->transform->GetPosition();
	playerCurrentPosition.y = 0.0f;

	float distance = enemyCurrentPosition.Distance(playerCurrentPosition);

	if (distance < activationDistance)
	{
		enemyController->enemyState = EnemyState::STANDUP;
	}
}

void BasicEnemyAIScript::StandUp()
{
	// Translate on the Z axis
	float3 movement = gameObject->transform->up.Normalized() * standupSpeed * App->time->gameDeltaTime;
	gameObject->transform->SetLocalPosition(gameObject->transform->GetLocalPosition() + movement);
	auxTranslation += movement.y;

	// Check if the needed Z has been reached
	if (yTranslation <= auxTranslation)
	{
		enemyController->enemyState = EnemyState::CHASE;
		auxTranslation = 0.0f;
	}
}

void BasicEnemyAIScript::Chase()
{
	// Look at player and move towards
	gameObject->transform->LookAt(enemyController->player->transform->position);
	MoveTowards(chaseSpeed);

	// Check collision
	if (enemyController->myBbox != nullptr && enemyController->myBbox->Intersects(*enemyController->playerBbox))
	{
		// Player intersected, change to attack
		enemyController->enemyState = EnemyState::ATTACK;
	}
	else
	{
		// Check if player is too far
		float3 enemyCurrentPosition = gameObject->transform->GetPosition();
		enemyCurrentPosition.y = 0.0f;
		float3 playerCurrentPosition = enemyController->player->transform->GetPosition();
		playerCurrentPosition.y = 0.0f;

		float distance = enemyCurrentPosition.Distance(playerCurrentPosition);

		if (distance > returnDistance)
		{
			// Return to start position
			enemyController->enemyState = EnemyState::RETURN;
		}
	}
}

void BasicEnemyAIScript::ReturnToStartPosition()
{
	// Look at start position and move towards
	gameObject->transform->LookAt(startPosition);
	MoveTowards(returnSpeed);

	// Check distance to player
	math::float3 enemyCurrentPosition = gameObject->transform->GetPosition();
	enemyCurrentPosition.y = startPosition.y;
	float3 playerCurrentPosition = enemyController->player->transform->GetPosition();
	playerCurrentPosition.y = startPosition.y;

	float distance = enemyCurrentPosition.Distance(playerCurrentPosition);
	if (distance < activationDistance)
	{
		enemyController->enemyState = EnemyState::CHASE;
	}
	else if (startPosition.Distance(enemyCurrentPosition) < 1.5f)
	{
		enemyController->enemyState = EnemyState::LAYDOWN;
	}
}

void BasicEnemyAIScript::Laydown()
{
	// Translate on the Z axis
	float3 movement = gameObject->transform->up.Normalized() * standupSpeed * App->time->gameDeltaTime;
	gameObject->transform->SetLocalPosition(gameObject->transform->GetLocalPosition() - movement);
	auxTranslation += movement.y;

	// Check if the needed Z has been reached
	if (yTranslation <= auxTranslation)
	{
		enemyController->enemyState = EnemyState::WAIT;
		auxTranslation = 0.0f;
	}
}

void BasicEnemyAIScript::Attack()
{
	// Keep looking at player
	gameObject->transform->LookAt(enemyController->player->transform->position);

	if (enemyController->myBbox != nullptr && !enemyController->myBbox->Intersects(*enemyController->playerBbox))
	{
		enemyController->enemyState = EnemyState::CHASE;
	}
	else
	{
		// TODO: Add function to make damage to the player

		playerScript->Damage(damage);
		auxTimer = App->time->gameTime;
		enemyController->enemyState = EnemyState::COOLDOWN;
	}
}

void BasicEnemyAIScript::Cooldown()
{
	float waitedTime = (App->time->gameTime - auxTimer);

	if (waitedTime > cooldownTime)
	{
		enemyController->enemyState = EnemyState::ATTACK;
		auxTimer = 0.0f;
	}
}

void BasicEnemyAIScript::Die()
{
	gameObject->SetActive(false);
}

void BasicEnemyAIScript::MoveTowards(float speed) const
{
	math::float3 movement = gameObject->transform->front.Normalized() * -speed * App->time->gameDeltaTime;
	gameObject->transform->SetLocalPosition(gameObject->transform->GetLocalPosition() + movement);
}

void BasicEnemyAIScript::CheckStateChange(EnemyState previous, EnemyState newState)
{
	if (previous != newState)
	{
		switch (newState)
		{
		case EnemyState::WAIT:
			//anim->SendTriggerToStateMachine("Wait");
			enemyController->myRender->SetMaterial("White");
			break;
		case EnemyState::STANDUP:
			//anim->SendTriggerToStateMachine("StandUp");
			enemyController->myRender->SetMaterial("Blue");
			break;
		case EnemyState::CHASE:
			//anim->SendTriggerToStateMachine("Chase");
			enemyController->myRender->SetMaterial("Yellow");
			break;
		case EnemyState::RETURN:
			//anim->SendTriggerToStateMachine("Return");
			enemyController->myRender->SetMaterial("Green");
			break;
		case EnemyState::LAYDOWN:
			//anim->SendTriggerToStateMachine("Laydown");
			enemyController->myRender->SetMaterial("Blue");
			break;
		case EnemyState::ATTACK:
			//anim->SendTriggerToStateMachine("Attack");
			enemyController->myRender->SetMaterial("Red");
			break;
		case EnemyState::COOLDOWN:
			//anim->SendTriggerToStateMachine("Cooldown");
			enemyController->myRender->SetMaterial("White");
			break;
		case EnemyState::DEAD:
			//anim->SendTriggerToStateMachine("Dead");
			break;
		default:
			break;

		}
	}
}