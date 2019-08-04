#include "FlyingSkullEnemyAIScript.h"

#include "EnemyControllerScript.h"

#include "EnemyStateAttack.h"
#include "EnemyStateChase.h"
#include "EnemyStateCooldown.h"
#include "EnemyStateDeath.h"
#include "EnemyStatePatrol.h"
#include "EnemyStateFlee.h"
#include "EnemyStateReturn.h"

#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentRenderer.h"

#include "imgui.h"
#include <assert.h>
#include "JSON.h"

FlyingSkullEnemyAIScript_API Script* CreateScript()
{
	FlyingSkullEnemyAIScript* instance = new FlyingSkullEnemyAIScript;
	return instance;
}

void FlyingSkullEnemyAIScript::Awake()
{
	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name);
	}
}

void FlyingSkullEnemyAIScript::Start()
{
	//Create states
	enemyStates.reserve(7);
	enemyStates.push_back(attack		= new EnemyStateAttack(this));
	enemyStates.push_back(cooldown		= new EnemyStateCooldown(this));
	enemyStates.push_back(returnToStart = new EnemyStateReturn(this));
	enemyStates.push_back(chase			= new EnemyStateChase(this));
	enemyStates.push_back(death			= new EnemyStateDeath(this));
	enemyStates.push_back(flee			= new EnemyStateFlee(this));
	enemyStates.push_back(patrol		= new EnemyStatePatrol(this));

	currentState = patrol;

	startPosition = enemyController->GetPosition();

	boneCenter = App->scene->FindGameObjectByName("BoneCenter", gameobject);
	assert(boneCenter != nullptr);
	if (boneCenter == nullptr)
	{
		LOG("No BoneCenter GO found");
	}

	spinningBones = App->scene->FindGameObjectByName("BoneCenter", gameobject);
	assert(spinningBones != nullptr);
	if (spinningBones == nullptr)
	{
		LOG("No spinningBones GO found");
	}

	particles = App->scene->FindGameObjectByName("BoneCenter", gameobject);
	assert(particles != nullptr);
	if (particles == nullptr)
	{
		LOG("No particles GO found");
	}
}

void FlyingSkullEnemyAIScript::Update()
{
	if (enemyController->GetHealth() <= 0 && currentState != death)
	{
		currentState = (EnemyState*)death;
	}

	EnemyState* previous = currentState;

	currentState->UpdateTimer();
	currentState->HandleIA();

	CheckStates(previous);

	currentState->Update();
}

void FlyingSkullEnemyAIScript::Expose(ImGuiContext * context)
{

	if (currentState == patrol)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: patrol");
	else if (currentState == chase)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");
	else if (currentState == returnToStart)	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");
	else if (currentState == attack)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Attack");
	else if (currentState == cooldown)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cooldown");
	else if (currentState == death)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");
	else if (currentState == flee)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: flee");


	ImGui::InputFloat("attack cooldown duration: ", &cooldownDuration);
	ImGui::InputFloat("activation distance: ", &activationDistance);
	ImGui::InputFloat("disengage distance: ", &disengageDistance);
	ImGui::InputFloat("max attack range: ", &maxAttackRange);
	ImGui::InputFloat("min attack range: ", &minAttackRange);
	ImGui::InputFloat("Projectile Delay: ", &projectileDelay);
	ImGui::InputFloat("run speed: ", &runSpeed);
}

void FlyingSkullEnemyAIScript::Serialize(JSON_value * json) const
{
	json->AddFloat("cooldownDuration", cooldownDuration);
	json->AddFloat("activationDistance", activationDistance);
	json->AddFloat("disengageDistance", disengageDistance);
	json->AddFloat("maxAttackRange", maxAttackRange);
	json->AddFloat("minAttackRange", minAttackRange);
	json->AddFloat("projectileDelay", projectileDelay);
	json->AddFloat("runSpeed", runSpeed);
}

void FlyingSkullEnemyAIScript::DeSerialize(JSON_value * json)
{
	cooldownDuration = json->GetFloat("cooldownDuration", 0);
	activationDistance = json->GetFloat("activationDistance",0);
	disengageDistance = json->GetFloat("disengageDistance",0);
	maxAttackRange = json->GetFloat("maxAttackRange",0);
	minAttackRange = json->GetFloat("minAttackRange",0);
	projectileDelay = json->GetFloat("projectileDelay", 0);
	runSpeed = json->GetFloat("runSpeed",0);
}

void FlyingSkullEnemyAIScript::CheckStates(EnemyState* previous)
{
	if (previous != currentState)
	{
		previous->Exit();
		currentState->Enter();
	}
}
