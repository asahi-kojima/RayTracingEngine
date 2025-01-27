#include "material.h"
#include "hittable.h"
#include "util.h"

//======================================================
// �����o�[�g
//======================================================
bool Lambertian::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	const vec3 target = record.pos + record.normal + random_in_unit_sphere();
	ray_scattered.direction() = target - record.pos;
	ray_scattered.origin() = record.pos;
	attenuation = mTexture->color(0, 0, record.pos);
	return true;
}

//======================================================
// ����
//======================================================
bool Metal::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	//�V���ȃ��C���Z�b�g
	vec3 reflected_ray = reflect(ray_in.direction(), record.normal);

	//�Ō��0.1f�ɂ��Ă�reflected_ray�̒������P�����A����Ɠ����I�[�_�[�ɂ���Ƌ��E�Ŗ������˂��N����̂ŁA�����h�����߁B
	ray_scattered = Ray(record.pos, reflected_ray + fuzz * random_in_unit_sphere() * 0.1f);

	//�\�ʐF���Z�b�g
	attenuation = albedo;

	return (dot(ray_scattered.direction(), record.normal) > 0);
}


//======================================================
// �U�d��
//======================================================
bool Dielectric::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	attenuation = Color::White;

	vec3 outwardNormal;
	vec3 reflected = reflect(ray_in.direction(), record.normal);
	f32 niOverNt;
	vec3 refracted;
	f32 cosine;
	f32 reflectProb;

	// ��������o�Ă��悤�Ƃ��Ă��鎞
	if (dot(ray_in.direction(), record.normal) > 0)
	{
		outwardNormal = record.normal;
		niOverNt = refIdx;
		cosine = dot(ray_in.direction(), record.normal) / ray_in.direction().length();
	}
	// �O��������ł��Ă��鎞
	else
	{
		outwardNormal = -record.normal;
		niOverNt = 1.0 / refIdx;
		cosine = -dot(ray_in.direction(), record.normal) / ray_in.direction().length();
	}

	if (isRefract(ray_in.direction(), outwardNormal, niOverNt, refracted))
	{
		reflectProb = schlick(cosine, refIdx);
		if (RandomGenerator::uniform_real() < reflectProb)
		{
			ray_scattered = Ray(record.pos, reflected);
		}
		else
		{
			ray_scattered = Ray(record.pos, refracted);
		}
	}
	else
	{
		ray_scattered = Ray(record.pos, reflected);
	}

	return true;

}

bool Dielectric::isRefract(const vec3& v, const vec3& n, f32 niOverNt, vec3& refracted)
{
	vec3 uv = normalize(v);
	f32 dt = dot(uv, n);

	// �X�l�����������Ă�BD��cos^2Theta�ɑ������A���Ȃ��������B
	f32 D = 1.0 - niOverNt * niOverNt * (1 - dt * dt);

	// ��������ꍇ�B���܌����Z�o����B
	if (D > 0)
	{
		refracted = niOverNt * (uv - n * dt) + n * sqrt(D);
		return true;
	}

	// �S���˂̏ꍇ
	return false;
}

f32 Dielectric::schlick(f32 cosine, f32 refIdx)
{
	f32 r0 = (1 - refIdx) / (1 + refIdx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}


//======================================================
// �ċA�����ˑf��
//======================================================
bool Retroreflective::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	ray_scattered.direction() = -ray_in.direction();
	ray_scattered.origin() = record.pos;

	attenuation = albedo;

	return false;
}


//======================================================
// ����
//======================================================

bool SunLight::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	attenuation = Color::White;
	return false;
}



//======================================================
// �d�͏�
//======================================================
bool GravitationalField::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	const f32 M = mGravityScale;
	const f32 m = 1.0f;
	const f32 v = 10.0f;

	const vec3 CP = record.pos - mCenter;
	const f32 R = CP.length();

	const vec3 OC = mCenter - ray_in.origin();
	const vec3 D = ray_in.direction();
	const f32 ray_center_dist = (D * (dot(OC, D) / D.lengthSquared()) - OC).length();

	const f32 E = 0.5f * m * v * v - G * M * m / R;
	const f32 L = m * v * ray_center_dist;
	const f32 R0 = L * L / (G * M);
	const f32 typical_E = L * L / (2 * R0 * R0);//�T�^�I�ȃG�l���M�[�X�P�[�����Ӗ����Ă���A���ۂ̃G�l���M�[�Ƃ͕�

	//���S��
	const f32 e = sqrtf(1.0f + E / typical_E);
	if (e < 1.0f)
	{
		attenuation = Color::Black;
		return false;
	}

	attenuation = Color::White;

	{
		const vec3 ux = -normalize(D);
		const vec3 uz = normalize(cross(ux, CP));
		const vec3 uy = cross(uz, ux);
		const f32 h = ray_center_dist;
		const f32 theta = asinf(h / R);

		const f32 phi = -(acosf(((R0 / OC.length()) - 1) / e) - theta);// assert(phi < 0);
		const f32 phi2 = 2 * phi;

		const f32 x = R * cos(theta);
		const f32 y = R * sin(theta);
		const f32 cosPhi2 = cos(phi2);
		const f32 sinPhi2 = sin(phi2);

		const f32 outgoing_x = cosPhi2 * x + sinPhi2 * y;
		const f32 outgoing_y = sinPhi2 * x - cosPhi2 * y;
		const vec3 outgoing_pos = outgoing_x * ux + outgoing_y * uy + mCenter;
		const vec3 outgoing_dir = cosPhi2 * ux + sinPhi2 * uy;

		ray_scattered.direction() = outgoing_dir;
		ray_scattered.origin() = outgoing_pos;
	}
	return true;
}


//======================================================
// �^���d�͏�i�����Čv�Z�~�X�����Ă���j
//======================================================
bool QuasiGravitationalField::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	const f32 M = mGravityScale;
	const f32 m = 1.0f;
	const f32 v = 10.0f;

	const vec3 CP = record.pos - mCenter;
	const f32 R = CP.length();

	const vec3 OC = mCenter - ray_in.origin();
	const vec3 D = ray_in.direction();
	const f32 ray_center_dist = (D * (dot(OC, D) / D.lengthSquared()) - OC).length();

	const f32 E = 0.5f * m * v * v - G * M * m / R;
	const f32 L = m * v * ray_center_dist;
	const f32 R0 = L * L / (G * M);
	const f32 typical_E = L * L / (2 * R0 * R0);//�T�^�I�ȃG�l���M�[�X�P�[�����Ӗ����Ă���A���ۂ̃G�l���M�[�Ƃ͕�

	//���S��
	const f32 e = sqrtf(1.0f + E / typical_E);
	if (e < 1.0f)
	{
		attenuation = Color::Black;
		return false;
	}

	attenuation = Color::White;

	{
		const vec3 ux = -normalize(D);
		const vec3 uz = normalize(cross(ux, CP));
		const vec3 uy = cross(uz, ux);
		const f32 h = abs(dot(ux, CP));
		const f32 theta = asinf(h / R);

		const f32 phi = -(acosf(((R0 / OC.length()) - 1) / e) - theta);
		const f32 phi2 = 2 * phi;

		const f32 x = R * cos(theta);
		const f32 y = R * sin(theta);
		const f32 cosPhi2 = cos(phi2);
		const f32 sinPhi2 = sin(phi2);

		const f32 outgoing_x = cosPhi2 * x + sinPhi2 * y;
		const f32 outgoing_y = sinPhi2 * x - cosPhi2 * y;
		const vec3 outgoing_pos = outgoing_x * ux + outgoing_y * uy + mCenter;
		const vec3 outgoing_dir = cosPhi2 * ux + sinPhi2 * uy;

		ray_scattered.direction() = outgoing_dir;
		ray_scattered.origin() = outgoing_pos;
	}
	return true;
}

//======================================================
// �^���d�͏�2�i�����Čv�Z�~�X�����Ă���j
//======================================================
bool QuasiGravitationalField2::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	const f32 M = mGravityScale;
	const f32 m = 1.0f;
	const f32 v = 10.0f;

	const vec3 CP = record.pos - mCenter;
	const f32 R = CP.length();

	const vec3 OC = mCenter - ray_in.origin();
	const vec3 D = ray_in.direction();
	const f32 ray_center_dist = (D * (dot(OC, D) / D.lengthSquared()) - OC).length();

	const f32 E = 0.5f * m * v * v - G * M * m / R;
	const f32 L = m * v * ray_center_dist;
	const f32 R0 = L * L / (G * M);
	const f32 typical_E = L * L / (2 * R0 * R0);//�T�^�I�ȃG�l���M�[�X�P�[�����Ӗ����Ă���A���ۂ̃G�l���M�[�Ƃ͕�

	//���S��
	const f32 e = sqrtf(1.0f + E / typical_E);
	if (e < 1.0f)
	{
		attenuation = Color::Black;
		return false;
	}

	attenuation = Color::White;

	{
		const vec3 ux = -normalize(D);
		const vec3 uz = normalize(cross(ux, CP));
		const vec3 uy = cross(uz, ux);
		const f32 h = abs(dot(uz, CP));
		const f32 theta = asinf(h / R);

		const f32 phi = -(acosf(((R0 / OC.length()) - 1) / e) - theta);
		const f32 phi2 = 2 * phi;

		const f32 x = R * cos(theta);
		const f32 y = R * sin(theta);
		const f32 cosPhi2 = cos(phi2);
		const f32 sinPhi2 = sin(phi2);

		const f32 outgoing_x = cosPhi2 * x + sinPhi2 * y;
		const f32 outgoing_y = sinPhi2 * x - cosPhi2 * y;
		const vec3 outgoing_pos = outgoing_x * ux + outgoing_y * uy + mCenter;
		const vec3 outgoing_dir = cosPhi2 * ux + sinPhi2 * uy;

		ray_scattered.direction() = outgoing_dir;
		ray_scattered.origin() = outgoing_pos;
	}
	return true;
}



//======================================================
// ���U�t�H�[�h�U��
//======================================================
bool Rutherford::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	const f32 M = mGravityScale;
	const f32 m = 1.0f;
	const f32 v = 10.0f;

	const vec3 CP = record.pos - mCenter;
	const f32 R = CP.length();

	const vec3 OC = mCenter - ray_in.origin();
	const vec3 D = ray_in.direction();
	const f32 ray_center_dist = (D * (dot(OC, D) / D.lengthSquared()) - OC).length();

	const f32 E = 0.5f * m * v * v + G * M * m / R;
	const f32 L = m * v * ray_center_dist;
	const f32 R0 = L * L / (G * M);
	const f32 typical_E = L * L / (2 * R0 * R0);//�T�^�I�ȃG�l���M�[�X�P�[�����Ӗ����Ă���A���ۂ̃G�l���M�[�Ƃ͕�

	//���S��
	const f32 e = sqrtf(1.0f + E / typical_E);
	
	attenuation = Color::White;

	{
		const vec3 ux = -normalize(D);
		const vec3 uz = normalize(cross(ux, CP));
		const vec3 uy = cross(uz, ux);
		const f32 h = ray_center_dist;
		const f32 theta = asinf(h / R);

		const f32 phi = 2 * atan(G * M / (h * v * v));


		const f32 x = R * cos(theta);
		const f32 y = R * sin(theta);

		const f32 outgoing_x = -cos(phi + theta) * x - sin(phi + theta) * y;//-cos(phi + theta) * x + sin(phi + theta) * y;
		const f32 outgoing_y = sin(phi + theta) * x - cos(phi + theta) * y;//-sin(phi + theta) * x - cos(phi + theta) * y;
		const vec3 outgoing_pos = outgoing_x * ux + outgoing_y * uy + mCenter;
		const vec3 outgoing_dir = -cos(phi) * ux + sin(phi) * uy;

		ray_scattered.direction() = outgoing_dir;
		ray_scattered.origin() = outgoing_pos;
	}
	return true;
}

//======================================================
// �^�����U�t�H�[�h�U��
//======================================================
bool QuasiRutherford::scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& ray_scattered)
{
	const f32 M = mGravityScale;
	const f32 m = 1.0f;
	const f32 v = 10.0f;

	const vec3 CP = record.pos - mCenter;
	const f32 R = CP.length();

	const vec3 OC = mCenter - ray_in.origin();
	const vec3 D = ray_in.direction();
	const f32 ray_center_dist = (D * (dot(OC, D) / D.lengthSquared()) - OC).length();

	const f32 E = 0.5f * m * v * v + G * M * m / R;
	const f32 L = m * v * ray_center_dist;
	const f32 R0 = L * L / (G * M);
	const f32 typical_E = L * L / (2 * R0 * R0);//�T�^�I�ȃG�l���M�[�X�P�[�����Ӗ����Ă���A���ۂ̃G�l���M�[�Ƃ͕�

	//���S��
	const f32 e = sqrtf(1.0f + E / typical_E);

	attenuation = Color::White;

	{
		const vec3 ux = -normalize(D);
		const vec3 uz = normalize(cross(ux, CP));
		const vec3 uy = cross(uz, ux);
		const f32 h = ray_center_dist;
		const f32 theta = asinf(h / R);

		const f32 phi = 2 * atan(G * M / (h * v * v));


		const f32 x = R * cos(theta);
		const f32 y = R * sin(theta);

		const f32 outgoing_x = -(cos(phi + theta) * x + sin(phi + theta) * y);
		const f32 outgoing_y = -(sin(phi + theta) * x - cos(phi + theta) * y);
		const vec3 outgoing_pos = outgoing_x * ux + outgoing_y * uy + mCenter;
		const vec3 outgoing_dir = -cos(phi) * ux + sin(phi) * uy;

		ray_scattered.direction() = outgoing_dir;
		ray_scattered.origin() = outgoing_pos;
	}
	return true;
}
