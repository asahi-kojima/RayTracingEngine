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
	f32 u = static_cast<f32>(i) / static_cast<f32>(mScreenResolutionWidth - 1);
	f32 v = static_cast<f32>(j) / static_cast<f32>(mScreenResolutionHeight - 1);
	Ray ray = mCamera->getRay(u, v);

	const s32 MaxDepth = 20;
	s32 depth = MaxDepth;
	bool isNextRayExist = true;
	HitRecord record;
	while (depth >= 0 && isNextRayExist)
	{
		bool isHit = mRootNode->hit(ray, 0.01f, 1e+10, record);
		f32 t = (isHit ? record.t : 100.0f);

		const u32 DeltaStep = 1000;
		for (u32 i = 1; i < DeltaStep; i++)
		{
			const f32 t0 = i * t / DeltaStep;

			const vec3 P = ray.pointAt(t0);
			const vec3 S = mCamera->getScreenOrigin();
			const vec3 I = mCamera->getEyeOrigin();
			const vec3 W = mCamera->getCameraZ();
			const f32 s = dot(W, (S - I)) / dot(W, (P - I));

			const vec3 intersectionPoint = I + s * (P - I);

			const f32 alpha = dot(intersectionPoint - S, mCamera->getCameraX()) / mCamera->getHorizontalScreenScale();
			const f32 beta = dot(intersectionPoint - S, mCamera->getCameraY()) / mCamera->getVerticalScreenScale();

			if (!(alpha >= 0.0f && alpha <= 1.0f && beta >= 0.0f && beta <= 1.0f))
			{
				continue;
			}


			s32 screenX = static_cast<s32>((mScreenResolutionWidth - 1) * alpha);
			s32 screenY = static_cast<s32>((mScreenResolutionHeight - 1) * beta);

			const s32 LineWidth = 1;
			for (s32 j = -LineWidth; j <= LineWidth; j++)
			{
				for (s32 k = -LineWidth; k <= LineWidth; k++)
				{
					s32 shiftedScreenX = min(max((screenX + j), 0), mScreenResolutionWidth - 1);
					s32 shiftedScreenY = min(max((screenY + k), 0), mScreenResolutionHeight - 1);

					vec3 color = vec3::red() + (vec3::blue() - vec3::red()) * (i * 1.0f / DeltaStep);
					mRenderTarget->setColor(shiftedScreenX, shiftedScreenY, color * depth / MaxDepth);
				}
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


		/*if (mRootNode->hit(ray, 0.01f, 1e+10, record))
		{
			f32 t = record.t;
			for (u32 i = 1; i < 100; i++)
			{
				const f32 t0 = i * t / 100.0f;

				const vec3 P = ray.pointAt(t0);
				const vec3 S = mCamera->getScreenOrigin();
				const vec3 I = mCamera->getEyeOrigin();
				const vec3 W = mCamera->getCameraZ();
				const f32 s = dot(W, (S - I)) / dot(W, (P - I));

				const vec3 intersectionPoint = I + s * (P - I);

				const f32 alpha = dot(intersectionPoint - S, mCamera->getCameraX()) / mCamera->getHorizontalScreenScale();
				const f32 beta = dot(intersectionPoint - S, mCamera->getCameraY()) / mCamera->getVerticalScreenScale();

				if (!(alpha >= 0.0f && alpha <= 1.0f && beta >= 0.0f && beta <= 1.0f))
				{
					continue;
				}


				s32 screenX = static_cast<s32>((mScreenResolutionWidth - 1) * alpha);
				s32 screenY = static_cast<s32>((mScreenResolutionHeight - 1) * beta);

				const s32 LineWidth = 1;
				for (s32 j = -LineWidth; j <= LineWidth; j++)
				{
					for (s32 k = -LineWidth; k <= LineWidth; k++)
					{
						s32 shiftedScreenX = min(max((screenX + j), 0), mScreenResolutionWidth - 1);
						s32 shiftedScreenY = min(max((screenY + k), 0), mScreenResolutionHeight - 1);

						mRenderTarget->setColor(shiftedScreenX, shiftedScreenY, vec3::white());
					}
				}
			}

			depth--;
			vec3 attenuation;
			Ray ray_next;
			isNextRayExist = record.material->scatter(ray, record, attenuation, ray_next);
			ray = ray_next;
		}
		else
		{
			isNextRayExist = false;
		}*/
	}
}