#include "SkillTreeController.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentText.h"
#include "ComponentImage.h"

#include "imgui.h"
#include "JSON.h"
#include "Resource.h"
#include "Math/float4.h"
#include "ResourceTexture.h"

#define None "None Selected"

#pragma warning(disable : 4996)

SkillTreeController_API Script* CreateScript()
{
	SkillTreeController* instance = new SkillTreeController;
	return instance;
}

void SkillTreeController::Awake()
{
	
}

void SkillTreeController::Start()
{
	//Tree
	tree = App->scene->FindGameObjectByName(gameobject, "SkillsTree");
	skill = App->scene->FindGameObjectByName(tree, "Skill1")->GetComponent<ComponentImage>();
	counterBackground = App->scene->FindGameObjectByName(tree, "SkillCounterBackground1")->GetComponent<ComponentImage>();
	skillCounter = App->scene->FindGameObjectByName(tree, "SkillCounter1")->GetComponent<Text>();

	//Points
	skillPointsLabel = App->scene->FindGameObjectByName(gameobject, "SkillPoints")->GetComponent<Text>();
	skillPoints = 3;
	skillPointsLabel->text = std::to_string(skillPoints);


}

void SkillTreeController::Update()
{
	if (skill->isHovered && App->input->GetMouseButtonDown(1) == KEY_DOWN && skillPoints > 0)
	{
		++skillactualPoints;
		skillCounter->text = std::to_string(skillactualPoints) + "/5";
		skillCounter->color = math::float4(0,0,0,0);
		--skillPoints;
		skillPointsLabel->text = std::to_string(skillPoints);
	}
}

void SkillTreeController::AddSkillPoint()
{
	++skillPoints;
	skillPointsLabel->text = std::to_string(skillPoints);
}


void SkillTreeController::Expose(ImGuiContext* context)
{

	ImGui::SetCurrentContext(context);

	if (textureFiles.empty())
	{
		textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);
	}

	if (ImGui::Button("Refresh textures List"))
	{
		textureFiles.clear();
		textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);
	}

	for (int i = 0; i != 13; ++i) {

		skillList[i].id = i;
		
		ImGui::PushID(i);
		if (ImGui::CollapsingHeader(skillList[i].name.c_str()))
		{
			ImGui::Text("Skill id: %i", skillList[i].id);

			if (ImGui::BeginCombo("Texture Active", skillList[i].spriteActive != nullptr ? skillList[i].spriteActive->GetName() : None))
			{
				bool none_selected = (skillList[i].spriteActive == nullptr);
				if (ImGui::Selectable(None, none_selected))
				{
					if (skillList[i].spriteActive != nullptr)
					{
						App->resManager->DeleteResource(skillList[i].spriteActive->GetUID());
						skillList[i].spriteActive = nullptr;
					}
				}
				if (none_selected)
					ImGui::SetItemDefaultFocus();
				for (int n = 0; n < textureFiles.size(); n++)
				{
					bool is_selected = (skillList[i].spriteActive != nullptr && (std::string(skillList[i].spriteActive->GetName()) == textureFiles[n]));
					if (ImGui::Selectable(textureFiles[n].c_str(), is_selected) && !is_selected)
					{
						// Delete previous texture
						if (skillList[i].spriteActive != nullptr)
							App->resManager->DeleteResource(skillList[i].spriteActive->GetUID());

						skillList[i].spriteActive = (ResourceTexture*)App->resManager->GetByName(textureFiles[n].c_str(), TYPE::TEXTURE);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Texture inactive", skillList[i].spriteInactive != nullptr ? skillList[i].spriteInactive->GetName() : None))
			{
				bool none_selected = (skillList[i].spriteInactive == nullptr);
				if (ImGui::Selectable(None, none_selected))
				{
					if (skillList[i].spriteInactive != nullptr)
					{
						App->resManager->DeleteResource(skillList[i].spriteInactive->GetUID());
						skillList[i].spriteInactive = nullptr;
					}
				}
				if (none_selected)
					ImGui::SetItemDefaultFocus();
				for (int n = 0; n < textureFiles.size(); n++)
				{
					bool is_selected = (skillList[i].spriteInactive != nullptr && (std::string(skillList[i].spriteInactive->GetName()) == textureFiles[n]));
					if (ImGui::Selectable(textureFiles[n].c_str(), is_selected) && !is_selected)
					{
						// Delete previous texture
						if (skillList[i].spriteInactive != nullptr)
							App->resManager->DeleteResource(skillList[i].spriteInactive->GetUID());

						skillList[i].spriteInactive = (ResourceTexture*)App->resManager->GetByName(textureFiles[n].c_str(), TYPE::TEXTURE);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			char* imguiText = new char[64];
			strcpy(imguiText, skillList[i].name.c_str());
			if (ImGui::InputText("##", imguiText, 64, ImGuiInputTextFlags_EnterReturnsTrue))
				skillList[i].name = imguiText;
			delete[] imguiText;

			ImGui::InputInt("Num. Levels", &skillList[i].maxLevels);
			ImGui::InputInt("Next skill", &skillList[i].nextSkill);
			ImGui::InputInt("Previous skill", &skillList[i].previousSkill);

			ImGui::Checkbox("Locked", &skillList[i].locked);
		}
		ImGui::PopID();
	}

}


void SkillTreeController::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

	JSON_value *skillsJson = json->CreateValue(rapidjson::kArrayType);
	for (auto &skill : skillList)
	{
		JSON_value *skillJSON = skillsJson->CreateValue();
		skillJSON->AddInt("id", skill.id);
		skillJSON->AddString("name", skill.name.c_str());
		skillJSON->AddString("description", skill.description.c_str());
		skillJSON->AddInt("locked", skill.locked);
		skillJSON->AddInt("maxLevel", skill.maxLevels);
		skillJSON->AddInt("nextSkill", skill.nextSkill);
		skillJSON->AddInt("previousSkill", skill.previousSkill);
		skillJSON->AddUint("activeTextureUID", (skill.spriteActive != nullptr) ? skill.spriteActive->GetUID() : 0u);
		skillJSON->AddUint("inactiveTextureUID", (skill.spriteInactive != nullptr) ? skill.spriteInactive->GetUID() : 0u);
		skillsJson->AddValue("", *skillJSON);
	}

	json->AddValue("skills", *skillsJson);
}

void SkillTreeController::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	JSON_value* skillsJSON = json->GetValue("skills");
	for (unsigned i = 0; i < skillsJSON->Size(); i++)
	{
		JSON_value* skillJSON = skillsJSON->GetValue(i);
		skillList[i].id = skillJSON->GetInt("id");
		skillList[i].name = skillJSON->GetString("name");
		skillList[i].description = skillJSON->GetString("description");
		skillList[i].locked = skillJSON->GetInt("locked");
		skillList[i].maxLevels = skillJSON->GetInt("maxLevel");
		skillList[i].nextSkill = skillJSON->GetInt("nextSkill");
		skillList[i].previousSkill = skillJSON->GetInt("previousSkill");
		unsigned uid = skillJSON->GetUint("activeTextureUID");
		skillList[i].spriteActive = (ResourceTexture*)App->resManager->Get(uid);
		unsigned uidIn = skillJSON->GetUint("inactiveTextureUID");
		skillList[i].spriteInactive = (ResourceTexture*)App->resManager->Get(uidIn);
	}
}