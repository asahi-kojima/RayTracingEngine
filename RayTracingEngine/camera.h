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
		focusDistance = focusDist;

		f32 theta = vfov * M_PI / 180.0f;
		f32 halfHeight = tan(theta / 2);
		f32 halfWidth = aspect * halfHeight;

		mEyeOrigin = lookFrom;
		mCameraZ = normalize(lookFrom - lookAt);//z
		mCameraX = normalize(cross(vUp, mCameraZ));//x
		mCameraY = cross(mCameraZ, mCameraX);//y

		mScreenOrigin = mEyeOrigin - focusDist * mCameraZ - focusDist * halfWidth * mCameraX - focusDist * halfHeight * mCameraY;
		horizontal = focusDist * 2 * halfWidth * mCameraX;
		vertical = focusDist * 2 * halfHeight * mCameraY;
	}


	Ray getRay(f32 s, f32 t)
	{
		vec3 offset;
		if (lensRadius > 1e-5)
		{
			vec3 rd = lensRadius * random_in_unit_disk();
			offset = mCameraX * rd[0] + mCameraY * rd[1];
		}
		else
		{
			offset = vec3::zero();
		}
		vec3 rayOrigin = mEyeOrigin + offset;
		return Ray(rayOrigin, normalize(mScreenOrigin + s * horizontal + t * vertical - rayOrigin));
	}

	vec3 getEyeOrigin() const { return mEyeOrigin; }
	vec3 getScreenOrigin() const { return mScreenOrigin; }
	vec3 getCameraX() const { return mCameraX; }
	vec3 getCameraY() const { return mCameraY; }
	vec3 getCameraZ() const { return mCameraZ; }
	f32 getHorizontalScreenScale() const { return dot(horizontal, mCameraX); }
	f32 getVerticalScreenScale() const { return dot(vertical, mCameraY); }
	f32 getFocusDistance() const { return focusDistance; }

private:

	vec3 mEyeOrigin;
	vec3 mScreenOrigin;//ç∂â∫Ç™å¥ì_
	vec3 horizontal;
	vec3 vertical;
	vec3 mCameraX, mCameraY, mCameraZ;
	f32 lensRadius;
	f32 focusDistance;


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