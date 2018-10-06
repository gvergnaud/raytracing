#ifndef MATERIALH
#define MATERIALH

struct hit_record;

#include "ray.h"
#include "hitable.h"

vec3 random_point_in_unit_sphere() {
    vec3 p;
    do {
        p = 2 * vec3(drand48(), drand48(), drand48()) - 1.0;
    } while (p.squared_length() > 1.0);
    return p;
}

// retours la reflexivité en fonction de l'angle avec l'element
float schlick(float cosine, float refraction_index) {
    float r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

// direction du vecteur réflechit par rapport à une normal `n`.
vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    return false;
}


class material {
    public:
        virtual bool scatter(
            const ray& r_in,
            const hit_record& rec,
            vec3& attenuation,
            ray& scattered
        ) const = 0;
};

class lambertian : public material {
    public:
        lambertian(const vec3& a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in,
            const hit_record& rec,
            vec3& attenuation,
            ray& scattered
        ) const {
            // la target est le point vers lequel point le ray après avoir rebondi.
            // on part du point rect.p ou a eu lieu le hit, on ajoute la normal, ça
            // nous donne un point à une distance de 1 dans la direction ou il aurait rebondi
            // si la surface avait été parfaitement droite (glossy, en mode reflexion, pas diffusion),
            // et on ajoute un point random dans une unit sphere (c'est à dire une sphere de rayon 1)
            // qui se trouve donc à la surface de l'object que l'on a touché et qui permet de
            // simuler la roughness, l'aspect aléatoire du rebond sur l'object.
            vec3 target = rec.p + rec.normal + random_point_in_unit_sphere();

            // on crée un ray du point d'impact vers la target
            scattered = ray(rec.p, target - rec.p);

            // le facteur "albedo" défini la luminosité de notre object en r, g et b
            attenuation = albedo;

            return true;
        }

        vec3 albedo;
};

class metal : public material {
    public:
        metal(const vec3& a, float f) : albedo(a) {
            if (f < 1) fuzz = f;
            else fuzz = 1;
        }

        virtual bool scatter(
            const ray& r_in,
            const hit_record& rec,
            vec3& attenuation,
            ray& scattered
        ) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);

            scattered = ray(rec.p, reflected + fuzz * random_point_in_unit_sphere());
            attenuation = albedo;
            // seulement si le la ray est reflehie dans le sens de la normale.
            return (dot(scattered.direction(), rec.normal) > 0);
        }

        vec3 albedo;
        float fuzz;
};

class dielectric : public material {
    public:
        dielectric (float ri) : refraction_index(ri) {}

        virtual bool scatter(
            const ray& r_in,
            const hit_record& rec,
            vec3& attenuation,
            ray& scattered
        ) const {
            attenuation = vec3(1.0, 1.0, 1.0);

            vec3 outward_normal;
            float ni_over_nt;
            float cosine;
            if (dot(r_in.direction(), rec.normal) > 0) {
                outward_normal = -rec.normal;
                ni_over_nt = refraction_index;
                cosine = refraction_index * dot(r_in.direction(), rec.normal);
            } else {
                outward_normal = rec.normal;
                ni_over_nt = 1.0 / refraction_index;
                cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
            }

            vec3 refracted;
            float reflect_prob;
            if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
                reflect_prob = schlick(cosine, refraction_index);
            } else {
                reflect_prob = 1.0;
            }

            if(drand48() < reflect_prob) {
                vec3 reflected = reflect(r_in.direction(), rec.normal);
                scattered = ray(rec.p, reflected);
            } else {
                scattered = ray(rec.p, refracted);
            }

            return true;
        }

        float refraction_index;
};

#endif
