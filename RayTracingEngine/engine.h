#pragma once
#include <iostream>
#include <random>
#include <omp.h>
#include <map>
#include <thread>
#include <mutex>
#include "object.h"
#include "camera.h"
#include "node.h"
#include "render-target.h"
#include "util.h"


class RayTracingEngine
{
public:
	RayTracingEngine(const std::shared_ptr<Camera>& camera, const std::shared_ptr<RenderTarget>& renderTarget)
		: mCamera(camera)
		, mRenderTarget(renderTarget)
		, mScreenResolutionHeight(renderTarget->getResolutionHeight())
		, mScreenResolutionWidth(renderTarget->getResolutionWidth())
	{
	}

	/// <summary>
	/// オブジェクトをセットする。実際は内部でBVHを生成している。
	/// (BVH = Bounding Volume Hierarchy)
	/// </summary>
	/// <param name="world"></param>
	void setObjects(std::vector<std::shared_ptr<Hittable> >&& world)
	{
		mRootNode = std::make_unique<Node>(std::move(world));
	}

	/// <summary>
	/// セットされたカメラとオブジェクトに基づき、
	/// レンダーターゲットに対しレンダリングを行う。
	/// </summary>
	/// <param name="sampleSize"></param>
	/// <param name="depth"></param>
	void render(const u32 sampleSize = 30, const u32 depth = 20)
	{
		if (!mRootNode)
		{
			assert(0);
		}


#pragma omp parallel for
		for (s32 i = 0; i < mScreenResolutionWidth; i++)
		{
			for (u32 j = 0; j < mScreenResolutionHeight; j++)
			{
				vec3 resultColor = vec3::zero();
				for (u32 s = 0; s < sampleSize; s++)
				{
					f32 u = f32(i + RandomGenerator::signed_uniform_real() * 0.5f) / f32(mScreenResolutionWidth - 1);
					f32 v = f32(j + RandomGenerator::signed_uniform_real() * 0.5f) / f32(mScreenResolutionHeight - 1);
					Ray ray = mCamera->getRay(u, v);
					resultColor += color(ray, depth);
				}
				resultColor /= sampleSize;
				mRenderTarget->setColor(i, j, resultColor);
			}
		}
	}

	/// <summary>
	/// 指定されたパスにレンダリング結果を保存する。
	/// </summary>
	/// <param name="path"></param>
	void saveRenderResult(const std::string& path) const
	{
		std::cout << "Save Render Result in Path[" << path << "]" << std::endl;
		mRenderTarget->saveRenderResult(path);
	}

private:
	std::shared_ptr<Camera> mCamera = nullptr;
	std::shared_ptr<RenderTarget> mRenderTarget = nullptr;
	u32 mScreenResolutionHeight;
	u32 mScreenResolutionWidth;

	std::unique_ptr<Node> mRootNode = nullptr;




	vec3 color(const Ray& ray_in, s32 Depth)
	{
		HitRecord record;
		if (mRootNode->hit(ray_in, 0.001, 1e+10, record))
		{
			Ray ray_scatterd;
			vec3 attenuation;
			if (Depth >= 0 && record.material->scatter(ray_in, record, attenuation, ray_scatterd))
			{
				vec3 colorResult = color(ray_scatterd, Depth - 1);
				return attenuation * colorResult;
			}
			else
			{
				return vec3::zero();
			}
		}
		else
		{
			vec3 unit_direction = normalize(ray_in.direction());
			f32 t = 0.5f * (unit_direction.getY() + 1.0f);
			return vec3(1.0f, 1.0f, 1.0f) * (1.0f - t) + vec3(0.5f, 0.7f, 1.0f) * t;
		}
	}
};