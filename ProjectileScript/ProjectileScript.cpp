#include "ProjectileScript.h"

ProjectileScript_API Script* CreateScript()
{
	ProjectileScript* instance = new ProjectileScript;
	return instance;
}
