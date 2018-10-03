#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

class sphere: public hitable {
    public:
        sphere() {}
        sphere(vec3 cent, float r) : center(cent), radius(r) {};
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        vec3 center;
        float radius;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float t = (-b - sqrt(b*b - a*c)) / a;
        if (t < t_max && t > t_min) {
            rec.t = t;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; // divisée par la radius pour normalizer (x,y,z)
            return true;
        }
        t = (-b + sqrt(b*b - a*c)) / a;
        if (t < t_max && t > t_min) {
            rec.t = t;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; // divisée par la radius pour normalizer (x,y,z)
            return true;
        }
    }
    return false;
};

#endif
