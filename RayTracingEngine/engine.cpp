#include <cmath>
#include <iostream>
#include <random>
#include <omp.h>
#include <map>
#include <thread>
#include <mutex>
#include "engine.h"
//============================================
/*
開発メモ
・もし巨大なオブジェクトに変な模様が付いたら、
	if (mRootNode->hit(ray_in, 0.01f, 1e+10, record))
	ここに問題あり。
*/
//============================================
RayTracingEngine::RayTracingEngine(const std::shared_ptr<Camera>& camera, const std::shared_ptr<RenderTarget>& renderTarget)
	: mCamera(camera)
	, mRenderTarget(renderTarget)
	, mScreenResolutionHeight(renderTarget->getResolutionHeight())
	, mScreenResolutionWidth(renderTarget->getResolutionWidth())
{
#if REALTIME_GRAPHICAL_UDP_DEBUG
	//このデバッグがONの場合、アプリケーションがUDP通信でコンタクトを取ってくるまで、
	//ブロッキングされます。
	mUDPServer.boot();
#endif
}


void RayTracingEngine::setObjects(std::vector<std::shared_ptr<Hittable> >&& world)
{
	mRootNode = std::make_unique<Node>(std::move(world));
}


void RayTracingEngine::render(const u32 sampleSize, const u32 depth)
{
	std::cout << "Ray Tracing Engine : Rendering Start\n";

	if (!mRootNode)
	{
		assert(0);
	}


	const f32 inv_coeff_width = 1.0f / static_cast<f32>(mScreenResolutionWidth - 1);
	const f32 inv_coeff_heigth = 1.0f / static_cast<f32>(mScreenResolutionHeight - 1);
#if !ONLY_SINGLE_THREAD_DEBUG
#pragma omp parallel for
#endif
	for (s32 i = 0; i < mScreenResolutionWidth; i++)
	{
		for (u32 j = 0; j < mScreenResolutionHeight; j++)
		{
			vec3 resultColor = vec3::zero();
			for (u32 s = 0; s < sampleSize; s++)
			{
				f32 u = static_cast<f32>(i + RandomGenerator::signed_uniform_real() * 0.1f) * inv_coeff_width;
				f32 v = static_cast<f32>(j + RandomGenerator::signed_uniform_real() * 0.1f) * inv_coeff_heigth;
				Ray ray = mCamera->getRay(u, v);

				SecondaryInfoByRay additinalRayInfo;
				resultColor += color(ray, depth, additinalRayInfo);
			}
			resultColor /= sampleSize;
			mRenderTarget->setColor(i, j, resultColor);

#if REALTIME_GRAPHICAL_UDP_DEBUG
			//計算結果をUDP通信でアプリケーションに送信する。
			mUDPServer.send(static_cast<f32>(i) / mScreenResolutionWidth, static_cast<f32>(j) / mScreenResolutionHeight, vec3(resultColor[0], resultColor[1], resultColor[2]));
#endif
		}
	}
}



void RayTracingEngine::saveRenderResult(const std::string& path) const
{
	std::cout << "Save Render Result in Path[" << path << "]" << std::endl;
	mRenderTarget->saveRenderResult(path);
}


vec3 RayTracingEngine::color(const Ray& ray_in, s32 depth)
{
	//反射回数が限界深度に達したら、黒色で早期リターン
	if (depth == 0)
	{
		return vec3::black();
	}

	HitRecord record;
	if (mRootNode->hit(ray_in, 0.01f, 1e+10, record))
	{
		Ray ray_scatterd;
		vec3 attenuation;
		if (record.material->scatter(ray_in, record, attenuation, ray_scatterd))
		{
			vec3 colorResult = color(ray_scatterd, depth - 1);
			return attenuation * colorResult;
		}
		else
		{
			return attenuation;
		}
	}
	else
	{
		vec3 unit_direction = normalize(ray_in.direction());
		f32 t = 0.5f * (unit_direction.getY() + 1.0f);
		return vec3(0.4f, 0.4f, 1.0f) * (1.0f - t) + vec3(0.5f, 0.7f, 1.0f) * t;
	}
}

vec3 RayTracingEngine::color(const Ray& ray_in, s32 depth, SecondaryInfoByRay& additinalRayInfo)
{
	additinalRayInfo.depth = depth;
	//反射回数が限界深度に達したら、黒色で早期リターン
	if (depth == 0)
	{
		return vec3::black();
	}

	HitRecord record;
	if (mRootNode->hit(ray_in, 0.01f, 1e+10, record))
	{
		Ray ray_scatterd;
		vec3 attenuation;
		if (record.material->scatter(ray_in, record, attenuation, ray_scatterd))
		{
			vec3 colorResult = color(ray_scatterd, depth - 1, additinalRayInfo);
			return attenuation * colorResult;
		}
		else
		{
			return attenuation;
		}
	}
	else
	{
		vec3 unit_direction = normalize(ray_in.direction());
		f32 t = 0.5f * (unit_direction.getY() + 1.0f);
		return vec3(0.4f, 0.4f, 1.0f) * (1.0f - t) + vec3(0.5f, 0.7f, 1.0f) * t;
	}
}


void RayTracingEngine::drawTrajectory(u32 i, u32 j)
{
	auto calc_coeff = [](const vec3& O, const vec3& D, f32 t, const Camera& camera, f32& alpha, f32& beta)
		{
			const vec3& Os = camera.getScreenOrigin();
			const vec3& I = camera.getEyeOrigin();
			const vec3& Nx = camera.getCameraX();
			const vec3& Ny = camera.getCameraY();
			const vec3& Nz = camera.getCameraZ();

			vec3 Pt = O + t * D;
			f32 s = dot(Os - I, Nz) / dot(Pt - I, Nz);
			if (!std::isfinite(s))
			{
				t = -0.001f;
				Pt = O + t * D;
				s = dot(Os - I, Nz) / dot(Pt - I, Nz);
			}
			const vec3 Pintersect = I + (Pt - I) * s;

			const f32 Sx = camera.getHorizontalScreenScale();
			const f32 Sy = camera.getVerticalScreenScale();

			alpha = dot(Pintersect - Os, Nx) / Sx;
			beta = dot(Pintersect - Os, Ny) / Sy;
		};

	const f32 u = static_cast<f32>(i) / static_cast<f32>(mScreenResolutionWidth - 1);
	const f32 v = static_cast<f32>(j) / static_cast<f32>(mScreenResolutionHeight - 1);
	Ray ray = mCamera->getRay(u, v);

	const s32 MaxDepth = 20;
	s32 depth = MaxDepth;

	bool isNextRayExist = true;
	HitRecord record;


	while (depth >= 0 && isNextRayExist)
	{
		bool isHit = mRootNode->hit(ray, 0.01f, 1e+10, record);

		f32 t = (isHit ? record.t : 1000.0f);

		const vec3 O = ray.origin();
		const vec3 D = ray.direction();

		f32 alpha0, beta0, alpha1, beta1;
		calc_coeff(O, D, 0.0f, *mCamera, alpha0, beta0);
		calc_coeff(O, D, t, *mCamera, alpha1, beta1);

		s32 lattice_x_0 = static_cast<s32>(alpha0 * (mScreenResolutionWidth - 1));
		s32 lattice_x_1 = static_cast<s32>(alpha1 * (mScreenResolutionWidth - 1));
		s32 lattice_y_0 = static_cast<s32>(beta0 * (mScreenResolutionHeight - 1));
		s32 lattice_y_1 = static_cast<s32>(beta1 * (mScreenResolutionHeight - 1));

		//lattice_x_0 = clamp(lattice_x_0, 0, static_cast<s32>(mScreenResolutionWidth - 1));
		//lattice_x_1 = clamp(lattice_x_1, 0, static_cast<s32>(mScreenResolutionWidth - 1));
		//lattice_y_0 = clamp(lattice_y_0, 0, static_cast<s32>(mScreenResolutionHeight - 1));
		//lattice_y_1 = clamp(lattice_y_1, 0, static_cast<s32>(mScreenResolutionHeight - 1));

		f32 lattice_length = sqrtf(powf(lattice_x_0 - lattice_x_1, 2) + powf(lattice_y_0 - lattice_y_1, 2));
		if (lattice_length > 0.9f)
		{
			bool isSwaped = false;
			if (lattice_x_0 > lattice_x_1)
			{
				swap(lattice_x_0, lattice_x_1);
				swap(lattice_y_0, lattice_y_1);
				isSwaped = true;
			}

			const f32 inclination = (lattice_y_1 - lattice_y_0) * 1.0f / (lattice_x_1 - lattice_x_0);//傾き：0になることはない。
			f32 deltaStep = sqrt(1 + powf(min(abs(inclination), abs(1.0f / inclination)), 2)) / 2.0f;
			for (f32 total_step = 0.0f; total_step < lattice_length; total_step += deltaStep)
			{
				f32 theta = atan(inclination);
				f32 step_x = total_step * cos(theta);
				f32 step_y = total_step * sin(theta);

				s32 lattice_x = static_cast<s32>(lattice_x_0 + step_x);
				s32 lattice_y = static_cast<s32>(lattice_y_0 + step_y);

				f32 gradation = total_step / lattice_length;
				gradation = (isSwaped ? 1 - gradation : gradation);
				vec3 drawColor = vec3::red() * (1 - gradation) + vec3::blue() * gradation;

				if (!(lattice_x > 0 && lattice_x < mScreenResolutionWidth && lattice_y > 0 && lattice_y < mScreenResolutionHeight))
				{
					continue;
				}
				mRenderTarget->setColor(lattice_x, lattice_y, drawColor);
			}

		}

		if (!isHit)
		{
			isNextRayExist = false;
			continue;
		}

		depth--;
		vec3 attenuation;
		Ray ray_next;
		isNextRayExist = record.material->scatter(ray, record, attenuation, ray_next);
		ray = ray_next;


	}
}