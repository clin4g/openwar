// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/type_ptr.hpp>
#include "geometry.h"



plane::plane(glm::vec3 n, glm::vec3 p) :
normal(n),
d(-glm::dot(n, p))
{
}


plane::plane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) :
normal(glm::normalize(glm::cross(v2 - v1, v3 - v1))),
d(-glm::dot(normal, v1))
{
}


glm::vec3 plane::project(const glm::vec3& v) const
{
	glm::mat3x3 m;
	glm::vec3::value_type* mm = glm::value_ptr(m);
	mm[0] = 1.0f - normal.x * normal.x;
	mm[1] = -normal.x * normal.y;
	mm[2] = -normal.x * normal.z;
	mm[3] = -normal.y * normal.x;
	mm[4] = 1.0f - normal.y * normal.y;
	mm[5] = -normal.y * normal.z;
	mm[6] = -normal.z * normal.x;
	mm[7] = -normal.z * normal.y;
	mm[8] = 1.0f - normal.z * normal.z;
	return m * v;
}


static bool almost_zero(float value)
{
	static const float epsilon = 10 * std::numeric_limits<float>::epsilon();
	return fabsf(value) < epsilon;
}


float distance(glm::vec3 v, plane p)
{
	return glm::dot(p.normal, v) + p.d;
}


const float* intersect(ray r, plane p)
{
	static float result;

	float denom = glm::dot(p.normal, r.direction);
	if (almost_zero(denom))
		return nullptr;

	float nom = glm::dot(p.normal, r.origin) + p.d;
	result = -(nom / denom);
	return &result;
}


const float* intersect(ray r, bounds3f b)
{
	static float result;

	if (b.is_empty())
		return nullptr;

	if (b.contains(r.origin))
	{
		result = 0;
		return &result;
	}

	glm::vec3 e = glm::vec3(0.001, 0.001, 0.001);
	bounds3f b2(b.min - e, b.max + e);
	bool hit = false;

	if (r.origin.x <= b.min.x && r.direction.x > 0) // min x
	{
		float t = (b.min.x - r.origin.x) / r.direction.x;
		if (t >= 0)
		{
			glm::vec3 p = r.point(t);
			if (p.y >= b2.min.y && p.y <= b2.max.y &&
					p.z >= b2.min.z && p.z <= b2.max.z &&
					(!hit || t < result))
			{
				hit = true;
				result = t;
			}
		}
	}

	if (r.origin.x >= b.max.x && r.direction.x < 0) // max x
	{
		float t = (b.max.x - r.origin.x) / r.direction.x;
		if (t >= 0)
		{
			glm::vec3 p = r.point(t);
			if (p.y >= b2.min.y && p.y <= b2.max.y &&
					p.z >= b2.min.z && p.z <= b2.max.z &&
					(!hit || t < result))
			{
				hit = true;
				result = t;
			}
		}
	}

	if (r.origin.y <= b.min.y && r.direction.y > 0) // min y
	{
		float t = (b.min.y - r.origin.y) / r.direction.y;
		if (t >= 0)
		{
			glm::vec3 p = r.point(t);
			if (p.x >= b2.min.x && p.x <= b2.max.x &&
					p.z >= b2.min.z && p.z <= b2.max.z &&
					(!hit || t < result))
			{
				hit = true;
				result = t;
			}
		}
	}

	if (r.origin.y >= b.max.y && r.direction.y < 0) // max y
	{
		float t = (b.max.y - r.origin.y) / r.direction.y;
		if (t >= 0)
		{
			glm::vec3 p = r.point(t);
			if (p.x >= b2.min.x && p.x <= b2.max.x &&
					p.z >= b2.min.z && p.z <= b2.max.z &&
					(!hit || t < result))
			{
				hit = true;
				result = t;
			}
		}
	}

	if (r.origin.z <= b.min.z && r.direction.z > 0) // min z
	{
		float t = (b.min.z - r.origin.z) / r.direction.z;
		if (t >= 0)
		{
			glm::vec3 p = r.point(t);
			if (p.x >= b2.min.x && p.x <= b2.max.x &&
					p.y >= b2.min.y && p.y <= b2.max.y &&
					(!hit || t < result))
			{
				hit = true;
				result = t;
			}
		}
	}

	if (r.origin.z >= b.max.z && r.direction.z < 0) // max z
	{
		float t = (b.max.z - r.origin.z) / r.direction.z;
		if (t >= 0)
		{
			glm::vec3 p = r.point(t);
			if (p.x >= b2.min.x && p.x <= b2.max.x &&
					p.y >= b2.min.y && p.y <= b2.max.y &&
					(!hit || t < result))
			{
				hit = true;
				result = t;
			}
		}
	}

	return hit ? &result : nullptr;
}
