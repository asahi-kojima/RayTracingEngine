#pragma once
#include <vector>
#include <memory>
#include "vector.h"
#include "hittable.h"


class AABB : public Hittable
{
public:
	AABB() = default;
	AABB(vec3 minPos, vec3 maxPos)
		: minPos(minPos), maxPos(maxPos) {}

	const vec3& getMinPos() const { return minPos; }
	const vec3& getMaxPos() const { return maxPos; }

	inline static AABB wraping(AABB lhs, AABB rhs)
	{
		vec3 minPos;
		{
			minPos.setX(fminf(lhs.minPos.getX(), rhs.minPos.getX()));
			minPos.setY(fminf(lhs.minPos.getY(), rhs.minPos.getY()));
			minPos.setZ(fminf(lhs.minPos.getZ(), rhs.minPos.getZ()));
		}
		vec3 maxPos;
		{
			maxPos.setX(fmaxf(lhs.maxPos.getX(), rhs.maxPos.getX()));
			maxPos.setY(fmaxf(lhs.maxPos.getY(), rhs.maxPos.getY()));
			maxPos.setZ(fmaxf(lhs.maxPos.getZ(), rhs.maxPos.getZ()));
		}
		return AABB(minPos, maxPos);
	}

private:
	virtual bool hit(const Ray& r, const f32 t_min, const f32 t_max, HitRecord& record) override;
	virtual AABB calcAABB() override { return *this; }


	vec3 minPos;
	vec3 maxPos;
};

bool isIntersecting(const AABB& aabb, const Ray& ray, f32 t_min, f32 t_max);




class Sphere : public Hittable
{
public:
	Sphere() = default;
	Sphere(const vec3& center, const f32 radius, std::shared_ptr<Material>&& material)
		: center(center), radius(radius), material(std::move(material)) {}


private:
	bool hit(const Ray& r, const f32 t_min, const f32 t_max, HitRecord& record) override;
	AABB calcAABB() override;


	vec3 center;
	f32 radius;
	std::shared_ptr<Material> material;
};


//class Mesh : public Hittable
//{
//public:
//	Mesh(std::vector<vec3>&& vertexList, std::vector<f32>&& indexList);
//	Mesh(const std::vector<vec3>& vertexList, const std::vector<f32>& indexList);
//
//private:
//	std::vector<vec3> mVertexList;
//	std::vector<f32> mIndexList;
//
//	bool hit(const Ray& r, const f32 t_min, const f32 t_max, HitRecord& record) override;
//	AABB calcAABB() override;
//};