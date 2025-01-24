﻿
#include "engine.h"


int main()
{
	//for (u32 i = 0; i < 100; i++)
	//{
	//	ThreadPool pool{};

	//	for (u32 i = 0; i < 40; i++)
	//	{
	//		pool.push_task([]()
	//			{
	//				std::cout << "thread start\n";
	//				std::this_thread::sleep_for(std::chrono::seconds(1));
	//				std::cout << "end\n";
	//			});
	//	}


	//	std::this_thread::sleep_for(std::chrono::seconds(RandomGenerator::uniform_int(1, 5)));
	//	std::cout << "20s\n";
	//}
	//return 0;


	//オブジェクトの登録
	std::vector<std::shared_ptr<Hittable> > world;
	{
		constexpr f32 Range = 10;
		constexpr u32 Dense = 20;
		constexpr f32 Interval = 2 * Range / Dense;
		constexpr f32 Radius = 0.25 * Interval;
		for (f32 x = -Range; x < Range; x += Interval)
		{
			for (f32 z = -Range; z < Range; z += Interval)
			{
				vec3 center(x + 0.3f * RandomGenerator::signed_uniform_real(), Radius, z + 0.3 * RandomGenerator::signed_uniform_real());

				world.push_back(std::make_shared<Sphere>(
					center,
					Radius,
					std::make_shared<Metal>(
						Color(
							RandomGenerator::uniform_real(),
							RandomGenerator::uniform_real(),
							RandomGenerator::uniform_real()),
						0)));

			}
		}


		world.push_back(std::make_shared<Sphere>(vec3(0, -3000, 0), 3000, std::make_shared<Metal>(Color::Gray, 0.1)));

		world.push_back(std::make_shared<Sphere>(vec3(-12, 1, 2), 1.0f, std::make_shared<Metal>(Color(0.5, 0.8, 0.3), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(-8, 1, 0), 1.0f, std::make_shared<Metal>(Color(1, 1, 0.2), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(-4, 1, 0), 1.0f, std::make_shared<Dielectric>(10.0f)));
		world.push_back(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0f, std::make_shared<Metal>(Color::Gold, 0)));
		//world.push_back(std::make_shared<Sphere>(vec3(4, 1, 0), 1.0f, std::make_shared<Metal>(Color::Blue, 0)));
		world.push_back(std::make_shared<Sphere>(vec3(8, 1, 0), 1.0f, std::make_shared<Retroreflective>(Color::Red)));
		world.push_back(std::make_shared<Sphere>(vec3(12, 1, 2), 1.0f, std::make_shared<Metal>(Color(0, 1, 0.9), 0)));
		world.push_back(std::make_shared<Sphere>(vec3(4, 1, 0), 1.0f, std::make_shared<Dielectric>(1.5f)));
		world.push_back(std::make_shared<Sphere>(vec3(4, 1, 0), -0.9f, std::make_shared<Dielectric>(1.5f)));
	}


	//カメラの準備
	constexpr f32 BaseResolution = 1.0f * 1.0f;
	//constexpr f32 BaseResolution = 1.0f * 0.5f;
	const u32 resolutionX = static_cast<u32>(1920 * BaseResolution);
	const u32 resolutionY = static_cast<u32>(1080 * BaseResolution);

	vec3 lookAt(0, 0, 0);
	vec3 lookFrom(13, 1.5, 4);
	std::shared_ptr<Camera> camera(std::make_shared<Camera>(lookFrom, lookAt, vec3(0, 1, 0), 20, f32(resolutionX) / f32(resolutionY), 0.0, (lookFrom - lookAt).length()));

	//レンダーターゲット
	std::shared_ptr<RenderTarget> render_target(std::make_shared<RenderTarget>(resolutionX, resolutionY));

	RayTracingEngine engine(camera, render_target);
	engine.setObjects(std::move(world));

	engine.render();


	for (s32 i = 0; i < 10; i++)
	{
		//u32 w = RandomGenerator::uniform_int_round(resolutionX / 2, 400);
		//u32 h = RandomGenerator::uniform_int_round(resolutionY / 2, 400);
		u32 w = static_cast<u32>((resolutionX - 1) * RandomGenerator::uniform_real());
		u32 h = static_cast<u32>((resolutionY - 1) * RandomGenerator::uniform_real());
		engine.drawTrajectory(w, h);
	}

	engine.drawTrajectory(570, 250);

	engine.saveRenderResult(".\\product\\result.ppm");
}