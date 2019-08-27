#include "WorldControllerScript.h"

#include "Application.h"
#include "GameObject.h"
#include "ModuleNavigation.h"
#include "ComponentTransform.h"

#include "Globals.h"

WorldControllerScript_API Script* CreateScript()
{
	WorldControllerScript* instance = new WorldControllerScript;
	return instance;
}

WorldControllerScript::WorldControllerScript()
{
}

WorldControllerScript::~WorldControllerScript()
{
	//no need to delete crowdToolPointer since its a smart pointer and maganes itself
	objectAgentIndexMap.clear();
	agentObjects.clear();
}

void WorldControllerScript::resetWorldAgents()
{
	crowdToolPointer.reset();//delete smart pointer calling the destructor
	crowdToolPointer = std::make_unique<crowdTool>();//create a new instance
	//reset our map aswell
	objectAgentIndexMap.clear();
	agentObjects.clear();
}

int WorldControllerScript::addAgent(const float* pos)
{
	return crowdToolPointer.get()->AddNewAgent(pos);
}

void WorldControllerScript::setPlayer(GameObject* player)
{
	playerUID = player->UUID;
	int newAgentId = addAgent(player->transform->crowdPosition);
	//once we created the agent, save the info in our map
	objectAgentIndexMap[player->UUID] = newAgentId;
	//we also keep track of this gameObject
	agentObjects.push_back(player->transform);
}

void WorldControllerScript::addEnemy(GameObject* enemy)
{
	int newAgentId = addAgent(enemy->transform->crowdPosition);
	//once we created the agent, save the info in our map
	objectAgentIndexMap[enemy->UUID] = newAgentId;
	//we also keep track of this gameObject
	agentObjects.push_back(enemy->transform);
}

void WorldControllerScript::moveRequest(GameObject* agent)
{
	

}

void WorldControllerScript::Update()
{
	//go over all the objects in the crowd and update their pointer to the transform because thats what detour uses
	for (ComponentTransform* agent : agentObjects)
	{
		agent->UpdateCrowdTransform();
	}

	//parameter is 0.01 which communicates to crowd update that it has 10ms to do the stuff
	crowdToolPointer.get()->UpdateCrowd(0.01f);

	//and now we update the math::float3 transform because detour may have used/updated them
	for (ComponentTransform* agent : agentObjects)
	{
		agent->UpdatePositionWithCrowdValue();
	}
}
