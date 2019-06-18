#include "MeleeSkill.h"

#include "ComponentBoxTrigger.h"

#include "PlayerMovement.h"

MeleeSkill::MeleeSkill(PlayerMovement* PM, ComponentBoxTrigger* attackBox) :
	GenericSkill(PM), attackBoxTrigger(attackBox)
{
}

MeleeSkill::~MeleeSkill()
{
}

void MeleeSkill::Expose(ImGuiContext* context)
{
	GenericSkill::Expose(context);
}

void MeleeSkill::Serialize(JSON_value* json) const
{
	GenericSkill::Serialize(json);
}

void MeleeSkill::DeSerialize(JSON_value* json)
{
	GenericSkill::DeSerialize(json);
}

void MeleeSkill::Reset()
{
	GenericSkill::Reset();
}
