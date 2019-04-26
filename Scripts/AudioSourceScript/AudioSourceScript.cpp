#include "AudioSourceScript.h"

#include "Application.h"

#include "ModuleInput.h"

#include "ComponentAudioSource.h"
#include "GameObject.h"

AudioSourceScript_API Script* CreateScript()
{
	AudioSourceScript* instance = new AudioSourceScript;
	return instance;
}

void AudioSourceScript::Start()
{
	audioSource = (ComponentAudioSource*) gameObject->GetComponent(ComponentType::AudioSource);
	if (audioSource == nullptr) LOG("The GameObject %s has no Audio Source component attached", gameObject->name);
}

void AudioSourceScript::Update()
{

	if (App->input->IsKeyPressed(SDL_SCANCODE_A)) {
		audioSource->Play();
	}


	audioSource->UpdateState();
}
