#pragma once
#include "vector.h"

class Ray
{
public:
	Ray() = default;
	Ray(const Ray& ray) : mA(ray.mA), mB(ray.mB){}
	Ray(const vec3& a, const vec3& b) : mA(a) , mB(b){}



	//vec3 origin() const { return mA; }
	const vec3& origin() const { return mA; }
	//vec3 direction() const { return mB; }
	const vec3& direction() const { return mB; }
	vec3 pointAt(const f32 t) const { return mA + mB * t; }
	
private:
	vec3 mA;
	vec3 mB;
};