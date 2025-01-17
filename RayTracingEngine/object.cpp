#include "object.h"

bool AABB::hit(const Ray& r, const f32 t_min, const f32 t_max, HitRecord& record)
{
	return false;
}

bool Sphere::hit(const Ray& r, const f32 t_min, const f32 t_max, HitRecord& record)
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2 * dot(r.direction(), oc);
	float c = dot(oc, oc) - radius * radius;
	float D = b * b - 4 * a * c;
#ifdef _DEBUG
	float h = this->center.getY();
#endif
	if (D > 0)
	{
		//âÇ™ê≥ïâ
		if (c < 0)
		{
			float tmp = (-b + sqrt(D)) / (2.0 * a);
			if (tmp < t_max && tmp > t_min)
			{
				record.t = tmp;
				record.pos = r.pointAt(tmp);
				record.normal = (record.pos - center) / radius;
				record.material = this->material;
				return true;
			}
		}
		else
		{
			float tmp = (-b - sqrt(D)) / (2.0 * a);
			if (tmp < t_max && tmp > t_min)
			{
				record.t = tmp;
				record.pos = r.pointAt(tmp);
				record.normal = (record.pos - center) / radius;
				record.material = this->material;
				return true;
			}

			tmp = (-b + sqrt(D)) / (2.0 * a);
			if (tmp < t_max && tmp > t_min)
			{
				record.t = tmp;
				record.pos = r.pointAt(tmp);
				record.normal = (record.pos - center) / radius;
				record.material = this->material;
				return true;
			}
		}
	}

	return false;
}

AABB Sphere::calcAABB()
{
	const vec3 v_min = center - radius;
	const vec3 v_max = center + radius;
	return AABB(v_min, v_max);
}

bool isIntersecting(const AABB& aabb, const Ray& ray, f32 t_min, f32 t_max)
{
	const vec3& min_pos = aabb.getMinPos();
	const vec3& max_pos = aabb.getMaxPos();
	const vec3& origin = ray.origin();
	const vec3& direction = ray.direction();

	auto intersection_per_axis = [&t_min, &t_max](f32 min_pos, f32 max_pos, f32 origin, f32 direction, f32& t_min_x, f32& t_max_x)->bool
		{
			t_min_x = std::min((min_pos - origin) / direction, (max_pos - origin) / direction);
			t_max_x = std::max((min_pos - origin) / direction, (max_pos - origin) / direction);

			if (isnan(t_min_x) || isnan(t_max_x))
			{
				return false;
			}

			t_min = fmax(t_min, t_min_x);
			t_max = fmin(t_max, t_max_x);

			if (t_max <= t_min)
			{
				return false;
			}

			return true;
		};

	//xé≤îªíË
	f32 t_min_x, t_max_x;
	{
		f32 min_pos_x = min_pos.getX();
		f32 max_pos_x = max_pos.getX();
		f32 origin_x = origin.getX();
		if (!intersection_per_axis(min_pos_x, max_pos_x, origin_x, direction.getX(), t_min_x, t_max_x))
		{
			return false;
		}
	}

	f32 t_min_y, t_max_y;
	{
		f32 min_pos_y = min_pos.getY();
		f32 max_pos_y = max_pos.getY();
		f32 origin_y = origin.getY();
		if (!intersection_per_axis(min_pos_y, max_pos_y, origin_y, direction.getY(), t_min_y, t_max_y))
		{
			return false;
		}
	}

	f32 t_min_z, t_max_z;
	{
		f32 min_pos_z = min_pos.getZ();
		f32 max_pos_z = max_pos.getZ();
		f32 origin_z = origin.getZ();
		if (!intersection_per_axis(min_pos_z, max_pos_z, origin_z, direction.getZ(), t_min_z, t_max_z))
		{
			return false;
		}
	}


	//auto intersection_per_axis = [](f32 min_pos, f32 max_pos, f32 origin, f32 direction, f32& x_min, f32& x_max)->bool
	//	{
	//		if (direction == 0.0)
	//		{
	//			return false;
	//		}
	//		x_min = std::min((min_pos - origin) / direction, (max_pos - origin) / direction);
	//		x_max = std::max((min_pos - origin) / direction, (max_pos - origin) / direction);
	//		return true;
	//	};

	////xé≤îªíË
	//f32 x_min, x_max;
	//{
	//	f32 min_pos_x = min_pos.getX();
	//	f32 max_pos_x = max_pos.getX();
	//	f32 origin_x = origin.getX();
	//	if (!intersection_per_axis(min_pos_x, max_pos_x, origin_x, direction.getX(), x_min, x_max))
	//	{
	//		if (origin.getX() < min_pos_x || origin.getX() > max_pos_x)
	//			return false;
	//	}
	//}

	//f32 y_min, y_max;
	//{
	//	f32 min_pos_y = min_pos.getY();
	//	f32 max_pos_y = max_pos.getY();
	//	f32 origin_y = origin.getY();
	//	if (!intersection_per_axis(min_pos_y, max_pos_y, origin_y, direction.getY(), y_min, y_max))
	//	{
	//		if (origin_y < min_pos_y || origin_y > max_pos_y)
	//			return false;
	//	}
	//}

	//f32 z_min, z_max;
	//{
	//	f32 min_pos_z = min_pos.getZ();
	//	f32 max_pos_z = max_pos.getZ();
	//	f32 origin_z = origin.getZ();
	//	if (!intersection_per_axis(min_pos_z, max_pos_z, origin_z, direction.getZ(), z_min, z_max))
	//	{
	//		if (origin_z < min_pos_z || origin_z > max_pos_z)
	//			return false;
	//	}
	//}


	return true;
}
