#pragma once
#include <vector>
#include <fstream>
#include "vector.h"
#include "color.h"
class RenderTarget
{
public:
	RenderTarget(u32 resolutionWidth, u32 resolutionHeight, Color clearColor = Color::Black)
		: mResolutionWidth(resolutionWidth)
		, mResolutionHeight(resolutionHeight)
		, mPixelSize(resolutionHeight* resolutionWidth)
	{
		mPixels.resize(mPixelSize);
		for (auto& pixel : mPixels)
		{
			pixel = clearColor;
		}
	}

	void setColor(u32 width, u32 height, const Color& color)
	{
		u32 index = calcIndex(width, height);
		mPixels[index] = color;
	}

	void saveRenderResult(const std::string& path)
	{
		std::ofstream stream(path.c_str());
		
		stream << "P3\n" << mResolutionWidth << " " << mResolutionHeight << "\n255\n";

		for (s32 j = mResolutionHeight - 1; j >= 0; j--)
		{
			for (u32 i = 0; i < mResolutionWidth; i++)
			{
				Color color = mPixels[calcIndex(i, j)];
				color = Color(sqrt(color[0]), sqrt(color[1]), sqrt(color[2]));
				color[0] = static_cast<f32>(static_cast<s32>(255.99 * color[0]));
				color[1] = static_cast<f32>(static_cast<s32>(255.99 * color[1]));
				color[2] = static_cast<f32>(static_cast<s32>(255.99 * color[2]));
				stream << color[0] << " " << color[1] << " " << color[2] << "\n";
			}
		}
		stream.close();
	}
	u32 getResolutionWidth() const { return mResolutionWidth; }
	u32 getResolutionHeight() const { return mResolutionHeight; }

private:
	const u32 mResolutionWidth;
	const u32 mResolutionHeight;
	const u32 mPixelSize;
	std::vector<Color> mPixels;

	u32 calcIndex(u32 width, u32 height) const
	{
		return height * mResolutionWidth + width;
	}
};