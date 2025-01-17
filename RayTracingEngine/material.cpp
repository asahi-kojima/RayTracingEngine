#include "material.h"
#include "hittable.h"



bool Metal::scatter(const Ray& ray_in, const HitRecord& record, vec3& attenuation, Ray& ray_scattered)
{
    //新たなレイをセット
    vec3 reflected_ray = reflect(normalize(ray_in.direction()), record.normal);
    ray_scattered = Ray(record.pos, reflected_ray + fuzz * random_in_unit_sphere());
    
    //表面色をセット
    attenuation = albedo;

    return (dot(ray_scattered.direction(), record.normal) > 0);
}
