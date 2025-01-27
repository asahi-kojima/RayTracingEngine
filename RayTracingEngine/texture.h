#pragma once
#include <memory>
#include "vector.h"
#include "color.h"

class Texture
{
public:
	virtual Color color(f32 u, f32 v, const vec3& p) = 0;
};



class ConstantTexture : public Texture
{
public:
	ConstantTexture() = default;
	ConstantTexture(Color c) : mColor(c){}


private:
	virtual Color color(f32 u, f32 v, const vec3& p) override;

	Color mColor;
};


class CheckerTexture : public Texture
{
public:
	CheckerTexture() = default;
	CheckerTexture(std::unique_ptr<Texture>&& odd_texure, std::unique_ptr<Texture>&& even_texture) 
		: mOddTexture(std::move(odd_texure))
		, mEvenTexture(std::move(even_texture)){}

private:
	virtual Color color(f32 u, f32 v, const vec3& p) override;

	std::unique_ptr<Texture> mOddTexture;
	std::unique_ptr<Texture> mEvenTexture;
};