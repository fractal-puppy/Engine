#include "ExperienceController.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ComponentText.h"
#include "ComponentImage.h"

#include "SkillTreeController.h"

ExperienceController_API Script* CreateScript()
{
	ExperienceController* instance = new ExperienceController;
	return instance;
}

void ExperienceController::Start()
{
	GameObject* levelInventory = App->scene->FindGameObjectByName("LevelBackground");
	assert(levelInventory != nullptr);
	xpProgressInventory = App->scene->FindGameObjectByName("LevelProgressBar", levelInventory)->GetComponent<ComponentImage>();
	assert(xpProgressInventory != nullptr);
	xpProgressHUD = App->scene->FindGameObjectByName("XpProgress", gameobject)->GetComponent<ComponentImage>();
	assert(xpProgressHUD != nullptr);
	levelText = App->scene->FindGameObjectByName("LevelActual", levelInventory)->GetComponent<Text>();
	assert(levelText != nullptr);
	xpText = App->scene->FindGameObjectByName("LevelExperience", levelInventory)->GetComponent<Text>();
	assert(xpText != nullptr);
	levelUPGO = App->scene->FindGameObjectByName("LevelUP");
	assert(levelUPGO != nullptr);
	levelReached = App->scene->FindGameObjectByName("LevelReached", levelUPGO)->GetComponent<Text>();
	assert(levelReached != nullptr);
	

	skillTreeScript = App->scene->FindGameObjectByName("Skills")->GetComponent<SkillTreeController>();
}

void ExperienceController::Update()
{
	if (levelUP)
	{
		currentTime += App->time->gameDeltaTime;
		if (currentTime >= timeShowing)
		{
			levelUPGO->SetActive(false);
			levelUP = false;
			currentTime = 0;
		}
	}
}

void ExperienceController::AddXP(int xp)
{
	totalXPAcumulated += xp;
	if (currentLevel < maxLevel)
	{
		updateXP = true;
		previousXP = currentXP;
		currentXP += xp;
		if (currentXP >= maxXPLevel)
		{
			while (currentXP >= maxXPLevel)
			{
				++currentLevel;
				if (currentLevel == maxLevel)
				{
					currentXP = maxXPLevel;
					break;
				}
				currentXP -= maxXPLevel;
				maxXPLevel = levelsExp[currentLevel - 1];
				skillTreeScript->AddSkillPoint();
				levelUP = true;
			}
			levelText->text = "LVL " + std::to_string(currentLevel);
			levelReached->text = "LEVEL " + std::to_string(currentLevel) + " REACHED";
			levelUPGO->SetActive(true);
		}
		int mask = (currentXP * 100) / maxXPLevel;
		xpText->text = std::to_string(currentXP) + "/" + std::to_string(maxXPLevel);
		xpProgressHUD->SetMaskAmount(mask);
		xpProgressInventory->SetMaskAmount(mask);
	}
}
