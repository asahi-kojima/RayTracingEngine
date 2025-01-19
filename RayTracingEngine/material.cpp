#include "material.h"
#include "hittable.h"



bool Metal::scatter(const Ray& ray_in, const HitRecord& record, vec3& attenuation, Ray& ray_scattered)
{
    //�V���ȃ��C���Z�b�g
    vec3 reflected_ray = reflect(ray_in.direction(), record.normal);

    //�Ō��0.1f�ɂ��Ă�reflected_ray�̒������P�����A����Ɠ����I�[�_�[�ɂ���Ƌ��E�Ŗ������˂��N����̂ŁA�����h�����߁B
    ray_scattered = Ray(record.pos, reflected_ray + fuzz * random_in_unit_sphere() * 0.1f);
    
    //�\�ʐF���Z�b�g
    attenuation = albedo;

    return (dot(ray_scattered.direction(), record.normal) > 0);
}
