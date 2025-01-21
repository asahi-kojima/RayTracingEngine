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
	/// �J�����ƃ����_�[�^�[�Q�b�g���w�肵�č\�z����
	/// </summary>
	/// <param name="camera">�J����</param>
	/// <param name="renderTarget">�����_�[�^�[�Q�b�g</param>
	RayTracingEngine(const std::shared_ptr<Camera>& camera, const std::shared_ptr<RenderTarget>& renderTarget);

	/// <summary>
	/// �I�u�W�F�N�g���Z�b�g����B���ۂ͓�����BVH�𐶐����Ă���B
	/// (BVH = Bounding Volume Hierarchy)
	/// </summary>
	/// <param name="world">�ՓˑΏۂƂȂ�S�I�u�W�F�N�g�̃x�N�g��</param>
	void setObjects(std::vector<std::shared_ptr<Hittable> >&& world);

	/// <summary>
	/// �Z�b�g���ꂽ�J�����ƃI�u�W�F�N�g�Ɋ�Â��A
	/// �����_�[�^�[�Q�b�g�ɑ΂������_�����O���s���B
	/// </summary>
	/// <param name="sampleSize">�����s�N�Z�����牽�{�̃��C���΂����w��</param>
	/// <param name="depth">���ˉ񐔂��w��</param>
	void render(const u32 sampleSize = 30, const u32 depth = 15);

	/// <summary>
	/// �w�肳�ꂽ�p�X�Ƀ����_�����O���ʂ�ۑ�����B
	/// </summary>
	/// <param name="path">�ۑ���̃p�X</param>
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
	/// �w�肵�����C�����̃��[���h�́u�F�v���������
	/// </summary>
	/// <param name="ray_in"></param>
	/// <param name="Depth"></param>
	/// <returns></returns>
	vec3 color(const Ray& ray_in, s32 depth);


	vec3 color(const Ray& ray_in, s32 depth, SecondaryInfoByRay& debugInfo);
};