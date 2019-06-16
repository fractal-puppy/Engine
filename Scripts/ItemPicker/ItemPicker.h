#ifndef  __ItemPicker_h__
#define  __ItemPicker_h__

#include "BaseScript.h"
#include "Item.h"
#include "Geometry/AABB.h"
#include <vector>

#ifdef ItemPicker_EXPORTS
#define ItemPicker_API __declspec(dllexport)
#else
#define ItemPicker_API __declspec(dllimport)
#endif

class InventoryScript;
class AABB;
class JSON_value;
class ComponentRenderer;
class ItemNameController;

enum class ItemRarity
{
	BASIC = 0,
	RARE,
	EPIC,
	LEGENDARY
};

class ItemPicker_API ItemPicker : public Script
{
	void Expose(ImGuiContext* context) override;

	void Start() override;
	void Update() override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;
	void SetItem(ItemType type, std::string name, std::string sprite);

	std::string name;
	std::string description;
	std::string sprite;
	ItemType type = ItemType::NONE;
	Item item;

private:

	InventoryScript* inventoryScript = nullptr;
	math::AABB* playerBbox = nullptr;

	std::vector<std::string> textureFiles;
	const char * selectedTexture = "basicPostion";

	std::string myBboxName;

	ComponentRenderer* myRender;
	ItemRarity rarity = ItemRarity::BASIC;

	ItemNameController* itemName = nullptr;
};

#endif __ItemPicker_h__
