#ifndef __ModuleDevelopmentBuildDebug_h__
#define __ModuleDevelopmentBuildDebug_h__

#include <list>
#include "Module.h"
#include "SDL_events.h"

class Panel;
class PanelConsole;
class PanelInspector;
class PanelConfiguration;
class PanelAbout;
class PanelHardware;
class PanelHierarchy;
class PanelTime;
class GameObject;
class MaterialEditor;
class FileExplorer;

class ModuleDevelopmentBuildDebug:
	public Module
{
public:

	bool Init(JSON * config) override;
	update_status PreUpdate() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void RenderGUI() const;

	void processInput(SDL_Event* event) const;

	unsigned frames = 0u;
	float totalTime = .0f;
	float maxFps = .0f;
	float minFps = 100000000.f;
	float minTime = 100000000.f;
	float maxTime = .0f;

};

#endif //__ModuleEditor_h__
