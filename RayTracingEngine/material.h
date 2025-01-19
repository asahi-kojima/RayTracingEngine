#pragma once
#include "ray.h"


struct HitRecord;

class Material 
{
public:
	/// <summary>
	/// ��{�I��true���Ԃ�B
	/// ��������̓��˂������Ȃ��ꍇ�͓��˕����ɂ����false�̏ꍇ���N����
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