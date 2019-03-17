#include "IntroScript.h"

#include "Application.h"
#include "ModuleTime.h"

#include "ComponentTransform.h"
#include "GameObject.h"

#include "JSON.h"
#include "imgui.h"

IntroScript_API IntroScript* CreateScript()
{
	IntroScript* instance = new IntroScript;
	return instance;
}

void IntroScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::DragFloat("Speed", &speed, 1.0f, 0.0f, 100.0f);
	ImGui::DragFloat3("Final position", (float*)&finalPosition, 1.0f, -10000.0f, 10000.0f);
}

void IntroScript::Start()
{
	cameraPosition = gameObject->transform->GetPosition();

	float distanceZToMove = finalPosition.z - cameraPosition.z;
	float distanceYToMove = finalPosition.y - cameraPosition.y;
	float distanceXToMove = finalPosition.x - cameraPosition.x;

	if (finalPosition.z < cameraPosition.z)
	{
		speedZ = -1;
		distanceZToMove = cameraPosition.z - finalPosition.z;
	}
	if (finalPosition.y < cameraPosition.y)
	{
		speedY = -1;
		distanceYToMove = cameraPosition.y - finalPosition.y;
	}
	if (finalPosition.x < cameraPosition.x)
	{
		speedX = -1;
		distanceXToMove = cameraPosition.x - finalPosition.x;
	}

	if (distanceZToMove >= distanceYToMove && distanceZToMove >= distanceXToMove)
	{
		float divisions = distanceZToMove / speed;
		speedY = speedY * (distanceYToMove / divisions);
		speedX = speedX * (distanceXToMove / divisions);
		speedZ *= speed;
		distanceToMove = distanceZToMove;
	}
	else if (distanceYToMove >= distanceZToMove && distanceYToMove >= distanceXToMove)
	{
		float divisions = distanceYToMove / speed;
		speedZ = speedZ * (distanceZToMove / divisions);
		speedX = speedX * (distanceXToMove / divisions);
		speedY *= speed;
		distanceToMove = distanceYToMove;
	}
	else
	{
		float divisions = distanceXToMove / speed;
		speedZ = speedZ * (distanceZToMove / divisions);
		speedY = speedY * (distanceYToMove / divisions);
		speedX *= speed;
		distanceToMove = distanceXToMove;
	}
}

void IntroScript::Update()
{
	if (!introDone)
	{
		cameraPosition.z += (speedZ * App->time->gameDeltaTime);
		cameraPosition.y += (speedY * App->time->gameDeltaTime);
		cameraPosition.x += (speedX * App->time->gameDeltaTime);
		gameObject->transform->SetPosition(cameraPosition);
		distanceMoved += fabsf(speed * App->time->gameDeltaTime);

		if (distanceMoved >= distanceToMove)
			introDone = true;
	}
}

void IntroScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
	json->AddFloat3("finalPosition", finalPosition);
}

void IntroScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
	finalPosition = json->GetFloat3("finalPosition");
}
