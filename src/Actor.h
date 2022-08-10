#pragma once

#include <raylib.h>

class Actor
{
public:
	Actor();

	Vector3 Position;
	Vector3 Velocity;
	Quaternion Rotation;

	Vector3 GetForward() const;
	Vector3 GetBack() const;
	Vector3 GetRight() const;
	Vector3 GetLeft() const;
	Vector3 GetUp() const;
	Vector3 GetDown() const;

	Vector3 TransformPoint(Vector3 point) const;
	void RotateLocalEuler(Vector3 axis, float degrees);
};
