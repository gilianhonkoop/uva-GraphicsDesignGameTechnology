/* Computer Graphics and Game Technology, Assignment Ray-tracing
 *
 * Student name .... Jan Deen, Gilian Honkoop
 * Student email ...
 * Collegekaart .... 12873551, 13710729
 * Date ............ 7-10-2022
 * Comments ........ Multiple ways to calculate the color of an intersection point.
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "shaders.h"
#include "perlin.h"
#include "v3math.h"
#include "intersection.h"
#include "scene.h"
#include "quat.h"
#include "constants.h"

// shade_constant()
//
// Always return the same color. This shader does no real computations
// based on normal, light position, etc. As such, it merely creates
// a "silhouette" of an object.

vec3
shade_constant(intersection_point ip)
{
    return v3_create(1, 0, 0);
}


vec3
shade_matte(intersection_point ip)
{
    float sum = 0;
    for (int i = 0; i < scene_num_lights; i++) {
        // Calculate for each light what its contribution is.
        light cur_light = scene_lights[i];
        vec3 l_i = v3_normalize(v3_subtract(cur_light.position, ip.p));
        // Add an offset to ip.p to prevent black dot froms appearing.
        vec3 offset = v3_multiply(ip.n, 0.0001);
        vec3 origin_with_offset = v3_add(ip.p, offset);

        if (!shadow_check(origin_with_offset, l_i)) {
            float contribution = v3_dotprod(ip.n, l_i) * cur_light.intensity;

            // Add the contribution to the total sum of contributions,
            // Negative contributions will be ignored.
            if (contribution > 0) {
                sum += contribution;
            }
        }
    }

    // Add the ambiant light.
    sum += scene_ambient_light;
    if (sum > 1) {
        sum = 1;
    }

    return v3_create(sum, sum, sum);
}


vec3
shade_blinn_phong(intersection_point ip)
{
    float k_d = 0.8;
    float k_s = 0.5;
    float alpha = 50;
    vec3 c_d = v3_create(1.0, 0.0, 0.0);
    vec3 c_s = v3_create(1.0, 1.0, 1.0);

    float sum1 = 0;
    float sum2 = 0;
    for (int i = 0; i < scene_num_lights; i++) {
        // Calculate for each light what its contribution is.
        light cur_light = scene_lights[i];
        vec3 l_i = v3_normalize(v3_subtract(cur_light.position, ip.p));
        // Add an offset to ip.p to prevent black dot froms appearing.
        vec3 offset = v3_multiply(ip.n, 0.0001);
        vec3 origin_with_offset = v3_add(ip.p, offset);

        if (!shadow_check(origin_with_offset, l_i)) {
            float contribution = v3_dotprod(ip.n, l_i) * cur_light.intensity;

            // Negative contributions will be ignored.
            if (contribution > 0) {
                sum1 += contribution;
            }

            // Calculate the vector h, halfway between i and l_i.
            vec3 h = v3_add(ip.i, l_i);
            h = v3_normalize(v3_multiply(h, 0.5));
            sum2 += cur_light.intensity * powf(v3_dotprod(ip.n, h), alpha);
        }
    }

    sum1 = sum1 * k_d;
    sum2 = sum2 * k_s;
    // Add the ambiant light.
    sum1 += scene_ambient_light;

    vec3 c_f = v3_add(v3_multiply(c_d, sum1), v3_multiply(c_s, sum2));
    return c_f;
}


vec3
shade_reflection(intersection_point ip)
{
    // 75% consists of matte shading.
    vec3 matte = shade_matte(ip);
    vec3 sum = v3_multiply(matte, 0.75);

    // Calculated the reflected direction.
    vec3 r = v3_multiply(ip.n, v3_dotprod(ip.i, ip.n) * 2);
    r = v3_subtract(r, ip.i);

    // Add an offset to ip.p to prevent black dot froms appearing.
    vec3 offset = v3_multiply(ip.n, 0.0001);
    vec3 origin_with_offset = v3_add(ip.p, offset);
    sum = v3_add(sum, v3_multiply(ray_color(ip.ray_level + 1, origin_with_offset, r), 0.25));

    return sum;
}


// Returns the shaded color for the given point to shade.
// Calls the relevant shading function based on the material index.
vec3
shade(intersection_point ip)
{
  switch (ip.material)
  {
    case 0:
      return shade_constant(ip);
    case 1:
      return shade_matte(ip);
    case 2:
      return shade_blinn_phong(ip);
    case 3:
      return shade_reflection(ip);
    default:
      return shade_constant(ip);

  }
}


// Determine the surface color for the first object intersected by
// the given ray, or return the scene background color when no
// intersection is found
vec3
ray_color(int level, vec3 ray_origin, vec3 ray_direction)
{
    intersection_point  ip;

    // If this ray has been reflected too many times, simply
    // return the background color.
    if (level >= 3)
        return scene_background_color;

    // Check if the ray intersects anything in the scene
    if (find_first_intersection(&ip, ray_origin, ray_direction))
    {
        // Shade the found intersection point
        ip.ray_level = level;
        return shade(ip);
    }

    // Nothing was hit, return background color
    return scene_background_color;
}
