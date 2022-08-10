#pragma once

#include "Actor.h"

struct TrailRung
{
	Vector3 LeftPoint;
	Vector3 RightPoint;
	float TimeToLive;
};

class Ship : public Actor
{
public:
	float InputForward = 0;
	float InputLeft = 0;
	float InputUp = 0;

	float InputPitchDown = 0;
	float InputRollRight = 0;
	float InputYawLeft = 0;

	float MaxSpeed = 20;
	float ThrottleResponse = 10;
	float TurnRate = 180;
	float TurnResponse = 10;

	float Length = 1.0f;
	float Width = 1.0f;

	Color TrailColor = DARKGREEN;

	Ship(const char* modelPath, const char* texturePath, Color color);
	~Ship();

	void Update(float deltaTime);
	void Draw(bool showDebugAxes) const;
	void DrawTrail() const;

private:
	Model Model = {};
	Color ShipColor = {};

	static const int RungCount = 16;
	TrailRung Rungs[RungCount];

	float SmoothForward = 0;
	float SmoothLeft = 0;
	float SmoothUp = 0;

	float SmoothPitchDown = 0;
	float SmoothRollRight = 0;
	float SmoothYawLeft = 0;

	float VisualBank = 0;

	void EmitTrailRung();
	Vector3 LastRungPosition = { 0, 0, 0 };
	int RungIndex = 0;
};

class Crosshair
{
public:
	Crosshair(const char* modelPath);
	~Crosshair();

	void PositionCrosshairOnShip(const Ship& ship, float distance);
	void DrawCrosshair() const;

private:
	Model Model = {};
};
