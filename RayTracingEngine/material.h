#pragma once
#include "ray.h"
#include "color.h"


struct HitRecord;

class Material 
{
public:
	/// <summary>
	/// 基本的にtrueが返る。
	/// 裏側からの入射を許さない場合は入射方向によってfalseの場合も起きる
	/// </summary>
	/// <returns></returns>
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered) = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const Color& color) : albedo(color){}
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered) override;


private:
	Color albedo;
};



class Metal : public Material
{
public:
	Metal(const Color& albedo, f32 fuzz = 0.0f) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

private:
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered) override;


	Color albedo;
	f32 fuzz;
};



class Dielectric : public Material
{
public:
	Dielectric(float ref) : refIdx(ref) {}

private:
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered) override;

	static bool isRefract(const vec3& v, const vec3& n, float niOverNt, vec3& refracted);

	static f32 schlick(float cosine, float refIdx);


	f32 refIdx;
};

class Retroreflective : public Material
{
public:
	Retroreflective(const Color& albedo) : albedo(albedo) {}

private:
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered) override;

	Color albedo;
};

class SunLight : public Material
{
public:
	SunLight() {}

private:
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered) override;
};


class GravitationalField : public Material
{
public:
	GravitationalField(f32 gravityScale, vec3 center) :mGravityScale(gravityScale), mCenter(center){}

	virtual bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered) override;


private:
	f32 mGravityScale;
	constexpr static f32 G = 1.0f;
	vec3 mCenter;
};