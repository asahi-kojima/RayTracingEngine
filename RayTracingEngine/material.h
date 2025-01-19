#pragma once
#include "ray.h"


struct HitRecord;

class Material 
{
public:
	/// <summary>
	/// 基本的にtrueが返る。
	/// 裏側からの入射を許さない場合は入射方向によってfalseの場合も起きる
	/// </summary>
	/// <returns></returns>
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, vec3& attenuation, Ray& ray_scattered) = 0;
};


class Metal : public Material
{
public:
	Metal(const vec3& albedo, f32 fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

private:
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, vec3& attenuation, Ray& ray_scattered) override;


	vec3 albedo;
	f32 fuzz;
};