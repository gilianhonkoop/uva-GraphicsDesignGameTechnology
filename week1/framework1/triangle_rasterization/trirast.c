/* Computer Graphics assignment, Triangle Rasterization
 * Filename ........ trirast.c
 * Description ..... Implements triangle rasterization
 * Created by ...... Paul Melis
 *
 * Student 1 name: Jan Deen
 * Student 1 id  : 12873551
 * Student 2 name: Gilian Honkoop
 * Student 2 id  : 13710729
 * Date          : 9-9-2022
 * Comments      : The optimized results are a bit different because
 *                 of small error that occure when adding multiple
 *                 floats together.
 *
 * (always fill in these fields before submitting!!)
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "types.h"

/*
 * Rasterize a single triangle.
 * The triangle is specified by its corner coordinates
 * (x0,y0), (x1,y1) and (x2,y2).
 * The triangle is drawn in color (r,g,b).
 */

int getSmallest(int a, int b, int c) {
    int smallest = a;

    if (b < smallest) {
        smallest = b;
    }
    if (c < smallest) {
        smallest = c;
    }

    return smallest;
}

float f01(float x, float y, float x0, float y0, float x1, float y1) {
    return (y0-y1)*x + (x1-x0)*y + x0*y1 - x1*y0;
}

float f12(float x, float y, float x1, float y1, float x2, float y2) {
    return (y1-y2)*x + (x2-x1)*y + x1*y2 - x2*y1;
}

float f20(float x, float y, float x0, float y0, float x2, float y2) {
    return (y2-y0)*x + (x0-x2)*y + x2*y0 - x0*y2;
}

void
draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{

    int xmin = getSmallest(x0, x1, x2);
    int ymin = getSmallest(y0, y1, y2);
    int xmax = -getSmallest(-x0, -x1, -x2);
    int ymax = -getSmallest(-y0, -y1, -y2);

    float fa = f12(x0, y0, x1, y1, x2, y2);
    float fb = f20(x1,y1, x0, y0, x2, y2);
    float fg = f01(x2, y2, x0, y0, x1, y1);


    for (int y = ymin; y != ymax + 1; y++) {
        for (int x = xmin; x != xmax + 1; x++) {
            float alpha = f12(x, y, x1, y1, x2, y2) / fa;
            float beta = f20(x, y, x0, y0, x2, y2) / fb;
            float gamma = f01(x, y, x0, y0, x1, y1) / fg;

            // Print the point if it is in the trangle.
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                if ((alpha > 0 || fa*f12(-1,-1, x1, y1, x2, y2) > 0) &&
                (beta > 0 || fb*f20(-1,-1, x0, y0, x2, y2) > 0) &&
                (gamma > 0 || fg*f01(-1,-1, x0, y0, x1, y1) > 0)) {
                    PutPixel(x, y, r, g, b);
                }

            }
        }
    }
}

void
draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
    int xmin = getSmallest(x0, x1, x2);
    int ymin = getSmallest(y0, y1, y2);
    int xmax = -getSmallest(-x0, -x1, -x2);
    int ymax = -getSmallest(-y0, -y1, -y2);

    double fa = f12(x0, y0, x1, y1, x2, y2);
    double fb = f20(x1,y1, x0, y0, x2, y2);
    double fg = f01(x2, y2, x0, y0, x1, y1);

    // Calculate how much alpha, beta and gamma change when increasing
    // the x- or y-coordinate by 1.
    float d_alpha_x = (f12(xmin+1, ymin, x1, y1, x2, y2) / fa) - (f12(xmin, ymin, x1, y1, x2, y2) / fa);
    float d_alpha_y = (f12(xmin, ymin+1, x1, y1, x2, y2) / fa) - (f12(xmin, ymin, x1, y1, x2, y2) / fa);
    float d_beta_x = (f20(xmin+1, ymin, x0, y0, x2, y2) / fb) - (f20(xmin, ymin, x0, y0, x2, y2) / fb);
    float d_beta_y = (f20(xmin, ymin+1, x0, y0, x2, y2) / fb) - (f20(xmin, ymin, x0, y0, x2, y2) / fb);
    float d_gamma_x = (f01(xmin+1, ymin, x0, y0, x1, y1) / fg) - (f01(xmin, ymin, x0, y0, x1, y1) / fg);
    float d_gamma_y = (f01(xmin, ymin+1, x0, y0, x1, y1) / fg) - (f01(xmin, ymin, x0, y0, x1, y1) / fg);

    // subtract 1 from the x and y values to draw edges since alpha is 
    // incremented immediately in the for loop
    float alpha = f12(xmin-1, ymin-1, x1, y1, x2, y2) / fa;
    float beta = f20(xmin-1, ymin-1, x0, y0, x2, y2) / fb;
    float gamma = f01(xmin-1, ymin-1, x0, y0, x1, y1) / fg;

    float alpha_begin = alpha;
    float beta_begin = beta;
    float gamma_begin = gamma;

    for (int y = ymin; y != ymax + 1; y++) {
        // Set alpha, beta and gamma to values on the line x = x_min.
        alpha = alpha_begin + d_alpha_y;
        beta = beta_begin + d_beta_y;
        gamma = gamma_begin + d_gamma_y;

        alpha_begin = alpha;
        beta_begin = beta;
        gamma_begin = gamma;

        int printed = 0;

        for (int x = xmin; x != xmax + 1; x++) {

            int printing = 0;
            alpha += d_alpha_x;
            beta += d_beta_x;
            gamma += d_gamma_x;

            // Removes small inconsistencies by adding/subtracting
            // multiple floats.
            alpha = roundf(alpha * 100000) / 100000;
            beta = roundf(beta * 100000) / 100000;
            gamma = roundf(gamma * 100000) / 100000;

            // Test if the current point is in the triangle.
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                // The point will not always be printed if it is on the edge.
                if ((alpha > 0 || fa*f12(-1,-1, x1, y1, x2, y2) > 0) &&
                (beta > 0 || fb*f20(-1,-1, x0, y0, x2, y2) > 0) &&
                (gamma > 0 || fg*f01(-1,-1, x0, y0, x1, y1) > 0)) {
                    PutPixel(x, y, r, g, b);
                    printed = 1;
                    printing = 1;
                }
            }

            // Go to the next line if points were printed on this line,
            // but nothing was printed on the current x-coordinate.
            if (printed == 1 && printing == 0) {
                continue;
            }
        }
    }
}
