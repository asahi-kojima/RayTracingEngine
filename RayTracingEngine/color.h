#pragma once
#include "vector.h"


class Color
{
public:
	Color(const f32 r, const f32 g, const f32 b, const f32 alpha = 1.0f)
		: mRGB(r, g, b), mAlpha(alpha)
	{
		
	}

	Color(const u32 hexadecimal) 
		: mRGB{ static_cast<f32>((hexadecimal & 0xFF0000) >> 16) / 255.0f,  static_cast<f32>((hexadecimal & 0x00FF00) >> 8) / 255.0f, static_cast<f32>((hexadecimal & 0x0000FF) >> 0) / 255.0f }, mAlpha(1.0f)
	{
		if (hexadecimal > 0xFFFFFF)
		{
			assert(0);
		}
	}



	static Color White;
	static Color Black;
	static Color Gray;
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Yellow;
	static Color Bronze;

private:
	vec3 mRGB;
	f32 mAlpha;
};


inline Color Color::White(0xFFFFFF);
inline Color Color::Black(0x000000);
inline Color Color::Gray(0x808080);
inline Color Color::Red(0xFF0000);
inline Color Color::Green(0x00FF00);
inline Color Color::Blue(0x0000FF);
inline Color Color::Yellow(0xFFFF00);
inline Color Color::Bronze(0xD99730);