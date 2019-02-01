#include "BaseScript.h"

#include "GameObject.h"
#include "Globals.h"
#include "imgui.h"
#include <iostream>

void Script::SetGameObject(GameObject * go)
{
	gameObject = go;
}

void Script::Expose(ImGuiContext *context)
{
	ImGui::SetCurrentContext(context);
}

void Script::Start()
{
}

void Script::Update()
{
}

Script* CreateScript()
{
	Script* instance = new Script;
	return instance;
}