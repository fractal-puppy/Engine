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
	itemName = App->scene->FindGameObjectByName("ItemName");
	itemBackground = App->scene->FindGameObjectByName("ItemBackground", itemName);
	itemTypeName = App->scene->FindGameObjectByName("ItemTypeName", itemName);
	myText = itemTypeName->GetComponent<Text>();
}

void ItemNameController::Update()
{
	itemBackground->SetActive(false);
	itemTypeName->SetActive(false);
}

void ItemNameController::SetNameBar(std::string name, int rarity)
{
	myText->text = name;
	switch (rarity)
	{
	case 0:
		myText->color = math::float4(211.0f, 211.0f, 211.0f, 1.0f);
		break;
	case 1:
		myText->color = math::float4(0.0f, 255.0f, 0.0f,1.0f);
		break;
	case 2:
		myText->color = math::float4(255.0f, 69.0f, 0.0f,1.0f);
		break;
	case 3:
		myText->color = math::float4(148.0f, 0.0f, 211.0f,1.0f);
		break;
	}
	itemBackground->SetActive(true);
	itemTypeName->SetActive(true);
}