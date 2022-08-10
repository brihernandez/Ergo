#include "GameCamera.h"

#include <raymath.h>

#include "Ship.h"
#include "MathUtils.h"

GameCamera::GameCamera(bool isPerspective, float fieldOfView)
{
	Camera = Camera3D();
	Camera.position = Vector3{ 0, 10, -10 };
	Camera.target = Vector3{ 0, 0, 0 };
	Camera.up = Vector3{ 0, 1, 0 };

	Camera.fovy = fieldOfView;
	Camera.projection = isPerspective
		? CameraProjection::CAMERA_PERSPECTIVE
		: CameraProjection::CAMERA_ORTHOGRAPHIC;

	SmoothPosition = Vector3Zero();
	SmoothTarget = Vector3Zero();
	SmoothUp = Vector3Zero();
}

void GameCamera::FollowShip(const Ship& ship, float deltaTime)
{
	Vector3 position = ship.TransformPoint({ 0, 1, -3 });
	Vector3 shipForwards = Vector3Scale(ship.GetForward(), 25);
	Vector3 target = Vector3Add(ship.Position, shipForwards);
	Vector3 up = ship.GetUp();

	MoveTo(position, target, up, deltaTime);
}

void GameCamera::MoveTo(Vector3 position, Vector3 target, Vector3 up, float deltaTime)
{
	Camera.position = SmoothDamp(
		Camera.position, position,
		10, deltaTime);

	Camera.target = SmoothDamp(
		Camera.target, target,
		5, deltaTime);

	Camera.up = SmoothDamp(
		Camera.up, up,
		5, deltaTime);
}

void GameCamera::SetPosition(Vector3 position, Vector3 target, Vector3 up)
{
	Camera.position = position;
	Camera.target = target;
	Camera.up = up;

	SmoothPosition = position;
	SmoothTarget = target;
	SmoothUp = up;
}

Vector3 GameCamera::GetPosition() const
{
	return Camera.position;
}

void GameCamera::Begin3DDrawing() const
{
	BeginMode3D(Camera);
}

void GameCamera::EndDrawing() const
{
	EndMode3D();
}
