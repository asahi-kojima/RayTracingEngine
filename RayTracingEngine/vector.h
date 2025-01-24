#pragma once
#include <cassert>
#include "typeinfo.h"

class vec3
{
public:
	vec3() = default;
	vec3(f32 x, f32 y, f32 z) : mPos{ x, y, z }, mLength(-1) {}
	vec3(u32 hexadecimal) 
		: mPos{ static_cast<f32>((hexadecimal & 0xFF0000) >> 16) / 255.0f,  static_cast<f32>((hexadecimal & 0x00FF00) >> 8) / 255.0f, static_cast<f32>((hexadecimal & 0x0000FF) >> 0) / 255.0f },
		mLength(-1)
	{
	}

	f32 getX() const { return mPos[0]; }
	f32 getY() const { return mPos[1]; }
	f32 getZ() const { return mPos[2]; }
	f32& getX() { return mPos[0]; }
	f32& getY() { return mPos[1]; }
	f32& getZ() { return mPos[2]; }
	void setX(f32 value) { mPos[0] = value; }
	void setY(f32 value) { mPos[1] = value; }
	void setZ(f32 value) { mPos[2] = value; }

	f32& operator[](size_t i);
	f32 operator[](size_t i) const;

	vec3 operator-() const;

	vec3 operator+(const vec3&) const;
	vec3& operator+=(const vec3&);
	vec3 operator+(const f32) const;
	vec3& operator+=(const f32);
	vec3 operator-(const vec3&) const;
	vec3& operator-=(const vec3&);
	vec3 operator-(const f32) const;
	vec3& operator-=(const f32);
	vec3 operator*(const vec3&) const;
	vec3& operator*=(const vec3&);
	vec3 operator*(const f32) const;
	vec3& operator*=(const f32);
	vec3 operator/(const f32) const;
	vec3& operator/=(const f32);

	vec3 normalize();
	f32 length() const;
	static f32 length(const vec3&);
	f32 lengthSquared() const;
	static f32 lengthSquared(const vec3&);
	static vec3 cross(const vec3&, const vec3&);
	//static f32 inner(const vec3&, const vec3&);

	static inline vec3 zero() { return vec3(0.0f, 0.0f, 0.0f); };
	static inline vec3 one() { return vec3(1.0f, 1.0f, 1.0f); };


private:
	f32 mPos[3];
	f32 mLength;
};


vec3 operator*(const f32 value, const vec3& v);
vec3 normalize(const vec3& v);
vec3 cross(const vec3& v0, const vec3& v1);
f32 dot(const vec3& v0, const vec3& v1);
vec3 reflect(const vec3& v, const vec3& n);

vec3 random_in_unit_sphere();
