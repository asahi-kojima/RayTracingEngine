#pragma once

#include "debug-setting.h"
#include "object.h"
#include "camera.h"
#include "node.h"
#include "render-target.h"
#include "util.h"


struct SecondaryInfoByRay
{
	u32 depth;
};


class RayTracingEngine
{
public:
	/// <summary>
	/// カメラとレンダーターゲットを指定して構築する
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="renderTarget">レンダーターゲット</param>
	RayTracingEngine(const std::shared_ptr<Camera>& camera, const std::shared_ptr<RenderTarget>& renderTarget);

	/// <summary>
	/// オブジェクトをセットする。実際は内部でBVHを生成している。
	/// (BVH = Bounding Volume Hierarchy)
	/// </summary>
	/// <param name="world">衝突対象となる全オブジェクトのベクトル</param>
	void setObjects(std::vector<std::shared_ptr<Hittable> >&& world);

	/// <summary>
	/// セットされたカメラとオブジェクトに基づき、
	/// レンダーターゲットに対しレンダリングを行う。
	/// </summary>
	/// <param name="sampleSize">同じピクセルから何本のレイを飛ばすか指定</param>
	/// <param name="depth">反射回数を指定</param>
	void render(const u32 sampleSize = 30, const u32 depth = 15);

	/// <summary>
	/// 指定されたパスにレンダリング結果を保存する。
	/// </summary>
	/// <param name="path">保存先のパス</param>
	void saveRenderResult(const std::string& path) const;


	void drawTrajectory(u32 i, u32 j);

private:
	std::shared_ptr<Camera> mCamera = nullptr;
	std::shared_ptr<RenderTarget> mRenderTarget = nullptr;
	u32 mScreenResolutionHeight;
	u32 mScreenResolutionWidth;

	std::unique_ptr<Node> mRootNode = nullptr;

#if REALTIME_GRAPHICAL_UDP_DEBUG
	UDPServer mUDPServer;
#endif

	/// <summary>
	/// 指定したレイ方向のワールドの「色」を回収する
	/// </summary>
	/// <param name="ray_in"></param>
	/// <param name="Depth"></param>
	/// <returns></returns>
	vec3 color(const Ray& ray_in, s32 depth);


	vec3 color(const Ray& ray_in, s32 depth, SecondaryInfoByRay& debugInfo);
};