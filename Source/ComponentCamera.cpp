#include "ComponentCamera.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "Math/MathFunc.h"
#include "Geometry/AABB.h"

#define MAXFOV 120
#define MINFOV 40

ComponentCamera::ComponentCamera() : Component(nullptr, ComponentType::Camera)
{
}

ComponentCamera::ComponentCamera(GameObject * gameobject) : Component(gameobject, ComponentType::Camera)
{
}


ComponentCamera::~ComponentCamera()
{
}

ComponentCamera * ComponentCamera::Clone()
{
	return new ComponentCamera(*this);
}


void ComponentCamera::Move()
{
	float distance = movementSpeed * App->time->deltaTime;
	if (App->input->IsKeyPressed(SDL_SCANCODE_LSHIFT))
	{
		distance *= 2;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_Q))
	{
		cameraPos.y += distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_E))
	{
		cameraPos.y -= distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
	{
		cameraPos -= cameraFront * distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_W))
	{
		cameraPos += cameraFront * distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
	{
		cameraPos -= cameraFront.Cross(float3::unitY).Normalized() * distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		cameraPos += cameraFront.Cross(float3::unitY).Normalized() * distance;
	}
}
//TODO: Use mouse position + deltatime and not mouse motion
void ComponentCamera::Rotate()
{
	float deltaPitch = App->input->GetMouseMotion().y*rotationSpeed;
	pitch -= deltaPitch;
	if (pitch > 0)
	{
		pitch = MIN(89, pitch);
	}
	else
	{
		pitch = MAX(-89, pitch);
	}
	yaw += App->input->GetMouseMotion().x*rotationSpeed;
	ComputeEulerAngles();
}

void ComponentCamera::Zoom()
{
	float mouse_wheel = App->input->GetMouseWheel();
	if (mouse_wheel != 0)
	{
		App->renderer->frustum.verticalFov = mouse_wheel > 0 ?
			MAX(math::DegToRad(MINFOV), App->renderer->frustum.verticalFov - mouse_wheel * zoomSpeed) :
			MIN(math::DegToRad(MAXFOV), App->renderer->frustum.verticalFov - mouse_wheel * zoomSpeed);
		App->renderer->frustum.horizontalFov = 2.f * atanf(tanf(App->renderer->frustum.verticalFov * 0.5f) * ((float)App->window->width / (float)App->window->height));
	}
}

void ComponentCamera::Center()
{
	if (App->scene->selected == nullptr) return;

	AABB bbox = App->scene->selected->GetBoundingBox();
	float3 HalfSize = bbox.HalfSize();
	float distX = HalfSize.x / tanf(App->renderer->frustum.horizontalFov*0.5f);
	float distY = HalfSize.y / tanf(App->renderer->frustum.verticalFov*0.5f);
	float camDist = MAX(distX, distY) + HalfSize.z; //camera distance from model

	float3 center = bbox.FaceCenterPoint(5);
	cameraPos = center + float3(0, 0, camDist);

	cameraFront = float3(0, 0, -1);
	pitch = 0;
	yaw = -90;
}

void ComponentCamera::ComputeEulerAngles()
{// �Viva Euler, muerte al Quaternion!
	cameraFront.x = cos(math::DegToRad(yaw)) * cos(math::DegToRad(pitch));
	cameraFront.y = sin(math::DegToRad(pitch));
	cameraFront.z = sin(math::DegToRad(yaw)) *cos(math::DegToRad(pitch));
	cameraFront.Normalize();
}

void ComponentCamera::Orbit()
{
	if (App->scene->selected == nullptr) return;
	orbitX += App->input->GetMouseMotion().x*rotationSpeed;
	orbitY = MIN(89, orbitY + App->input->GetMouseMotion().y*rotationSpeed);

	AABB bbox = App->scene->selected->GetBoundingBox();
	radius = bbox.CenterPoint().Distance(cameraPos);

	cameraPos.x = cos(math::DegToRad(orbitX)) * cos(math::DegToRad(orbitY)) * radius;
	cameraPos.y = sin(math::DegToRad(orbitY)) * radius;;
	cameraPos.z = sin(math::DegToRad(orbitX)) *cos(math::DegToRad(orbitY)) * radius;
	cameraPos += bbox.CenterPoint();

	cameraFront = (bbox.CenterPoint() - cameraPos).Normalized();

	yaw = math::RadToDeg(atan2(cameraFront.z, cameraFront.x));
	pitch = math::RadToDeg(asin(cameraFront.y));
}

