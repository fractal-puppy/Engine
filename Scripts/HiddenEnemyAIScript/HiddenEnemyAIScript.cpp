#include "HiddenEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ComponentBoxTrigger.h"

#include "EnemyControllerScript.h"
#include "EnemyState.h"
#include "EnemyStateWait.h"
#include "EnemyStateShowUp.h"
#include "EnemyStateChase.h"
#include "EnemyStateReturnToStart.h"
#include "EnemyStateHide.h"
#include "EnemyStateAttack.h"
#include "EnemyStateCooldown.h"
#include "EnemyStateDeath.h"

#include "imgui.h"
#include "JSON.h"

HiddenEnemyAIScript_API Script* CreateScript()
{
	HiddenEnemyAIScript* instance = new HiddenEnemyAIScript;
	return instance;
}

void HiddenEnemyAIScript::Start()
{
	enemyStates.reserve(8);

	enemyStates.push_back(wait = new EnemyStateWait(this));
	enemyStates.push_back(showUp = new EnemyStateShowUp(this));
	enemyStates.push_back(chase = new EnemyStateChase(this));
	enemyStates.push_back(returnToStart = new EnemyStateReturnToStart(this));
	enemyStates.push_back(hide = new EnemyStateHide(this));
	enemyStates.push_back(attack = new EnemyStateAttack(this));
	enemyStates.push_back(cooldown = new EnemyStateCooldown(this));
	enemyStates.push_back(death = new EnemyStateDeath(this));
	
	currentState = wait;

	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name);
	}

	boxTrigger = (ComponentBoxTrigger*)gameobject->GetComponentInChildren(ComponentType::BoxTrigger);
	if (boxTrigger == nullptr)
	{
		LOG("No child of the GameObject %s has a boxTrigger component attached \n", gameobject->name);
	}

	startPosition = enemyController->GetPosition();

	LOG("Started hidden enemy AI script");
}

void HiddenEnemyAIScript::Update()
{
	EnemyState* previous = currentState;

	if (enemyController->GetHealth() <= 0 && currentState != death)
	{
		currentState = (EnemyState*)death;
	}

	currentState->UpdateTimer();
	currentState->Update();

	// If previous and current are different the functions Exit() and Enter() are called
	CheckStates(previous, currentState);
}

void HiddenEnemyAIScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("Enemy:");

	if (currentState == wait)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Wait");
	else if (currentState == showUp)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Show Up");
	else if (currentState == chase)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");
	else if (currentState == returnToStart)	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");
	else if (currentState == hide)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Hide");
	else if (currentState == attack)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Attack");
	else if (currentState == cooldown)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cooldown");
	else if (currentState == death)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");

	ImGui::Text("Show up:");
	ImGui::InputFloat("Distance to activate", &activationDistance);
	ImGui::InputFloat("Show-up Speed", &showUpSpeed);
	ImGui::Text("Chase:");
	ImGui::InputFloat("Y Translation", &yTranslation);
	ImGui::InputFloat("Chase Speed", &chaseSpeed);
	ImGui::Text("Return:");
	ImGui::InputFloat("Return Distance", &returnDistance);
	ImGui::InputFloat("Return Speed", &returnSpeed);
	ImGui::Text("Attack:");
	ImGui::InputFloat("Attack Time", &attackDuration);
	ImGui::InputFloat("Attack Damage", &attackDamage);
	ImGui::Text("Cooldown:");
	ImGui::InputFloat("Cooldown Time", &cooldownTime);
}

void HiddenEnemyAIScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

	//Wait variables
	json->AddFloat("activationDistance", activationDistance);

	// Stand-Up variables
	json->AddFloat("showUpSpeed", showUpSpeed);
	json->AddFloat("yTranslation", yTranslation);

	// Chase variables
	json->AddFloat("chaseSpeed", chaseSpeed);

	// Return variables
	json->AddFloat("returnSpeed", returnSpeed);
	json->AddFloat("returnDistance", returnDistance);

	// Attack variables
	json->AddFloat("attackDuration", attackDuration);
	json->AddFloat("attackDamage", attackDamage);

	// Cooldown variables
	json->AddFloat("cooldownTime", cooldownTime);
}

void HiddenEnemyAIScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);

	//Wait variables
	activationDistance = json->GetFloat("activationDistance");

	// Stand-Up variables
	showUpSpeed = json->GetFloat("showUpSpeed");
	yTranslation = json->GetFloat("yTranslation");

	// Chase variables
	chaseSpeed = json->GetFloat("chaseSpeed");

	// Return variables
	returnSpeed = json->GetFloat("returnSpeed");
	returnDistance = json->GetFloat("returnDistance");

	// Attack variables
	attackDuration = json->GetFloat("attackDuration");
	attackDamage = json->GetFloat("attackDamage");

	// Cooldown variables
	cooldownTime = json->GetFloat("cooldownTime");
}

void HiddenEnemyAIScript::CheckStates(EnemyState* previous, EnemyState* current)
{
	if (previous != current)
	{
		previous->ResetTimer();

		previous->Exit();
		current->Enter();

		if (enemyController->anim != nullptr)
		{
			enemyController->anim->SendTriggerToStateMachine(current->trigger.c_str());
		}
	}
}