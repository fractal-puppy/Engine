#include "ItemNameController.h"

#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

//#include "Component.h"
#include "ComponentImage.h"
#include "ComponentRenderer.h"
#include "ComponentText.h"

ItemNameController_API Script* CreateScript()
{
	ItemNameController* instance = new ItemNameController;
	return instance;
}

void ItemNameController::Start()
{
	GameObject* itemName = App->scene->FindGameObjectByName("ItemName");
	itemBackground = App->scene->FindGameObjectByName("ItemBackground", itemName);
	itemTypeName = App->scene->FindGameObjectByName("ItemTypeName", itemName);
	myText = itemTypeName->GetComponent<ComponentText>();
}

void ItemNameController::Update()
{
	gameobject->SetActive(false);
}

void ItemNameController::SetNameBar(std::string name)
{
	gameobject->SetActive(true);
	myText->
}