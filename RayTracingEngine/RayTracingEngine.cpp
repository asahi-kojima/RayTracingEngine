
#include "engine.h"


int main()
{
	//オブジェクトの登録
	std::vector<std::shared_ptr<Hittable> > world;
	{
		constexpr f32 Range = 50;
		constexpr u32 Dense = 2 * Range;
		constexpr f32 Interval = 2 * Range / Dense;
		constexpr f32 Radius = 0.1 * Interval;
		for (f32 x = -Range; x < Range; x += Interval)
		{
			for (f32 z = -Range; z < Range; z += Interval)
			{
				vec3 center(x + 0.3f * RandomGenerator::signed_uniform_real(), Radius, z + 0.3 * RandomGenerator::signed_uniform_real());

				world.push_back(std::make_shared<Sphere>(
					center, 
					Radius, 
					std::make_shared<Metal>(
						vec3(
							RandomGenerator::uniform_real(), 
							RandomGenerator::uniform_real(), 
							RandomGenerator::uniform_real()), 
						RandomGenerator::uniform_real() < 0.2f ? 0.0f : RandomGenerator::uniform_real())));

			}
		}


		world.push_back(std::make_shared<Sphere>(vec3(0, -3000, 0), 3000, std::make_shared<Metal>(vec3::gray(), 0.0)));
		world.push_back(std::make_shared<Sphere>(vec3(-12, 1, 2), 1.0f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(-8, 1, 0), 1.0f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(-4, 1, 0), 1.0f, std::make_shared<Metal>(vec3(0.3, 0.5, 0.9), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(-4, 1, 0), -0.9f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(4, 1, 0), 1.0f, std::make_shared<Metal>(vec3(1.0, 0.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(4, 1, 0), -0.9f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(8, 1, 0), 1.0f, std::make_shared<Metal>(vec3::gray(), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(12, 1, 2), 1.0f, std::make_shared<Metal>(vec3(1.0, 1.0, 1.0), 0)));
	}


	//カメラの準備
	constexpr f32 BaseResolution = 1.0f;
	const u32 resolutionX = static_cast<u32>(1920 * BaseResolution);
	const u32 resolutionY = static_cast<u32>(1080 * BaseResolution);
	vec3 lookFrom(13, 2, 5);
	vec3 lookAt(0, 0, 0);
	std::shared_ptr<Camera> camera(std::make_shared<Camera>(lookFrom, lookAt, vec3(0, 1, 0), 20, f32(resolutionX) / f32(resolutionY), 0.0, (lookFrom - lookAt).length()));

	//レンダーターゲット
	std::shared_ptr<RenderTarget> render_target(std::make_shared<RenderTarget>(resolutionX, resolutionY));

	RayTracingEngine engine(camera, render_target);
	engine.setObjects(std::move(world));

	engine.render();
	engine.saveRenderResult(".\\product\\result.ppm");
}