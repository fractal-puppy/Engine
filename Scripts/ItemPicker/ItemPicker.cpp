#include "ItemPicker.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentRenderer.h"

#include "InventoryScript.h"
#include "ItemNameController.h"

#include "JSON.h"
#include "imgui.h"
#include "Resource.h"

#pragma warning(disable : 4996)

ItemPicker_API Script* CreateScript()
{
	ItemPicker* instance = new ItemPicker;
	return instance;
}

void ItemPicker::Expose(ImGuiContext* context)
{

	ImGui::SetCurrentContext(context);
	const char * types[] = { "NONE","QUICK","KEY","MATERIAL","WEAPON","HELMET","CHEST","PANTS","BOOTS","AMULET","RING" };
	const char * rarities[] = { "BASIC","RARE","EPIC","LEGENDARY" };
	if (ImGui::BeginCombo("Type", types[(int)type]))
	{
		for (int n = 0; n < 11; n++)
		{
			bool isSelected = ((int)type == n);
			if (ImGui::Selectable(types[n], isSelected) && (int)type != n)
			{
				type = (ItemType)n;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Rarity", rarities[(int)rarity]))
	{
		for (int n = 0; n < 4; n++)
		{
			bool isSelected = ((int)rarity == n);
			if (ImGui::Selectable(rarities[n], isSelected) && (int)rarity != n)
			{
				rarity = (ItemRarity)n;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (textureFiles.empty())
	{
		textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);
	}

	//texture selector
	if (ImGui::BeginCombo("Texture", sprite.c_str()))
	{
		bool none_selected = (sprite == "None");
		if (ImGui::Selectable("None", none_selected))
		{
			sprite = "None";
		}
		if (none_selected)
			ImGui::SetItemDefaultFocus();
		for (int n = 0; n < textureFiles.size(); n++)
		{
			bool is_selected = (sprite == textureFiles[n]);
			if (ImGui::Selectable(textureFiles[n].c_str(), is_selected) && !is_selected)
			{
				sprite = textureFiles[n].c_str();
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	char* imguiText = new char[64];
	strcpy(imguiText, name.c_str());
	ImGui::InputText("##", imguiText, 64);
	name = imguiText;
	delete[] imguiText;

	char* bboxName = new char[64];
	strcpy_s(bboxName, strlen(myBboxName.c_str()) + 1, myBboxName.c_str());
	ImGui::InputText("My BBox Name", bboxName, 64);
	myBboxName = bboxName;
	delete[] bboxName;

}

void ItemPicker::Start()
{
	inventoryScript = App->scene->FindGameObjectByName("Inventory")->GetComponent<InventoryScript>();
	GameObject* playerGO = App->scene->FindGameObjectByName("Player");
	playerBbox = &App->scene->FindGameObjectByName("PlayerMesh", playerGO)->bbox;

	myBboxName = gameobject->name;

	item.name = this->name;
	item.description = this->description;
	item.sprite = this->sprite;
	item.type = this->type;

	GameObject* itemNameGO = App->scene->FindGameObjectByName("ItemName");
	if (itemNameGO == nullptr)
	{
		LOG("Item name controller GO couldn't be found \n");
	}
	else
	{
		itemName = itemNameGO->GetComponent<ItemNameController>();
		if (itemName != nullptr)
		{
			LOG("Item name controller couldn't be found \n");
		}
	}
}

void ItemPicker::Update()
{
	if (gameobject->isActive() && gameobject->bbox.Intersects(*playerBbox) && (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN))
	{
		if (inventoryScript->AddItem(item))
			gameobject->SetActive(false);
	}
	math::float3 closestPoint;

	GameObject* myRenderGO = App->scene->FindGameObjectByName(myBboxName.c_str(), gameobject);
	if (myRenderGO != nullptr)
		myRender = (ComponentRenderer*)myRenderGO->GetComponent<ComponentRenderer>();
	switch (rarity)
	{
	case ItemRarity::BASIC:
		myRender->highlightColor = math::float3(211, 211, 211);
		rare = 0;
		break;
	case ItemRarity::RARE:
		myRender->highlightColor = math::float3(0, 255, 0);
		rare = 1;
		break;
	case ItemRarity::EPIC:
		myRender->highlightColor = math::float3(255, 69, 0);
		rare = 2;
		break;
	case ItemRarity::LEGENDARY:
		myRender->highlightColor = math::float3(148, 0, 211);
		rare = 3;
		break;
	}

	if (App->scene->Intersects(closestPoint, myBboxName.c_str()))
	{
		if (itemName != nullptr)
			itemName->SetNameBar(gameobject->name, rare);

		if (myRender != nullptr)
			myRender->highlighted = true;
	}
	else
	{
		if (myRender != nullptr)
			myRender->highlighted = false;
	}
}

void ItemPicker::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddString("name", name.c_str());
	json->AddString("sprite", sprite.c_str());
	json->AddInt("type", (int)type);
}

void ItemPicker::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	name = json->GetString("name");
	sprite = json->GetString("sprite");
	type = (ItemType)json->GetInt("type");
}

void ItemPicker::SetItem(ItemType type, std::string name, std::string sprite)
{
	item.name = name;
	item.sprite = sprite;
	item.type = type;
}