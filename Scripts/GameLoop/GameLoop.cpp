#include "GameLoop.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentButton.h"
#include "ComponentText.h"
#include "ComponentTransform.h"
#include "ComponentScript.h"

#include "PlayerMovement.h"
#include "EnemyMovementScript.h"

#include "Globals.h"

GameLoop_API Script* CreateScript()
{
	GameLoop* instance = new GameLoop;
	return instance;
}

void GameLoop::Start()
{
	LOG("Started GameLoop script");
	menu = App->scene->FindGameObjectByName("Menu");
	assert(menu != nullptr);

	options = App->scene->FindGameObjectByName("OptionsMenu");
	assert(options != nullptr);

	controls = App->scene->FindGameObjectByName("ControlsMenu");
	assert(controls != nullptr);

	GameObject* mainMenuGO = App->scene->FindGameObjectByName("MainMenu");
	assert(mainMenuGO != nullptr);
	menuButtons = mainMenuGO->GetComponentsInChildren(ComponentType::Button);
																
	GameObject* optionsGO = App->scene->FindGameObjectByName("ButtonOptions");
	assert(optionsGO != nullptr);
	optionButton = (ComponentButton*)optionsGO->GetComponent(ComponentType::Button);

	GameObject* controlsGO = App->scene->FindGameObjectByName("ButtonControls");
	assert(controlsGO != nullptr);
	controlsButton = (ComponentButton*)controlsGO->GetComponent(ComponentType::Button);

	GameObject* musicGO = App->scene->FindGameObjectByName("MusicRow");
	assert(musicGO != nullptr);
	volumeButtons = musicGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* volumeGO = App->scene->FindGameObjectByName(musicGO, "MusicNumberText");
	volumeText = (ComponentText*) volumeGO->GetComponent(ComponentType::Text);//MusicNumberText

	GameObject* soundGO = App->scene->FindGameObjectByName("SoundRow");
	assert(soundGO != nullptr);
	soundButtons = soundGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* soundVolumeGO = App->scene->FindGameObjectByName(soundGO, "SoundNumberText");
	soundText = (ComponentText*)soundVolumeGO->GetComponent(ComponentType::Text);//SoundNumberText

	GameObject* backOptionsGO = App->scene->FindGameObjectByName(options, "Back");
	backOptionButton = (ComponentButton *)backOptionsGO->GetComponent(ComponentType::Button);
	assert(backOptionButton != nullptr);

	GameObject* backControlsGO = App->scene->FindGameObjectByName(controls, "Back");
	backControlsButton = (ComponentButton *)backControlsGO->GetComponent(ComponentType::Button);
	assert(backControlsButton != nullptr);

	playerGO = App->scene->FindGameObjectByName("Player");
	playerScript = (PlayerMovement*)playerGO->GetScript();
	assert(playerScript != nullptr);

	enemyGO = App->scene->FindGameObjectByName("Enemy");
	enemyMovementScript = (EnemyMovementScript*)enemyGO->GetScript();
	assert(enemyMovementScript != nullptr);

	loseWindow = App->scene->FindGameObjectByName("LoseWindow");
	assert(loseWindow != nullptr);

	creditsGO = App->scene->FindGameObjectByName("Credits");
	assert(creditsGO != nullptr);

	GameObject* toTheAltarGO = App->scene->FindGameObjectByName(loseWindow, "Button");
	toTheAltarButton = (ComponentButton *)toTheAltarGO->GetComponent(ComponentType::Button);
	assert(toTheAltarButton != nullptr);

	winWindow = App->scene->FindGameObjectByName("WinWindow");
	assert(winWindow != nullptr);

	hudGO = App->scene->FindGameObjectByName("GameHUB");
	assert(winWindow != nullptr);

	winGO = App->scene->FindGameObjectByName("WinZone");
	assert(winGO != nullptr);
}

void GameLoop::Update()
{
	switch (gameState)
	{
		case GameState::MENU:
		{
			//Manage menu stuff
			ManageMenu();
			break;
		}
		case GameState::INTRO:
		{
			//When the intro is done go to PLAYING
			ManageIntro();
			break;
		}
		case GameState::PLAYING:
		{
			//Update player / enemies / check for collisions
			ManagePlaying();
			break;
		}
		case GameState::WIN:
		{
			ManageWin();
			break;
		}
		case GameState::DEAD:
		{
			ManageDead();
			break;
		}
		case GameState::PAUSED:
		{
			//Wait until pause is removed // No update player / enemies / no check for collisions
			ManagePaused();
			break;
		}
		case GameState::OPTIONS:
		{
			//Show credits
			ManageOptions();
			break;
		}
		case GameState::CREDITS:
		{
			//Show credits
			ManageCredits();
			break;
		}
		case GameState::CONTROLS:
		{
			//Show credits
			ManageControls();
			break;
		}
		case GameState::QUIT:
		{
			//Close App
			ManageQuit();
			break;
		}

	}
}

void GameLoop::ManageDead()
{
	if (toTheAltarButton->IsPressed())
	{
		playerGO->transform->SetPosition(playerStartPosition);
		playerScript->isPlayerDead = false;
		enemyGO->transform->SetPosition(enemyStartPosition);
		enemyMovementScript->stop = false;
		ChangeGameState(GameState::PLAYING);
		loseWindow->SetActive(false);
	}
}

void GameLoop::ManageMenu()
{
	if (((ComponentButton*)menuButtons[0])->IsPressed()) //PlayButton
	{
		menu->SetActive(false);
		ChangeGameState(GameState::INTRO);
	}
	else if (optionButton->IsPressed())
	{
		options->SetActive(true);
		EnableMenuButtons(false);
		ChangeGameState(GameState::OPTIONS);
	}
	else if (controlsButton->IsPressed())
	{
		controls->SetActive(true);
		EnableMenuButtons(false);
		ChangeGameState(GameState::CONTROLS);
	}
}

void GameLoop::ManageIntro()
{
	hudGO->SetActive(true);
	playerStartPosition = playerGO->transform->GetGlobalPosition();
	enemyStartPosition = enemyGO->transform->GetGlobalPosition();
	ChangeGameState(GameState::PLAYING);
}

void GameLoop::ManagePlaying()
{
	if (playerScript->isPlayerDead)
	{
		loseWindow->SetActive(true);
		ChangeGameState(GameState::DEAD);
	} 
	else if (winGO->BboxIntersects(playerGO))
	{
		winWindow->SetActive(true);
		ChangeGameState(GameState::WIN);
	}
}

void GameLoop::ManageWin()
{
	loading++;

	if (loading == 300)
	{
		ChangeGameState(GameState::CREDITS);
	}
}

void GameLoop::ManagePaused()
{
}

void GameLoop::ManageOptions()
{
	VolumeManagement();
	SoundManagement();
	if (backOptionButton->IsPressed())
	{
		options->SetActive(false);
		EnableMenuButtons(true);
		ChangeGameState(GameState::MENU);
	}
}

void GameLoop::ManageCredits()
{
	ComponentScript* script = nullptr;

	if(!runningCredits)
	{
		hudGO->SetActive(false);
		winWindow->SetActive(false);
		creditsGO->SetActive(true);
		runningCredits = true;


		GameObject* imageCredits = App->scene->FindGameObjectByName(creditsGO, "ImageCredits");
		script = (ComponentScript*)imageCredits->GetComponent(ComponentType::Script);
		script->Enable(true);
		script->ScriptStart();
	}
	else
	{
		if (script != nullptr)
		{
			script->ScriptUpdate();
		}
	}
}

void GameLoop::ManageControls()
{
	if (backControlsButton->IsPressed())
	{
		controls->SetActive(false);
		EnableMenuButtons(true);
		ChangeGameState(GameState::MENU);
	}
}

void GameLoop::ManageQuit()
{
}

void GameLoop::EnableMenuButtons(bool enable)
{
	for (int i = 0; i < menuButtons.size(); i++)
	{
		menuButtons[i]->Enable(enable);
	}
}

void GameLoop::VolumeManagement()
{
	if (((ComponentButton*)volumeButtons[0])->IsPressed()) //Decrease
	{
		volume = MAX(minVolume, volume - 1);
		volumeText->text = std::to_string(volume);
	}
	else if (((ComponentButton*)volumeButtons[1])->IsPressed()) //Increase
	{
		volume = MIN(maxVolume, volume + 1);
		volumeText->text = std::to_string(volume);
	}
}

void GameLoop::SoundManagement()
{
	if (((ComponentButton*)soundButtons[0])->IsPressed()) //Decrease
	{
		sound = MAX(minSound, sound - 1);
		soundText->text = std::to_string(sound);
	}
	else if (((ComponentButton*)soundButtons[1])->IsPressed()) //Increase
	{
		sound = MIN(maxSound, sound + 1);
		soundText->text = std::to_string(sound);
	}
}

void GameLoop::ChangeGameState(GameState newState) //Set initial conditions for each state here if required
{
	switch (newState) 
	{
		case GameState::MENU:
		{		
			break;
		}
		case GameState::INTRO:
		{	
			break;
		}
		case GameState::PLAYING:
		{	
			break;
		}
		case GameState::DEAD:
		{
			break;
		}
		case GameState::WIN:
		{
			break;
		}
		case GameState::PAUSED:
		{	
			break;
		}
		case GameState::CREDITS:
		{	
			break;
		}
		case GameState::QUIT:
		{	
			break;
		}
	}

	gameState = newState;
}
