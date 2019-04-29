#include "PlayerMovement.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"


#include <assert.h>
#include "imgui.h"
#include <iostream>
#include "Globals.h"
#include "JSON.h"

PlayerMovement_API Script* CreateScript()
{
	PlayerMovement* instance = new PlayerMovement;
	return instance;
}

void PlayerMovement::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);
}

void PlayerMovement::Start()
{
	anim = (ComponentAnimation*)gameObject->GetComponent(ComponentType::Animation);
	if (anim == nullptr) LOG("The GameObject %s has no Animation component attached \n", gameObject->name);

	LOG("Started player movement script");
}
void PlayerMovement::Update()
{
	PlayerState previous = playerState;
	playerState = PlayerState::IDLE;
	currentPosition = gameObject->transform->GetPosition();
	if(App->input->IsKeyPressed(SDL_SCANCODE_A))
	{
		currentPosition -= gameObject->transform->right * speed * App->time->gameDeltaTime;
		playerState = PlayerState::WALK;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		currentPosition += gameObject->transform->right * speed * App->time->gameDeltaTime;
		playerState = PlayerState::WALK;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
	{
		currentPosition -= gameObject->transform->front * speed * App->time->gameDeltaTime;
		playerState = PlayerState::WALK;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_W))
	{
		currentPosition += gameObject->transform->front * speed * App->time->gameDeltaTime;
		playerState = PlayerState::WALK;
	}
	gameObject->transform->SetPosition(currentPosition);
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
		}
	}
}


void PlayerMovement::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
}

void PlayerMovement::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
}

