#include "CombatAudioEvents.h"
#include "Application.h"

#include "ModuleScene.h"
#include "GameObject.h"

#include "ComponentAudioSource.h"

CombatAudioEvents_API Script* CreateScript()
{
	CombatAudioEvents* instance = new CombatAudioEvents;
	return instance;
}

void CombatAudioEvents::Start()
{
	GameObject* GO = App->scene->FindGameObjectByName("m1AttackSound");
	if (GO != nullptr)
	{
		m1AttackSound = GO->GetComponent<ComponentAudioSource>();
		assert(m1AttackSound != nullptr);
	}
	else
	{
		LOG("Warning: m1AttackSound game object not found");
	}

	GO = App->scene->FindGameObjectByName("stepSound");
	if (GO != nullptr)
	{
		stepSound = GO->GetComponent<ComponentAudioSource>();
		assert(stepSound != nullptr);
	}
	else
	{
		LOG("Warning: stepSound game object not found");
	}
}

void CombatAudioEvents::OnAnimationEvent(std::string name)
{
	if (name == "step")
	{
		stepSound->Play();
	}	
}


