#include "material.h"
#include "hittable.h"
#include "util.h"

//======================================================
// ランバート
//======================================================
bool Lambertian::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	const vec3 target = record.pos + record.normal + random_in_unit_sphere();
	ray_scattered.direction() = target - record.pos;
	ray_scattered.origin() = record.pos;
	attenuation = albedo;
	return true;
}

//======================================================
// 金属
//======================================================
bool Metal::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	//新たなレイをセット
	vec3 reflected_ray = reflect(ray_in.direction(), record.normal);

	//最後の0.1fについてはreflected_rayの長さが１だが、それと同じオーダーにすると境界で無限反射が起きるので、それを防ぐため。
	ray_scattered = Ray(record.pos, reflected_ray + fuzz * random_in_unit_sphere() * 0.1f);

	//表面色をセット
	attenuation = albedo;

	return (dot(ray_scattered.direction(), record.normal) > 0);
}


//======================================================
// 誘電体
//======================================================
bool Dielectric::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	attenuation = Color::White;

	vec3 outwardNormal;
	vec3 reflected = reflect(ray_in.direction(), record.normal);
	f32 niOverNt;
	vec3 refracted;
	f32 cosine;
	f32 reflectProb;

	// 内部から出てこようとしている時
	if (dot(ray_in.direction(), record.normal) > 0)
	{
		outwardNormal = record.normal;
		niOverNt = refIdx;
		cosine = dot(ray_in.direction(), record.normal) / ray_in.direction().length();
	}
	// 外部から飛んできている時
	else
	{
		outwardNormal = -record.normal;
		niOverNt = 1.0 / refIdx;
		cosine = -dot(ray_in.direction(), record.normal) / ray_in.direction().length();
	}

	if (isRefract(ray_in.direction(), outwardNormal, niOverNt, refracted))
	{
		reflectProb = schlick(cosine, refIdx);
		if (RandomGenerator::uniform_real() < reflectProb)
		{
			ray_scattered = Ray(record.pos, reflected);
		}
		else
		{
			ray_scattered = Ray(record.pos, refracted);
		}
	}
	else
	{
		ray_scattered = Ray(record.pos, reflected);
	}

	return true;

}

bool Dielectric::isRefract(const vec3& v, const vec3& n, f32 niOverNt, vec3& refracted)
{
	vec3 uv = normalize(v);
	f32 dt = dot(uv, n);

	// スネル則を解いてる。Dはcos^2Thetaに相当し、正なら解がある。
	f32 D = 1.0 - niOverNt * niOverNt * (1 - dt * dt);

	// 解がある場合。屈折光を算出する。
	if (D > 0)
	{
		refracted = niOverNt * (uv - n * dt) + n * sqrt(D);
		return true;
	}

	// 全反射の場合
	return false;
}

f32 Dielectric::schlick(f32 cosine, f32 refIdx)
{
	f32 r0 = (1 - refIdx) / (1 + refIdx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}


//======================================================
// 再帰性反射素材
//======================================================
bool Retroreflective::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	ray_scattered.direction() = -ray_in.direction();
	ray_scattered.origin() = record.pos;

	attenuation = albedo;

	return false;
}


//======================================================
// 光源
//======================================================

bool SunLight::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	attenuation = Color::White;
	return false;
}



//======================================================
// 重力場
//======================================================
bool GravitationalField::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	const f32 m = 1.0f;
	const f32 v = 10.0f;

	const vec3 OC = mCenter - ray_in.origin();
	const vec3 D = ray_in.direction();
	const f32 ray_center_dist = (D * (dot(OC, D) / D.lengthSquared()) - OC).length();

	const f32 E = 0.5f * m * v * v - G * mGravityScale * m / (record.pos - mCenter).length();
	const f32 L = m * v * ray_center_dist;
	const f32 R0 = L * L / G / mGravityScale;
	const f32 typical_E = mGravityScale * mGravityScale * G * G / (2 * L * L);//典型的なエネルギースケールを意味しており、実際のエネルギーとは別

	//離心率
	const f32 e = sqrtf(1.0f + E / typical_E);
	if (e < 1.0f)
	{
		attenuation = Color::Black;
		return false;
	}

	attenuation = Color::White;

	{
		const vec3 CP = record.pos - mCenter;
		const f32 R = CP.length();
		const vec3 ux = -normalize(D);
		const vec3 uz = normalize(cross(ux,CP));
		const vec3 uy = cross(uz, ux);
		const f32 h = abs(dot(ux, CP));
		const f32 theta = asinf(h / R);

		const f32 phi = -( acosf(((R0 / OC.length()) - 1) / e) - theta);
		const f32 phi2 = 2 * phi;

		const f32 x = cos(phi2) * R * cos(theta) + sin(phi2) * R * sin(theta);
		const f32 y = sin(phi2) * R * cos(theta) - cos(phi2) * R * sin(theta);
		const vec3 outgoing_pos = x * ux + y * uy + mCenter;
		const vec3 outgoing_dir = cos(phi2) * ux + sin(phi2) * uy;

		ray_scattered.direction() = outgoing_dir;
		ray_scattered.origin() = outgoing_pos;
	}
	return true;
}


