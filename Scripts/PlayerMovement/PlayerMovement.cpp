#include "PlayerMovement.h"

#include "Application.h"
#include "ModuleInput.h"

#include "ComponentTransform.h"
#include "GameObject.h"
#include "Math/float3.h"

#include "JSON.h"
#include <assert.h>
#include "imgui.h"
#include <iostream>
#include "Globals.h"

PlayerMovement_API PlayerMovement* CreateScript()
{
	PlayerMovement* instance = new PlayerMovement;
	return instance;
}

void PlayerMovement::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);
}

void PlayerMovement::Update()
{
	if(App->input->IsKeyPressed(SDL_SCANCODE_W))
	{
		gameObject->transform->position.z -= speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
	{
		gameObject->transform->position.z += speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
	{
		gameObject->transform->position.x -= speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		gameObject->transform->position.x += speed;
	}
	LOG("test");
	float3 a(1.0f, 2.0f, 3.0f);
	a.AngleBetween(a);
	a += a;
	//LOG("PLAYER POSITION: (%d,%d)", gameObject->transform->position.x, gameObject->transform->position.z);
}

void PlayerMovement::Serialize(JSON_value * json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
}

void PlayerMovement::DeSerialize(JSON_value * json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
}
