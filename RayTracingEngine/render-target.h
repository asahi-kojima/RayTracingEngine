#pragma once
#include <vector>
#include <fstream>
#include "vector.h"

class RenderTarget
{
public:
	RenderTarget(u32 resolutionWidth, u32 resolutionHeight)
		: mResolutionWidth(resolutionWidth)
		, mResolutionHeight(resolutionHeight)
		, mPixelSize(resolutionHeight* resolutionWidth)
	{
		mPixels.resize(mPixelSize);
	}

	void setColor(u32 width, u32 height, vec3 color)
	{
		u32 index = calcIndex(width, height);
		mPixels[index] = color;
	}

	void output_result(const std::string& path)
	{
		std::ofstream stream(path.c_str());
		stream << "P3\n" << mResolutionWidth << " " << mResolutionHeight << "\n255\n";
		for (s32 j = mResolutionHeight - 1; j >= 0; j--)
		{
			if (j % 100 == 0) std::cout << j << " : finish\n";
			for (u32 i = 0; i < mResolutionWidth; i++)
			{
				vec3 color = mPixels[calcIndex(i, j)];
				color = vec3(sqrt(color[0]), sqrt(color[1]), sqrt(color[2]));
				color[0] = int(255.99 * color[0]);
				color[1] = int(255.99 * color[1]);
				color[2] = int(255.99 * color[2]);
				stream << color[0] << " " << color[1] << " " << color[2] << "\n";
			}
		}

		stream.close();
	}

private:
	const u32 mResolutionWidth;
	const u32 mResolutionHeight;
	const u32 mPixelSize;
	std::vector<vec3> mPixels;

	u32 calcIndex(u32 width, u32 height) const
	{
		return height * mResolutionWidth + width;
	}
};