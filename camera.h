#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0 * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1);
    return p;
}

class camera {
    public:
        // vfov : top to bottom field of view in degrees
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture) {
            lens_radius = aperture / 2;
            float focus_dist = (lookfrom - lookat).length();

            float theta = vfov * M_PI / 180;
            float half_height = tan(theta / 2);
            float half_width = aspect * half_height;

            // w est le vecteur qui part de lookat et qui arrive à lookfrom (à la camera)
            w = unit_vector(lookfrom - lookat);
            // u est le vecteur unitaire de l'axe x.
            // on le calcul grace au cross product entre la direciton vers le haut de la camera (en général, c'est le haut de la scene (0, 1, 0)) et w.
            // le cross product permet de determiner le vecteur perpendiculaire au plan formé par les deux vecteur, dont la longueur est l'air du quadrilataire formé par les vecteurs, et le signe est positif si l'angle entre le premier vecteur et le second est inferieur à PI (% 2PI) (tricks de la main droite)
            u = unit_vector(cross(vup, w));
            // quand on a deux vecteur i et j unitaires, on peut calculer le vecteur k de la troisième dimension avec le cross product entre j et i (cross(j, i))
            v = cross(w, u);

            origin = lookfrom;
            lower_left_corner =
                origin -
                focus_dist * (
                    half_width * u + half_height * v + w
                );
            horizontal = 2 * half_width * focus_dist * u;
            vertical = 2 * half_height * focus_dist * v;
        }

        ray get_ray(float s, float t) {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            return ray(
                origin + offset,
                lower_left_corner
                    + s*horizontal
                    + t*vertical
                    - origin
                    - offset
            );
        }

        vec3 u, v, w;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 origin;
        float lens_radius;
};

#endif
