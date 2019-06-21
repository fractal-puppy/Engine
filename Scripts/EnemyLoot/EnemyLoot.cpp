#include "EnemyLoot.h"
#include "Application.h"
#include "Resource.h"
#include "ModuleScene.h"
#include "ModuleScript.h"
#include "ModuleResourceManager.h"

#include "ComponentTransform.h"

#include "GameObject.h"


#pragma warning(disable : 4996)

EnemyLoot_API Script* CreateScript()
{
	EnemyLoot* instance = new EnemyLoot;
	return instance;
}

void EnemyLoot::Start()
{
}

void EnemyLoot::Update()
{
}

void EnemyLoot::GenerateLoot()
{
	for (int i = 0; i < items.size(); ++i)
	{
		if ((rand() % 100) < items[i].second)
		{
			go = items[i].first;
			go->SetActive(true);
		}
		go->transform->scale = scale;
		go->parent = App->scene->root;
	}
	//items.clear();
}

void EnemyLoot::AddItem(std::string name, int drop)
{
	go = App->scene->FindGameObjectByName(name.c_str());
	items.push_back(std::make_pair(go, drop));
	go->parent = App->scene->selected;
	if (go->isActive())
	{
		go->SetActive(false);
	}
	scale = go->transform->scale;
	go->transform = App->scene->selected->transform;
}

void EnemyLoot::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("List of items to drop:");
	for (int i = 0; i < items.size(); ++i)
	{
		ImGui::Text("Item: ");
		ImGui::SameLine();
		ImGui::Text(items[i].first->name.c_str());
		ImGui::SameLine();
		ImGui::Separator();
		ImGui::SameLine();
		ImGui::Text("Drop:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(items[i].second).c_str());

	}
	if (items.size() == 0)
	{
		ImGui::Text("No items!");
	}

	std::vector<std::string> itemList = App->scene->GetItems();
	if (itemList.size() > 0 && ImGui::BeginCombo("Item name", itemName.c_str()))
	{
		for (int i = 0; i < itemList.size(); i++)
		{
			if (ImGui::Selectable(itemList[i].c_str()))
			{
				itemName = itemList[i];
			}
		}
		ImGui::EndCombo();
	}

	ImGui::DragInt("Drop %", &drop, 0.01f, 0, 100);

	if(ImGui::Button("Add item"))
	{
		AddItem(itemName,drop);
		goName = "GO Name";
		itemName = "";
		drop = 0;
	}
}
