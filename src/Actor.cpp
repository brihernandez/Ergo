#include "Actor.h"

#include <raymath.h>

Actor::Actor()
{
	Position = Vector3Zero();
	Velocity = Vector3Zero();
	Rotation = QuaternionIdentity();
}

Vector3 Actor::GetForward() const
{
	return Vector3RotateByQuaternion(
		Vector3{ 0, 0, 1 },
		Rotation);
}

Vector3 Actor::GetBack() const
{
	return Vector3RotateByQuaternion(
		Vector3{ 0, 0, -1 },
		Rotation);
}

Vector3 Actor::GetRight() const
{
	return Vector3RotateByQuaternion(
		Vector3{ -1, 0, 0 },
		Rotation);
}

Vector3 Actor::GetLeft() const
{
	return Vector3RotateByQuaternion(
		Vector3{ 1, 0, 0 },
		Rotation);
}

Vector3 Actor::GetUp() const
{
	return Vector3RotateByQuaternion(
		Vector3{ 0, 1, 0 },
		Rotation);
}

Vector3 Actor::GetDown() const
{
	return Vector3RotateByQuaternion(
		Vector3{ 0, -1, 0 },
		Rotation);
}

Vector3 Actor::TransformPoint(Vector3 point) const
{
	auto mPos = MatrixTranslate(Position.x, Position.y, Position.z);
	auto mRot = QuaternionToMatrix(Rotation);
	auto matrix = MatrixMultiply(mRot, mPos);
	return Vector3Transform(point, matrix);
}

void Actor::RotateLocalEuler(Vector3 axis, float degrees)
{
	auto radians = degrees * DEG2RAD;
	Rotation = QuaternionMultiply(
		Rotation,
		QuaternionFromAxisAngle(axis, radians));
}
