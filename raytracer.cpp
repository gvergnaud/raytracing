#include <iostream>
#include "float.h"
#include "ray.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

vec3 background(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        }

        return vec3(0, 0, 0);
    }
    return background(r);

}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n + 1];
    list[0] = new sphere(
        vec3(0, -1000, 0),
        1000,
        new lambertian(vec3(0.5, 0.5, 0.5))
    );
    int i = 1;

    for (int a = -4; a < 4; a++) {
        for (int b = -4; b < 4; b++) {
            float choose_mat = drand48();
            vec3 center(a + 9.0 * drand48(), 0.2, b + 0.9 * drand48());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) { // diffuse
                    list[i++] = new sphere(
                        center,
                        0.2,
                        new lambertian(
                            vec3(
                                drand48()*drand48(),
                                drand48()*drand48(),
                                drand48()*drand48()
                            )
                        )
                    );
                } else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(
                        center,
                        0.2,
                        new metal(
                            vec3(
                                0.5 * (1 + drand48()),
                                0.5 * (1 + drand48()),
                                0.5 * (1 + drand48())
                            ),
                            0.5 * drand48()
                        )
                    );
                } else {
                    list[i++] = new sphere(
                        center,
                        0.2,
                        new dielectric(1.5)
                    );
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(
        vec3(-4, 1, 0),
        1.0,
        new lambertian(vec3(0.4, 0.2, 0.1))
    );
    list[i++] = new sphere(
        vec3(4, 1, 0),
        1.0,
        new metal(vec3(0.7, 0.6, 0.5), 0.0)
    );

    return new hitable_list(list, i);
}

int main() {
    int nx = 500;
    int ny = 300;
    int ns = 150;

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";


    hitable *world = random_scene();

    camera cam(
        vec3(7, 1.5, 2),
        vec3(0, 0, -1),
        vec3(0, 1, 0),
        35,
        float(nx) / float(ny),
        0.05
    );


    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {

            vec3 col(0, 0, 0);

            for (int s = 0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world, 0);
            }

            col /= float(ns);

            // gamma correction. "gamma 2",
            // c'est à dire x ^ (1/gamma),
            // soit x ^ (1/2) == sqrt(x)
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

            int ir = int(255.99 * col.r());
            int ig = int(255.99 * col.g());
            int ib = int(255.99 * col.b());

            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
