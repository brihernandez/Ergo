#include "Ship.h"

#include "MathUtils.h"

#include <vector>
#include <rlgl.h>

static const float RungDistance = 2.0f;
static const float RungTimeToLive = 2.0f;

Ship::Ship(const char* modelPath, const char* texturePath, Color color)
{
	Texture2D texture = LoadTexture(texturePath);
	texture.mipmaps = 0;
	SetTextureFilter(texture, TEXTURE_FILTER_POINT);

	Model = LoadModel(modelPath);
	Model.materials[0].maps[MaterialMapIndex::MATERIAL_MAP_ALBEDO].texture = texture;

	Rotation = QuaternionFromEuler(1, 2, 0);

	ShipColor = color;

	LastRungPosition = Position;
}

Ship::~Ship()
{
	UnloadModel(Model);
}

void Ship::Update(float deltaTime)
{
	// Give the ship some momentum when accelerating.
	SmoothForward = SmoothDamp(SmoothForward, InputForward, ThrottleResponse, deltaTime);
	SmoothLeft = SmoothDamp(SmoothLeft, InputLeft, ThrottleResponse, deltaTime);
	SmoothUp = SmoothDamp(SmoothUp, InputUp, ThrottleResponse, deltaTime);

	// Flying in reverse should be slower.
	auto forwardSpeedMultipilier = SmoothForward > 0.0f ? 1.0f : 0.33f;

	auto targetVelocity = Vector3Zero();
	targetVelocity = Vector3Add(
		targetVelocity,
		Vector3Scale(GetForward(), MaxSpeed * forwardSpeedMultipilier * SmoothForward));
	targetVelocity = Vector3Add(
		targetVelocity,
		Vector3Scale(GetUp(), MaxSpeed * .5f * SmoothUp));
	targetVelocity = Vector3Add(
		targetVelocity,
		Vector3Scale(GetLeft(), MaxSpeed * .5f * SmoothLeft));

	Velocity = SmoothDamp(Velocity, targetVelocity, 2.5, deltaTime);
	Position = Vector3Add(Position, Vector3Scale(Velocity, deltaTime));

	// Give the ship some inertia when turning. These are the pilot controlled rotations.
	SmoothPitchDown = SmoothDamp(SmoothPitchDown, InputPitchDown, TurnResponse, deltaTime);
	SmoothRollRight = SmoothDamp(SmoothRollRight, InputRollRight, TurnResponse, deltaTime);
	SmoothYawLeft = SmoothDamp(SmoothYawLeft, InputYawLeft, TurnResponse, deltaTime);

	RotateLocalEuler( { 0, 0, 1 }, SmoothRollRight * TurnRate * deltaTime);
	RotateLocalEuler({ 1, 0, 0 }, SmoothPitchDown * TurnRate * deltaTime);
	RotateLocalEuler({ 0, 1, 0 }, SmoothYawLeft * TurnRate * deltaTime);

	//// Auto-roll from yaw
	//// Movement like a 3D space sim. This only feels good if there's no horizon auto-align.
	//RotateLocalEuler({ 0, 0, -1 }, SmoothYawLeft * TurnRate * .5f * deltaTime);

	// Auto-roll to align to horizon
	if (fabs(GetForward().y) < 0.8)
	{
		float autoSteerInput = GetRight().y;
		RotateLocalEuler({ 0, 0, 1 }, autoSteerInput * TurnRate * .5f * deltaTime);
	}

	// When yawing and strafing, there's some bank added to the model for visual flavor.
	float targetVisualBank = (-30 * DEG2RAD * SmoothYawLeft) + (-15 * DEG2RAD * SmoothLeft);
	VisualBank = SmoothDamp(VisualBank, targetVisualBank, 10, deltaTime);
	Quaternion visualRotation = QuaternionMultiply(
		Rotation, QuaternionFromAxisAngle({ 0, 0, 1 }, VisualBank));

	// Sync up the raylib representation of the model with the ship's position so that processing
	// doesn't have to happen at the render stage.
	auto transform = MatrixTranslate(Position.x, Position.y, Position.z);
	transform = MatrixMultiply(QuaternionToMatrix(visualRotation), transform);
	Model.transform = transform;

	// Rungs
	if (Vector3Distance(Position, LastRungPosition) > RungDistance)
	{
		EmitTrailRung();
		LastRungPosition = Position;
	}

	for (int i = 0; i < RungCount; ++i)
		Rungs[i].TimeToLive -= deltaTime;
}

void Ship::EmitTrailRung()
{
	Rungs[RungIndex].TimeToLive = RungTimeToLive;
	float halfWidth = Width / 2.f;
	float halfLength = Length / 2.f;
	Rungs[RungIndex].LeftPoint = TransformPoint({ -halfWidth, 0.0f, -halfLength });
	Rungs[RungIndex].RightPoint = TransformPoint({ halfWidth, 0.0f, -halfLength });
	RungIndex = (RungIndex + 1) % RungCount;
}

void Ship::Draw(bool showDebugAxes) const
{
	DrawModel(Model, Vector3Zero(), 1, ShipColor);

	if (showDebugAxes)
	{
		BeginBlendMode(BlendMode::BLEND_ADDITIVE);
		DrawLine3D(Position, Vector3Add(Position, GetForward()), { 0, 0, 255, 255 });
		DrawLine3D(Position, Vector3Add(Position, GetLeft()), { 255, 0, 0, 255 });
		DrawLine3D(Position, Vector3Add(Position, GetUp()), { 0, 255, 0, 255 });
		EndBlendMode();
	}
}

void Ship::DrawTrail() const
{
	BeginBlendMode(BlendMode::BLEND_ADDITIVE);
	rlDisableDepthMask();

	for (int i = 0; i < RungCount; ++i)
	{
		if (Rungs[i].TimeToLive <= 0)
			continue;

		auto& thisRung = Rungs[i % RungCount];

		Color color = TrailColor;
		color.a = 255 * thisRung.TimeToLive / RungTimeToLive;
		Color fill = color;
		fill.a = color.a / 4;

		DrawLine3D(thisRung.LeftPoint, thisRung.RightPoint, color);

		auto& nextRung = Rungs[(i + 1) % RungCount];
		if (nextRung.TimeToLive > 0 && thisRung.TimeToLive < nextRung.TimeToLive)
		{
			DrawLine3D(nextRung.LeftPoint, thisRung.LeftPoint, color);
			DrawLine3D(nextRung.RightPoint, thisRung.RightPoint, color);

			DrawTriangle3D(thisRung.LeftPoint, thisRung.RightPoint, nextRung.LeftPoint, fill);
			DrawTriangle3D(nextRung.LeftPoint, thisRung.RightPoint, nextRung.RightPoint, fill);

			DrawTriangle3D(nextRung.LeftPoint, thisRung.RightPoint, thisRung.LeftPoint, fill);
			DrawTriangle3D(nextRung.RightPoint, thisRung.RightPoint, nextRung.LeftPoint, fill);
		}
	}

	rlDrawRenderBatchActive();
	rlEnableDepthMask();
	EndBlendMode();
}

Crosshair::Crosshair(const char* modelPath)
{
	Model = LoadModel(modelPath);
}

Crosshair::~Crosshair()
{
	UnloadModel(Model);
}

void Crosshair::PositionCrosshairOnShip(const Ship& ship, float distance)
{
	auto crosshairPos = Vector3Add(Vector3Scale(ship.GetForward(), distance), ship.Position);
	auto crosshairTransform = MatrixTranslate(crosshairPos.x, crosshairPos.y, crosshairPos.z);
	crosshairTransform = MatrixMultiply(QuaternionToMatrix(ship.Rotation), crosshairTransform);
	Model.transform = crosshairTransform;
}

void Crosshair::DrawCrosshair() const
{
	BeginBlendMode(BlendMode::BLEND_ADDITIVE);
	rlDisableDepthTest();

	DrawModel(Model, Vector3Zero(), 1, DARKGREEN);
	//DrawModelWires(Model, Vector3Zero(), 1, DARKGREEN);

	rlEnableDepthTest();
	EndBlendMode();
}
