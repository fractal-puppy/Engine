#include "DamageController.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentText.h"
#include "ComponentTransform2D.h"
#include <list>
#include <string>
#include "Math/float4.h"

DamageController_API Script* CreateScript()
{
	DamageController* instance = new DamageController;
	return instance;
}

void DamageController::Start()
{
	std::list<GameObject*> list = App->scene->FindGameObjectByName("Damage")->children;
	
	int k = 0;
	for (GameObject* const &i : list) {
		uiDamage[k++] = i;
	}

	for (int i = 0; i != 10; i++) {
		damageList[i] = Damage();
	}
}

void DamageController::Update()
{
	for (int i = 0; i != 10; ++i) {
		if (damageList[i].show)
		{
			damageList[i].actualPosition = damageList[i].actualPosition + (damageList[i].distanceNormalized * 100 * App->time->gameDeltaTime);
			uiDamage[i]->GetComponent<Transform2D>()->SetPositionUsingAligment(damageList[i].actualPosition);

			if (damageList[i].actualPosition.y > damageList[i].finalPosition.y)
			{
				uiDamage[i]->SetActive(false);
				damageList[i].show = false;
			}
		}
	}
}

bool DamageController::AddDamage(math::float2 position, int damage, int type)
{

	for (int i = 0; i != 10; i++) {
		if (!damageList[i].show)
		{
			damageList[i].damage = damage;
			damageList[i].initialPosition = damageList[i].actualPosition = position;
			damageList[i].type = (DamageType)type;
			damageList[i].show = true;
			damageList[i].finalPosition = math::float2(position.x, position.y + 100);
			damageList[i].distanceNormalized = (damageList[i].finalPosition - position).Normalized();
			
			uiDamage[i]->SetActive(true);
			uiDamage[i]->GetComponent<Text>()->text = std::to_string(damage);
			switch ((DamageType)type)
			{
			case DamageType::NORMAL:
				uiDamage[i]->GetComponent<Text>()->color = math::float4(1, 1, 1, 1);
				break;
			case DamageType::CRITICAL:
				uiDamage[i]->GetComponent<Text>()->color = math::float4(1, 1, 1, 1);
				break;
			case DamageType::POLLOMETER:
				uiDamage[i]->GetComponent<Text>()->color = math::float4(1, 1, 1, 1);
				break;
			case DamageType::HEALING:
				uiDamage[i]->GetComponent<Text>()->color = math::float4(0, 1, 0, 1);
				break;
			case DamageType::RECIEVED:
				uiDamage[i]->GetComponent<Text>()->color = math::float4(1, 0, 0, 1);
				break;
			default:
				uiDamage[i]->GetComponent<Text>()->color = math::float4(1, 1, 1, 1);
				break;
			}
			
			uiDamage[i]->GetComponent<Transform2D>()->SetPositionUsingAligment(position);

			return true;
		}
	}

	return false;
}