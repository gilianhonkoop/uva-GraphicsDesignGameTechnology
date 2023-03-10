/* Computer Graphics, Assignment, Bezier curves
 * Filename ........ bezier.c
 * Description ..... Bezier curves
 * Date ............ 22.07.2009
 * Created by ...... Paul Melis
 *
 * Student name .... Jan Deen & Gilian Honkoop
 * Student email ...  
 * Collegekaart .... 12873551 & 13710729
 * Date ............ 23/09/2022
 * Comments ........ Moves a robot arm to grab a teapot using bezier curves.
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <math.h>
#include "bezier.h"
#include <stdio.h>

/* Given a Bezier curve defined by the 'num_points' control points
 * in 'p' compute the position of the point on the curve for parameter
 * value 'u'.
 *
 * Return the x and y values of the point by setting *x and *y,
 * respectively.
 */

// Returns a factorial.
int fact(int x) {
    if (x == 1 || x == 0) {
        return 1;
    }

    return x * fact(x-1);
}

// Returns x to the power of y.
float power(float x, int y) {
    // Any number to the power 0 is 1.
    if (y == 0) {
        return 1;
    }

    float temp = 1;

    for(int i=0; i<y; i++) {
        temp = temp * x;
    }

    return temp;
}

/* Given a Bezier curve defined by the 'num_points' control points
 * in 'p' compute the position of the point on the curve for parameter
 * value 'u'.
 *
 * Return the x and y values of the point by setting *x and *y,
 * respectively.
 */

void
evaluate_bezier_curve(float *x, float *y, control_point p[], int num_points, float u)
{
    *x = 0.0;
    *y = 0.0;
    float temp;

    // For each control point calculate bernstein polynomial and calculate new x and y.
    for (int k=0; k < num_points; k++) {
        temp = fact(num_points - 1) / (fact(k) * fact(num_points-1-k)) * power((1-u), (num_points-1-k)) * pow(u, k);
        *x += temp * p[k].x;
        *y += temp * p[k].y;
    }

}

/* Draw a Bezier curve defined by the control points in p[], which
 * will contain 'num_points' points.
 *
 *
 * The 'num_segments' parameter determines the "discretization" of the Bezier
 * curve and is the number of straight line segments that should be used
 * to approximate the curve.
 *
 * Call evaluate_bezier_curve() to compute the necessary points on
 * the curve.
 *
 * You will have to create a GLfloat array of the right size, filled with the
 * vertices in the appropriate format and bind this to the buffer.
 */

void
draw_bezier_curve(int num_segments, control_point p[], int num_points)
{

    GLuint buffer[1];

    int step = 0;
    float u = 0;
    float arr[num_segments+1][2];

    // Calculate x and y for num_segments + 1 points.
    for (step = 0; step <= num_segments; step++) {
        u = (float)step/num_segments;

        evaluate_bezier_curve(&arr[step][0], &arr[step][1], p, num_points, u);
    }

    // This creates the VBO and binds an array to it.
    glGenBuffers(1, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, *buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arr) /* Fill in the right size here*/,
                 &arr /*Fill in the pointer to the array*/, GL_STATIC_DRAW);

    // This tells OpenGL to draw what is in the buffer as a Line Strip.
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_LINE_STRIP, 0, num_segments + 1/* Fill in the number of steps to be drawn*/);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, buffer);
}

/* Find the intersection of a cubic Bezier curve with the line X=x.
   Return 1 if an intersection was found and place the corresponding y
   value in *y.
   Return 0 if no intersection exists.
*/

int
intersect_cubic_bezier_curve(float *y, control_point p[], float x)
{
    float a, b;
    int num_points = 4;
    float ubottom = 0;
    float utop = 1;

    // Checks if the time line lies left of all the control points.
    evaluate_bezier_curve(&a, &b, p, num_points, 0);
    if (x < a) {
        return 0;
    }

    // Checks if the time line lies right of all the control points.
    evaluate_bezier_curve(&a, &b, p, num_points, 1);
    if (x > a) {
        return 0;
    }

    // Try different values of u, until we are close enough to the time line.
    while(fabs(a-x) > 0.001) {
        float u = (ubottom + utop) / 2;
        evaluate_bezier_curve(&a, &b, p, num_points, u);

        if (x < a) {
            utop = u;
        }
        else {
            ubottom = u;
        }
    }

    *y = b;
    return 1;
}
