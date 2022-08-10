#include "SpaceDust.h"

#include <raymath.h>
#include <rlgl.h>
#include <cmath>
#include <array>

inline float GetPrettyBadRandomFloat(float min, float max)
{
	return (float)GetRandomValue(min, max);

	auto value = static_cast<float>(GetRandomValue(min * 1000, max * 1000));
	value /= 1000.f;
	return value;
}

SpaceDust::SpaceDust(float size, int count)
{
	Points = std::vector<Vector3>();
	Points.reserve(count);
	Extent = size * .5f;

	for (int i = 0; i < count; ++i)
	{
		auto point = Vector3{
			GetPrettyBadRandomFloat(-Extent, Extent),
			GetPrettyBadRandomFloat(-Extent, Extent),
			GetPrettyBadRandomFloat(-Extent, Extent)
		};
		Points.push_back(point);

		auto color = Color{
			(unsigned char)GetRandomValue(192, 255),
			(unsigned char)GetRandomValue(192, 255),
			(unsigned char)GetRandomValue(192, 255),
			255
		};
		Colors.push_back(color);
	}
}

void SpaceDust::UpdateViewPosition(Vector3 viewPosition)
{
	float size = Extent * 2;
	for (auto& p : Points)
	{
		while (p.x > viewPosition.x + Extent)
			p.x -= size;
		while (p.x < viewPosition.x - Extent)
			p.x += size;

		while (p.y > viewPosition.y + Extent)
			p.y -= size;
		while (p.y < viewPosition.y - Extent)
			p.y += size;

		while (p.z > viewPosition.z + Extent)
			p.z -= size;
		while (p.z < viewPosition.z - Extent)
			p.z += size;
	}
}

void SpaceDust::Draw(Vector3 viewPosition, Vector3 velocity, bool drawDots) const
{
	BeginBlendMode(BlendMode::BLEND_ADDITIVE);

	for (int i = 0; i < Points.size(); ++i)
	{
		float distance = Vector3Distance(viewPosition, Points[i]);

		float farLerp = Clamp(Normalize(distance, Extent * .9f, Extent), 0, 1);
		unsigned char farAlpha = Lerp(255, 0, farLerp);

		const float cubeSize = 0.01f;

		if (drawDots)
		{
			DrawSphereWires(
				Points[i],
				cubeSize,
				2, 4,
				{ Colors[i].r, Colors[i].g, Colors[i].b, farAlpha });
		}

		DrawLine3D(
			Vector3Add(Points[i], Vector3Scale(velocity, 0.02f)),
			Points[i],
			{ Colors[i].r, Colors[i].g, Colors[i].b, farAlpha });
	}

	rlDrawRenderBatchActive();
	EndBlendMode();
}
