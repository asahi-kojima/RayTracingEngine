#include "engine.h"

//============================================
/*
開発メモ
・もし巨大なオブジェクトに変な模様が付いたら、
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
				f32 u = static_cast<f32>(i + RandomGenerator::signed_uniform_real() * 0.1f) / static_cast<f32>(mScreenResolutionWidth - 1);
				f32 v = static_cast<f32>(j + RandomGenerator::signed_uniform_real() * 0.1f) / static_cast<f32>(mScreenResolutionHeight - 1);
				Ray ray = mCamera->getRay(u, v);
				resultColor += color(ray, depth);
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
	HitRecord record;
	if (mRootNode->hit(ray_in, 0.01f, 1e+10, record))
	{
		Ray ray_scatterd;
		vec3 attenuation;
		if (depth >= 0 && record.material->scatter(ray_in, record, attenuation, ray_scatterd))
		{
			vec3 colorResult = color(ray_scatterd, depth - 1);
			return attenuation * colorResult;
		}
		else
		{
			return vec3::black();
		}
	}
	else
	{
		vec3 unit_direction = normalize(ray_in.direction());
		f32 t = 0.5f * (unit_direction.getY() + 1.0f);
		return vec3(0.4f, 0.4f, 1.0f) * (1.0f - t) + vec3(0.5f, 0.7f, 1.0f) * t;
	}
}