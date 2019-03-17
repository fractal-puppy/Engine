#include "GameLoop.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

GameLoop_API GameLoop* CreateScript()
{
	GameLoop* instance = new GameLoop;
	return instance;
}

void GameLoop::Start()
{
	LOG("Started GameLoop script");
}

void GameLoop::Update()
{
	//Test loop
	/*
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		gameState = GameState::MENU;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		gameState = GameState::PLAYING;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		gameState = GameState::PAUSED;
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		gameState = GameState::CREDITS;
	}
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		gameState = GameState::QUIT;
	}
	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
	{
		gameState = GameState::INTRO;
	}
	*/
	//Test loop - End
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
		case GameState::PAUSED:
		{
			//Wait until pause is removed // No update player / enemies / no check for collisions
			ManagePaused();
			break;
		}
		case GameState::CREDITS:
		{
			//Show credits
			ManageCredits();
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

void GameLoop::ManageMenu()
{
}

void GameLoop::ManageIntro()
{
}

void GameLoop::ManagePlaying()
{
}

void GameLoop::ManagePaused()
{
}

void GameLoop::ManageCredits()
{
}

void GameLoop::ManageQuit()
{
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
