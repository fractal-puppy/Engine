#include "PlayerMovement.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "ModuleNavigation.h"

#include "ComponentAnimation.h"
#include "ComponentTransform.h"
#include "GameObject.h"

#include "JSON.h"
#include <assert.h>
#include "imgui.h"
#include "Globals.h"
#include "debugdraw.h"

#define CLOSE_ENOUGH 400.0f

PlayerMovement_API Script* CreateScript()
{
	PlayerMovement* instance = new PlayerMovement;
	return instance;
}

void PlayerMovement::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);
	ImGui::InputFloat("attackDuration", &attackDuration);
}

void PlayerMovement::Start()
{
	anim = (ComponentAnimation*)gameObject->GetComponent(ComponentType::Animation);
	if (anim == nullptr)
	{
		LOG("The GameObject %s has no Animation component attached \n", gameObject->name);
	}

	LOG("Started player movement script");
}
void PlayerMovement::Update()
{
	if (isPlayerDead) return;

	PlayerState previous = playerState;
	if (playerState == PlayerState::ATTACK)
	{
		attackTimer += App->time->gameDeltaTime;
		if (attackTimer >= attackDuration)
		{
			attackTimer = 0.0f;
			playerState = PlayerState::IDLE;
		}
	}
	if (App->input->GetMouseButtonDown(3) == KEY_DOWN) //RIGHT BUTTON
	{
		math::float3 intersectionPoint = math::float3::inf;
		if (App->scene->Intersects(intersectionPoint, "floor"))
		{
			App->navigation->FindPath(gameObject->transform->position, intersectionPoint, path);
			pathIndex = 0;
		}
	}
	if (path.size() > 0)
	{
		math::float3 currentPosition = gameObject->transform->GetLocalPosition();
		while(pathIndex < path.size() && currentPosition.DistanceSq(path[pathIndex]) < CLOSE_ENOUGH)
		{
			pathIndex++;
		}
		if (pathIndex < path.size())
		{
			gameObject->transform->LookAt(path[pathIndex]);
			math::float3 direction = (path[pathIndex] - currentPosition).Normalized();
			gameObject->transform->SetLocalPosition(currentPosition + speed*direction*App->time->gameDeltaTime);
			playerState = PlayerState::WALK;
		}
		else
		{
			playerState = PlayerState::IDLE;
		}
	}
	else if (playerState != PlayerState::ATTACK)
	{
		playerState = PlayerState::IDLE;
	}

	if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
	{
		pathIndex = 0;
		path.clear();
		playerState = PlayerState::ATTACK;
		math::float3 attackPosition;
		if (App->scene->Intersects(attackPosition, "floor"))
		{
			gameObject->transform->LookAt(attackPosition);
		}
	}
	CheckState(previous, playerState);
}

void PlayerMovement::CheckState(PlayerState previous, PlayerState current)
{
	if (previous != current)
	{
		switch (current)
		{
		case PlayerState::IDLE:
			anim->SendTriggerToStateMachine("Idle");
			break;
		case PlayerState::WALK:
			anim->SendTriggerToStateMachine("Walk");
			break;
		case PlayerState::ATTACK:
			anim->SendTriggerToStateMachine("Attack");
			break;
		}
	}
}

PlayerMovement_API void PlayerMovement::Damage(float amount)
{
	health -= amount;
	if (health < 0)
	{
		isPlayerDead = true;
	}
}

void PlayerMovement::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
	json->AddFloat("attack", attackDuration);
}

void PlayerMovement::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
	attackDuration = json->GetFloat("attack");
}
