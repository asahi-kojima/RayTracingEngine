#include <iostream>
#include <random>
#include <omp.h>
#include "object.h"
#include "camera.h"
#include "render-target.h"
#include "engine.h"

f32 getRandom() { return rand() / (RAND_MAX + 1.0f); };

vec3 color(const Ray& ray_in, const std::unique_ptr<Node>& root_node, s32 Depth)
{
	HitRecord record;
	if (root_node->hit(ray_in, 0.001, 1e+10, record))
	{
		Ray ray_scatterd;
		vec3 attenuation;
		if (Depth >= 0 && record.material->scatter(ray_in, record, attenuation, ray_scatterd))
		{
			vec3 colorResult = color(ray_scatterd, root_node, Depth - 1);
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

int main()
{


	//オブジェクトの登録
	std::vector<std::shared_ptr<Hittable> > world;
	{
		f32 range = 10;
		u32 dense = 20;
		for (int a = -11; a < 11; a++)
		{
			for (int b = -11; b < 11; b++)
			{
				float chooseMat = getRandom();
				vec3 center(a + 0.9f * getRandom(), 0.2, b + 0.9 * getRandom());
				if ((center - vec3(4, 0.2f, 0)).length() > 0.9f)
				{
					world.push_back(std::make_shared<Sphere>(center, 0.2, std::make_shared<Metal>(vec3(getRandom() * getRandom(), getRandom() * getRandom(), getRandom() * getRandom()), 0)));
				}
			}
		}


		world.push_back(std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, std::make_shared<Metal>(vec3::gray(), 0.0)));
		world.push_back(std::make_shared<Sphere>(vec3(-12, 1, 2), 1.0f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(-8, 1, 0), 1.0f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(-4, 1, 0), 1.0f, std::make_shared<Metal>(vec3(0.3, 0.5, 0.9), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(-4, 1, 0), -0.9f,			std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(4, 1, 0), 1.0f, std::make_shared<Metal>(vec3(1.0, 0.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(4, 1, 0), -0.9f,			std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(8, 1, 0), 1.0f, std::make_shared<Metal>(vec3::gray(), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(12, 1, 2), 1.0f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
	}

	//ワールド内のオブジェクトからBVHを作成
	std::unique_ptr<Node> mRootNode = std::make_unique<Node>(std::move(world));

	//カメラの準備
	constexpr f32 BaseResolution = 2.0f;
	const u32 resolutionX = static_cast<u32>(1920 * BaseResolution);
	const u32 resolutionY = static_cast<u32>(1080 * BaseResolution);
	vec3 lookFrom(13, 2, 5);
	vec3 lookAt(0, 0, 0);
	Camera camera(lookFrom, lookAt, vec3(0, 1, 0), 20, f32(resolutionX) / f32(resolutionY), 0.0, (lookFrom - lookAt).length());

	//レンダーターゲット
	RenderTarget render_target(resolutionX, resolutionY);

	const u32 SampleSize = 30;
	const u32 Depth = 20;
#pragma omp parallel for
	for (s32 i = 0; i < resolutionX; i++)
	{
		if (i % 100 == 0) std::cout << i << " : finish\n";
		for (u32 j = 0; j < resolutionY; j++)
		{
			vec3 resultColor = vec3::zero();
			for (u32 s = 0; s < SampleSize; s++)
			{
				f32 u = f32(i + (2 * getRandom() - 1) * 0.5f) / f32(resolutionX - 1);
				f32 v = f32(j + (2 * getRandom() - 1) * 0.5f) / f32(resolutionY - 1);
				Ray ray = camera.getRay(u, v);
				resultColor += color(ray, mRootNode, Depth);
			}
			resultColor /= SampleSize;
			render_target.setColor(i, j, resultColor);
		}
	}

	render_target.output_result(".\\result.ppm");

	std::cout << "Hello World!\n";
}