#pragma once

#include <raylib.h>
#include <vector>

class SpaceDust
{
public:
	SpaceDust(float size, int count);

	void UpdateViewPosition(Vector3 viewPosition);
	void Draw(Vector3 viewPosition, Vector3 velocity, bool drawDots) const;

private:
	std::vector<Vector3> Points;
	std::vector<Color> Colors;
	float Extent;
};
