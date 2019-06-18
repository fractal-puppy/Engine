#include "SliceSkill.h"

#include "PlayerMovement.h"

#include "imgui.h"
#include "JSON.h"

SliceSkill::SliceSkill(PlayerMovement* PM) :
	MeleeSkill(PM)
{
}

SliceSkill::~SliceSkill()
{
}

void SliceSkill::Start()
{
}

void SliceSkill::Update()
{

}

void SliceSkill::Expose(ImGuiContext* context)
{
}

void SliceSkill::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

}

void SliceSkill::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);

}

void SliceSkill::UseSkill()
{
}
