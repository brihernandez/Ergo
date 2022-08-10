#pragma once

#include <raylib.h>

class Ship;

class GameCamera
{
public:

	GameCamera(bool isPerspective, float fieldOfView);

	/// <summary>
	/// Automatically moves the camera to follow a target ship.
	/// </summary>
	void FollowShip(const Ship& ship, float deltaTime);

	/// <summary>
	/// Moves the camera to the given positions. Smoothing is automatically applied.
	/// </summary>
	void MoveTo(Vector3 position, Vector3 target, Vector3 up, float deltaTime);

	/// <summary>
	/// Immediately moves the camera to the given positions with no smoothing.
	/// </summary>
	void SetPosition(Vector3 position, Vector3 target, Vector3 up);

	/// <summary>
	/// Required to tell raylib that any further 3D calls will be made with this camera.
	/// Must be paired with EndDrawing().
	/// </summary>
	void Begin3DDrawing() const;

	/// <summary>
	/// Requires to tell raylib to stop 3D rendering with this camera.
	/// </summary>
	void EndDrawing() const;

	Vector3 GetPosition() const;

private:
	Camera3D Camera;

	Vector3 SmoothPosition;
	Vector3 SmoothTarget;
	Vector3 SmoothUp;
};
