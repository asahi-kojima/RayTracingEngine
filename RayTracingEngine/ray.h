#pragma once
#include "vector.h"

class Ray
{
public:
	Ray() = default;
	Ray(const Ray& ray) : mOrigin(ray.mOrigin), mDirection(ray.mDirection){}
	Ray(const vec3& a, const vec3& b) : mOrigin(a) , mDirection(b){}



	//vec3 origin() const { return mA; }
	const vec3& origin() const { return mOrigin; }
	//vec3 direction() const { return mB; }
	const vec3& direction() const { return mDirection; }
	vec3 pointAt(const f32 t) const { return mOrigin + mDirection * t; }
	
private:
	vec3 mOrigin;
	vec3 mDirection;
};