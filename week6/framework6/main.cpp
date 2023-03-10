/* Computer Graphics and Game Technology, Assignment Box2D game
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <cstdio>

#ifdef __APPLE__
#include <OpenGL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <Box2D/Box2D.h>

#include "levels.h"

unsigned int reso_x = 800, reso_y = 600; // Window size in pixels
const float world_x = 8.f, world_y = 6.f; // Level (world) size in meters

int last_time;
int frame_count;

// Information about the levels loaded from files will be available in these.
unsigned int num_levels;
level_t *levels;

b2Vec2 gravity(0.0f, -10.0f);
b2World world(gravity);
unsigned int cur_level = 0;


void points_to_b2vec2(b2Vec2* vertices, point_t* obj_verts, int num_verts)
{

    for (int i = 0; i < num_verts; i++) {
        vertices[i] = b2Vec2(obj_verts[i].x, obj_verts[i].y);
    }
}

/*
 * Load a given world, i.e. read the world from the `levels' data structure and
 * convert it into a Box2D world.
 */

b2Body* get_body(int i) {
    b2Body * B2 = world.GetBodyList();

    int n = 0;

    while(n != i) {
        B2 = B2->GetNext();
    }

    return B2;
}

void load_world(unsigned int level)
{
    if (level >= num_levels)
    {
        // Note that level is unsigned but we still use %d so -1 is shown as
        // such.
        printf("Warning: level %d does not exist.\n", level);
        return;
    }

    // Create a Box2D world and populate it with all bodies for this level
    // (including the ball).

    level_t cur_lvl = levels[level];

    cur_level = level + 1;

    for (uint i = 0; i < cur_lvl.num_polygons; i++) {
        poly_t obj = cur_lvl.polygons[i];

        // A dynamic box
        b2BodyDef bodyDef;

        if (obj.is_dynamic) {
            bodyDef.type = b2_dynamicBody;
        }
        else {
            bodyDef.type = b2_staticBody;
        }

        bodyDef.position.Set(obj.position.x, obj.position.y);
        b2Body* body2 = world.CreateBody(&bodyDef);

        b2PolygonShape dynamicBox;
        // Surely no more than 50 points are needed in a polygon.
        b2Vec2 vertices[100];
        points_to_b2vec2(vertices, obj.verts, obj.num_verts);

        //if (obj.is_dynamic) {
        //    for(int k = 0; k<100; k+=2) {
        //        vertices[k] += obj.position.x;
        //        vertices[k+1] += obj.position.y;
        //    }
        //}

        dynamicBox.Set(vertices, obj.num_verts);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;

        body2->CreateFixture(&fixtureDef);
    }

    for (uint i = 0; i < cur_lvl.num_joints; i++) {
        joint_t obj = cur_lvl.joints[i];

        if(obj.joint_type == JOINT_REVOLUTE) {

            b2RevoluteJointDef revoluteJointDef;

            // revoluteJointDef.Initialize(get_body(obj.objectA), get_body(obj.objectB), b2Vec2 (obj.anchor.x, obj.anchor.y));

            // b2Joint* joint = world.CreateJoint( &revoluteJointDef );
        }

        else if (obj.joint_type == JOINT_PULLEY) {
            
        }

    }



    // A dynamic box
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(cur_lvl.start.x, cur_lvl.start.y);
    b2Body* body = world.CreateBody(&bodyDef);

    // b2PolygonShape dynamicBox;
    // dynamicBox.SetAsBox(0.4f, 0.4f);

    b2CircleShape dynamicBox;
    dynamicBox.m_radius = 0.2;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    body->CreateFixture(&fixtureDef);


    // // Simulating the world
    // float timeStep = 1.0f / 60.0f;
    // int32 velocityIterations = 6;
    // int32 positionIterations = 2;

    // // Er hoort hier geen loop....
    // for (int32 i = 0; i < 60; ++i)
    // {
    //     world.Step(timeStep, velocityIterations, positionIterations);
    //     b2Vec2 position = body->GetPosition();
    //     float angle = body->GetAngle();
    //     printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
    // }

}

void get_sphere_coor(float* buff, float mid_x, float mid_y, float r, int i) {
    // The radius of the sphere.
    float phi = 2 * M_PI * i / 20 ;

    buff[2 * (i + 1)] = mid_x + r  * cos(phi); // x
    buff[2 * (i + 1) + 1] = mid_y + r * sin(phi); // y
}


void createSphere(float mid_x, float mid_y, float r) {
    GLfloat buff[44];
    buff[0] = mid_x;
    buff[1] = mid_y;

    // Calculate the point of the sphere.
    for (int i = 0; i <= 20; i += 1) {
        get_sphere_coor(buff, mid_x, mid_y, r, i);
    }

    glColor3f( 1, 0, 1 );
    // activate and specify pointer to vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, buff);
    // draw a triangle
    glDrawArrays(GL_TRIANGLE_FAN, 0, 22);
    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);
}


void createPoly(b2PolygonShape* poly)
{
    GLfloat buff[10000];
    int count = poly->GetVertexCount();


    // Calculate the point of the sphere.
    for (int i = 0; i < count; i += 1) {
        buff[2 * i] = poly->GetVertex(i).x;
        buff[2 * i + 1] = poly->GetVertex(i).y;
    }

    glColor3f( 0, 0.7, 0 );
    // activate and specify pointer to vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, buff);
    // draw a poly
    glDrawArrays(GL_TRIANGLE_FAN, 0, poly->GetVertexCount());
    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);

}



/*
 * Called when we should redraw the scene (i.e. every frame).
 * It will show the current framerate in the window title.
 */
void draw(void)
{
    int time = glutGet(GLUT_ELAPSED_TIME);
    int frametime = time - last_time;
    frame_count++;

    // Clear the buffer
    glColor3f(0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);


    //
    // Do any logic and drawing here.
    //

    float timeStep = 1.0f / 1000.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    world.Step(timeStep, velocityIterations, positionIterations);

    // b2Vec2 position = body->GetPosition();
    // float angle = body->GetAngle();
    // printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
    // createSphere(position.x, position.y, 0.4);



    b2Body * B = world.GetBodyList();
    while(B != NULL)
    {
        b2Fixture* F = B->GetFixtureList();
        while(F != NULL)
        {
            switch (F->GetType())
            {
                case b2Shape::e_circle:
                {
                    b2CircleShape* circle = (b2CircleShape*) F->GetShape();
                    /* Do stuff with a circle shape */
                    createSphere(B->GetPosition().x, B->GetPosition().y, circle->m_radius);

                    // levels[cur_level].end.x
                    // levels[cur_level].end.y
                }
                break;

                case b2Shape::e_polygon:
                {
                    b2PolygonShape* poly = (b2PolygonShape*) F->GetShape();
                    /* Do stuff with a polygon shape */
                    createPoly(poly);
                }
                break;
            }
            F = F->GetNext();
        }

        B = B->GetNext();
    }


    glColor3f( 0, 0, 1 ); // blue
    // glColor3f( 0, 1, 0 ); // green
    // glColor3f( 1, 0, 0 ); // red


    // niet meer zelf
    // Show rendered frame
    glutSwapBuffers();

    // Display fps in window title.
    if (frametime >= 1000)
    {
        char window_title[128];
        snprintf(window_title, 128,
                "Box2D: %f fps, level %d/%d",
                frame_count / (frametime / 1000.f), cur_level, num_levels);
        glutSetWindowTitle(window_title);
        last_time = time;
        frame_count = 0;
    }
}

/*
 * Called when window is resized. We inform OpenGL about this, and save this
 * for future reference.
 */
void resize_window(int width, int height)
{
    glViewport(0, 0, width, height);
    reso_x = width;
    reso_y = height;
}

/*
 * Called when the user presses a key.
 */
void key_pressed(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // Esc
        case 'q':
            exit(0);
            break;
        // Add any keys you want to use, either for debugging or gameplay.
        default:
            break;
    }
}

/*
 * Called when the user clicked (or released) a mouse buttons inside the window.
 */
void mouse_clicked(int button, int state, int x, int y)
{

}

/*
 * Called when the mouse is moved to a certain given position.
 */
void mouse_moved(int x, int y)
{

}


int main(int argc, char **argv)
{
    // Create an OpenGL context and a GLUT window.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(reso_x, reso_y);
    glutCreateWindow("Box2D");

    glewInit();

    // Bind all GLUT events do callback function.
    glutDisplayFunc(&draw);
    glutIdleFunc(&draw);
    glutReshapeFunc(&resize_window);
    glutKeyboardFunc(&key_pressed);
    glutMouseFunc(&mouse_clicked);
    glutMotionFunc(&mouse_moved);
    glutPassiveMotionFunc(&mouse_moved);

    // Initialise the matrices so we have an orthogonal world with the same size
    // as the levels, and no other transformations.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, world_x, 0, world_y, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Read the levels into a bunch of structs.
    num_levels = load_levels(&levels);
    printf("Loaded %d levels.\n", num_levels);

    // Load the first level (i.e. create all Box2D stuff).
    load_world(3);

    last_time = glutGet(GLUT_ELAPSED_TIME);
    frame_count = 0;
    glutMainLoop();

    return 0;
}
