#pragma once
#include <math.h>
#include <cmath>
#include "ray.h"
#include "vector.h"

class Camera
{
public:
	//Camera()
	//{
	//	origin = vec3(0.0, 0.0, 0.0);
	//	lowerLeftCorner = vec3(-2.0f, -1.0f, -1.0f);
	//	horizontal = vec3(4.0f, 0.0f, 0.0f);
	//	vertical = vec3(0.0f, 2.0f, 0.0f);
	//}

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
		//vec3 rd = lensRadius * randominUnitDisk();
		//vec3 offset = u * rd.x() + v * rd.y();
		//vec3 rayOrigin = origin + offset;
		vec3 rayOrigin = origin;
		return Ray(rayOrigin, lowerLeftCorner + s * horizontal + t * vertical - rayOrigin);
	}


	vec3 origin;
	vec3 lowerLeftCorner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	f32 lensRadius;
};