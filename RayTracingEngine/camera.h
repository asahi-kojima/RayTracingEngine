#pragma once
#include <math.h>
#include <cmath>
#include "util.h"
#include "ray.h"
#include "vector.h"

class Camera
{
public:
	Camera(vec3 lookFrom, vec3 lookAt, vec3 vUp, f32 vfov, f32 aspect, f32 aperture = 0, f32 focusDist = 1)
	{
		lensRadius = aperture / 2;

		f32 theta = vfov * M_PI / 180.0f;
		f32 halfHeight = tan(theta / 2);
		f32 halfWidth = aspect * halfHeight;

		origin = lookFrom;
		w = normalize(lookFrom - lookAt);//z
		u = normalize(cross(vUp, w));//x
		v = cross(w, u);//y

		lowerLeftCorner = origin - focusDist * w - focusDist * halfWidth * u - focusDist * halfHeight * v;
		horizontal = focusDist * 2 * halfWidth * u;
		vertical = focusDist * 2 * halfHeight * v;
	}


	Ray getRay(f32 s, f32 t)
	{
		vec3 offset;
		if (lensRadius > 1e-5)
		{
			vec3 rd = lensRadius * random_in_unit_disk();
			offset = u * rd[0] + v * rd[1];
		}
		else
		{
			offset = vec3::zero();
		}
		vec3 rayOrigin = origin + offset;
		return Ray(rayOrigin, normalize(lowerLeftCorner + s * horizontal + t * vertical - rayOrigin));
	}


	vec3 origin;
	vec3 lowerLeftCorner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	f32 lensRadius;


private:
	/// <summary>
	/// ÉwÉãÉpÅ[ä÷êî
	/// </summary>
	/// <returns></returns>
	static vec3 random_in_unit_disk()
	{
		vec3 p;
		do
		{
			p = vec3(RandomGenerator::signed_uniform_real(), RandomGenerator::signed_uniform_real(), 0);
		} while (dot(p, p) >= 1.0f);

		return p;
	}
};