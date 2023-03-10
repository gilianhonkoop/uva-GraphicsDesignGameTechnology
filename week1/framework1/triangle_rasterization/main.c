/* Computer Graphics assignment, Triangle Rasterization
 *
 * Created by ...... Paul Melis
 */

#ifdef __APPLE__
#include <OpenGL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "trirast.h"

#include "triangles.h"

// Number of drawable pixels, i.e. x coordinates passed to PutPixel()
// should be in the range [0, framebuffer_width[.  Analogous for y.
// (These values must both be a power of 2)
const int   framebuffer_width = 128;
const int   framebuffer_height = 64;

const int   zoomed_pixel_size = 7;

int     screen_width, screen_height;
int     draw_optimized = 0;
int     zoom = 1;
int     scene = 1;
int     draw_corners = 0;
int     color_by_putpixel_count = 0;

byte    *framebuffer;

GLfloat quadVert[8] = {
    0.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0
};

GLfloat quadTex[8] = {
    0.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0
};

GLuint quadInd[6] = {
    0, 1, 2,
    0, 2, 3
};

GLuint vertBuffers;
GLuint indBuffers;
GLuint texBuffer;
GLuint texCoords;

GLuint triVerts;
GLuint *triInds;

void
InitOpenGL(void)
{
    // Set the background color
    glClearColor(0., 0., 0., 0.);

    // Allocate a framebuffer, to be filled during triangle rasterization
    framebuffer = (byte*)malloc(framebuffer_width*framebuffer_height*3);

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    // Generate required buffers
    glGenBuffers(1, &vertBuffers);
    glGenBuffers(1, &indBuffers);
    glGenBuffers(1, &texCoords);
    glGenTextures(1, &texBuffer);

    // Scale the quad to framebuffer size.
    quadVert[1] = framebuffer_height;
    quadVert[4] = framebuffer_width;
    quadVert[7] = framebuffer_height;
    quadVert[6] = framebuffer_width;
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffers);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), quadVert, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, texCoords);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), quadTex, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuffers);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), quadInd, GL_STATIC_DRAW);

    glBindTexture(GL_TEXTURE_2D, texBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, framebuffer_width, framebuffer_height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);


    // Setup texturing state (as we display the rasterization framebuffer
    // using a textured quad)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Setup buffers for OpenGL drawing of the triangles.
    int numTriangles = sizeof(triangles) / sizeof(struct triangle);

    // Put all triangle vertices in one buffer.
    triInds = (GLuint*) malloc(numTriangles * sizeof(GLuint));
    glGenBuffers(1, &triVerts);
    glGenBuffers(numTriangles, triInds);

    GLfloat verts[2 * sizeof(vertices) / sizeof(struct vertex)];
    for(unsigned int i = 0; i < sizeof(vertices) / sizeof(struct vertex); i++) {
        verts[2 * i] = vertices[i].x;
        verts[2 * i + 1] = vertices[i].y;
    }

    glBindBuffer(GL_ARRAY_BUFFER, triVerts);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Setup the indices to draw the various triangles.
    GLuint inds[3];
    for(int i = 0; i < numTriangles; i++) {
        inds[0] = triangles[i].i;
        inds[1] = triangles[i].j;
        inds[2] = triangles[i].k;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triInds[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLuint), inds, GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PutPixel(int x, int y, byte r, byte g, byte b)
{
    if (x < 0 || y < 0 || x >= framebuffer_width || y >= framebuffer_height)
    {
        printf("PutPixel(): x, y coordinates (%d, %d) outside of visible area!\n",
                x, y);
        return;
    }


    if (color_by_putpixel_count) {
        if (framebuffer[3*(framebuffer_width*y+x)] == 128) {
            framebuffer[3*(framebuffer_width*y+x)] = 255;
        }
        else {
            framebuffer[3*(framebuffer_width*y+x)] = 128;
        }
    }

    else {
        framebuffer[3*(framebuffer_width*y+x)] = r;
        framebuffer[3*(framebuffer_width*y+x)+1] = g;
        framebuffer[3*(framebuffer_width*y+x)+2] = b;
    }

    // The pixels in framebuffer[] are layed out sequentially,
    // with the R, G and B values one after the other, e.g
    // RGBRGBRGB...
}

void
DrawTriangles(void)
{
    struct  triangle tri;
    for (unsigned int t = 0; t < sizeof(triangles)/sizeof(struct triangle); t++)
    {
        tri = triangles[t];

        if (draw_optimized)
        {
            /* draw the triangle with the given color */
            draw_triangle_optimized(
                vertices[tri.i].x, vertices[tri.i].y,
                vertices[tri.j].x, vertices[tri.j].y,
                vertices[tri.k].x, vertices[tri.k].y,
                colors[tri.c].r, colors[tri.c].g, colors[tri.c].b);

        }
        else
        {
            /* draw the triangle with the given color */
            draw_triangle(
                vertices[tri.i].x, vertices[tri.i].y,
                vertices[tri.j].x, vertices[tri.j].y,
                vertices[tri.k].x, vertices[tri.k].y,
                colors[tri.c].r, colors[tri.c].g, colors[tri.c].b);
        }

        if (draw_corners)
        {
            PutPixel(vertices[tri.i].x, vertices[tri.i].y, 255, 255, 255);
            PutPixel(vertices[tri.j].x, vertices[tri.j].y, 255, 255, 255);
            PutPixel(vertices[tri.k].x, vertices[tri.k].y, 255, 255, 255);
        }
    }
}

void
DrawTrianglesOpenGL(void)
{
    struct triangle tri;

    glDisable(GL_TEXTURE_2D);

    glBindBuffer(GL_ARRAY_BUFFER, triVerts);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    for (unsigned int t = 0; t < sizeof(triangles)/sizeof(struct triangle); t++)
    {
        tri = triangles[t];
        /* Bind the buffer and draw the triangle with the given color. */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triInds[t]);
        glColor3ub(colors[tri.c].r, colors[tri.c].g, colors[tri.c].b);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    }

    if (draw_corners)
    {
        glColor3ub(255, 255, 255);
        for (unsigned int t = 0; t < sizeof(triangles)/sizeof(struct triangle); t++)
        {
            // Draw white points on the corners of the triangle.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triInds[t]);
            glDrawElements(GL_POINTS, 3, GL_UNSIGNED_INT, 0);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
TestRasterizationSpeed(void)
{
    const int N = 1000;

    struct timeval  t0, t1;
    double          diff;

    //srand(123456);

    gettimeofday(&t0, NULL);

    if (draw_optimized)
    {
        for (int t = 0; t < N; t++)
        {
            draw_triangle_optimized(
                rand()%framebuffer_width, rand()%framebuffer_height,
                rand()%framebuffer_width, rand()%framebuffer_height,
                rand()%framebuffer_width, rand()%framebuffer_height,
                colors[t%6].r, colors[t%6].g, colors[t%6].b);
        }
    }
    else
    {
        for (int t = 0; t < N; t++)
        {
            draw_triangle(
                rand()%framebuffer_width, rand()%framebuffer_height,
                rand()%framebuffer_width, rand()%framebuffer_height,
                rand()%framebuffer_width, rand()%framebuffer_height,
                colors[t%6].r, colors[t%6].g, colors[t%6].b);
        }
    }

    gettimeofday(&t1, NULL);

    /* calculate time used */
    diff = t1.tv_sec - t0.tv_sec + (t1.tv_usec - t0.tv_usec)*1.0e-6;

    printf("%d triangles in %.6f seconds, %.1f triangles/sec\n", N, diff, N/diff);
}

void
DrawScene(void)
{

    /* clear the draw buffer */
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // clear the rasterization framebuffer
    memset(framebuffer, 0, 3*framebuffer_width*framebuffer_height);

    if (scene == 1)
        DrawTriangles();
    else if (scene == 2)
        TestRasterizationSpeed();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);

    if (scene != 3)
    {
        if (zoom)
            glOrtho(0, framebuffer_width, 0, framebuffer_height, -1, 1);
        else
            glOrtho(0, screen_width, 0, screen_height, -1, 1);

        // Copy the framebuffer to the texture.
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texBuffer);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, framebuffer_width,
                        framebuffer_height, GL_RGB, GL_UNSIGNED_BYTE,
                        framebuffer);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // Set Texture coordinates.
        glBindBuffer(GL_ARRAY_BUFFER, texCoords);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);

        // Set Vertex coordinates.
        glBindBuffer(GL_ARRAY_BUFFER, vertBuffers);
        glVertexPointer(2, GL_FLOAT, 0, 0);

        // Draw textured quad.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuffers);
        glColor3f(1, 1, 1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        if (zoom)
            glOrtho(-0.5, framebuffer_width-0.5, -0.5, framebuffer_height-0.5, -1, 1);
        else
            glOrtho(-0.5, screen_width-0.5, -0.5, screen_height-0.5, -1, 1);

        DrawTrianglesOpenGL();
    }
    glDisableClientState(GL_VERTEX_ARRAY);

    // finally, swap the draw buffers to make the triangles appear on screen
    glutSwapBuffers();
}

void
KeyPressed(unsigned char key, int x, int y)
{
    (void) x;
    (void) y;
    switch (key)
    {
        case '1':
        case '2':
        case '3':
        {
            scene = key - '0';
            glutPostRedisplay();
            break;
        }
        case 'o':
        {
            draw_optimized = 1 - draw_optimized;
            printf("draw_optimized set to %d\n", draw_optimized);
            glutPostRedisplay();
            break;
        }
        case 'z':
        {
            zoom = 1 - zoom;
            glutPostRedisplay();
            break;
        }
        case 'd':
        {
            // debug
            color_by_putpixel_count = 1 - color_by_putpixel_count;
            printf("color_by_putpixel_count set to %d\n", color_by_putpixel_count);
            glutPostRedisplay();
            break;
        }
        case 'c':
        {
            // triangle corners
            draw_corners = 1 - draw_corners;
            glutPostRedisplay();
            break;
        }
        case 'q':
            exit(0);
    }
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    screen_width = framebuffer_width * zoomed_pixel_size;
    screen_height = framebuffer_height * zoomed_pixel_size;
    glutInitWindowSize(screen_width, screen_height);

    glutInitWindowPosition(20, 20);
    glutCreateWindow("Triangle rasterization");

    glewInit();

    glutDisplayFunc(&DrawScene);
    glutIdleFunc(&DrawScene);
    glutKeyboardFunc(&KeyPressed);

    InitOpenGL();

    glutMainLoop();

    return 1;
}
