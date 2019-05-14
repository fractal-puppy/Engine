#include "ItemPicker.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

#include "InventoryScript.h"

#include "imgui.h"

ItemPicker_API Script* CreateScript()
{
	ItemPicker* instance = new ItemPicker;
	return instance;
}

void ItemPicker::Expose(ImGuiContext* context)
{
	//Expose Item attributes
}


void ItemPicker::Start()
{
	inventoryScript = App->scene->FindGameObjectByName("Inventory")->GetComponent<InventoryScript>();
	GameObject* playerGO = App->scene->FindGameObjectByName("Player");
	playerBbox = &App->scene->FindGameObjectByName(playerGO, "PlayerMesh")->bbox;

	item.name = this->name;
	item.description = this->description;
	item.sprite = this->sprite;
	item.type = this->type;
}

void ItemPicker::Update()
{
	if (gameobject->isActive() && gameobject->bbox.Intersects(*playerBbox))
	{
		inventoryScript->AddItem(item);
		gameobject->SetActive(false);
	}
}