#pragma once
#include "vector.h"


class Color
{
public:
	Color() = default;
	Color(const f32 r, const f32 g, const f32 b, const f32 alpha = 1.0f);
	Color(const vec3& rbg, const f32 alpha = 1.0f);
	Color(const u32 hexadecimal);

	f32 operator[](size_t i)const;
	f32& operator[](size_t i);


	Color operator+(const Color& rhs) const;
	Color operator*(const Color& rhs) const;
	Color operator*(const f32 value) const;

	Color& operator+=(const Color& rhs);
	Color& operator*=(const Color& rhs);
	Color& operator*=(const f32 value);
	Color& operator/=(const f32 value);


	static const Color White;
	static const Color Black;
	static const Color Gray;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Yellow;
	static const Color Magenta;
	static const Color Orange;
	static const Color Gold;
	static const Color Bronze;

private:
	vec3 mRGB;
	f32 mAlpha;
};





