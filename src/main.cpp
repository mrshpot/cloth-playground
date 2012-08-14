/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include <cstdio>
#include <ctime>
#include <cmath>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "cloth.hpp"
#include "world.hpp"


World *g_world = NULL;
Cloth *g_cloth = NULL;

bool g_update = true;

// mouse movement
static const float angle_coeff = 0.4f;
float g_angle_x = 30.0f;
float g_angle_y = 0.0f;
bool g_mouse_down = false;
int g_mouse_ox, g_mouse_oy;

// FPS counter stuff
double g_last_fps_update = 0.0;
size_t g_num_frames = 0;
static const double update_interval = 1.0;
const char *title_format = "Cloth, %f fps";

double ptime()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

void update_fps()
{
    ++g_num_frames;
    double t = ptime();
    if (t - g_last_fps_update >= update_interval)
    {
        float fps = (float)(g_num_frames / (t - g_last_fps_update));
        char buf[255];
        snprintf(buf, sizeof(buf), title_format, fps);
        buf[sizeof(buf) - 1] = '\0';

        g_last_fps_update = t;
        g_num_frames = 0;
        glutSetWindowTitle(buf);
    }
}

void on_keyboard(unsigned char c, int, int)
{
    if (c == ' ')
    {
        g_update = !g_update;
    }
}

void on_mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_UP)
        {
            g_mouse_down = false;
        }
        else if (state == GLUT_DOWN)
        {
            g_mouse_down = true;
            g_mouse_ox = x;
            g_mouse_oy = y;
        }
    }
}

void on_mouse_move(int x, int y)
{
    if (g_mouse_down)
    {
        g_angle_y += angle_coeff * (x - g_mouse_ox);
        g_angle_x += angle_coeff * (y - g_mouse_oy);

        g_mouse_ox = x;
        g_mouse_oy = y;
    }
}

static void do_render(bool alt_color)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(g_angle_x, 1.0f, 0.0f, 0.0f);
    glRotatef(g_angle_y, 0.0f, 1.0f, 0.0f);

    if (!alt_color) {
        glColor3f(0.3f, 0.4f, 0.7f);
    } else {
        glColor3f(0.2f, 0.3f, 0.5f);
    }
    for (World::sphere_array_t::const_iterator it = g_world->spheres.begin();
         it != g_world->spheres.end(); ++it)
    {
        glPushMatrix();
        glTranslatef(it->origin.x, it->origin.y, it->origin.z);
        glutSolidSphere(it->r, 20, 20);
        glPopMatrix();
    }

    if (!alt_color) {
        glColor3f(0.4f, 0.7f, 0.8f);
    } else {
        glColor3f(0.75f, 0.3f, 0.25f);
    }
    g_cloth->draw();
}

void render()
{
    glClearColor(0.0, 0.0, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glPolygonOffset(0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    do_render(false);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonOffset(-1.0, -1.0);
    glLineWidth(2.0f);
    do_render(true);

    glutSwapBuffers();

    update_fps();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    if (height == 0) {
        height = 1;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120.0, (GLdouble)width / height, 0.01, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
}

void update()
{
    double t = ptime();

    // moving spheres
    static float sphere_angle = 0.0f;
    static float sphere_r = 0.1f;
    static bool expanding = true;
    static const float min_r = 0.4f;
    static const float max_r = 1.2f;
    static const float sphere_angle_step = 0.04f;
    static const float sphere_r_step= 0.002f;

    assert(g_world->spheres.size() >= 2);
    Sphere &sph1 = g_world->spheres[0];
    Sphere &sph2 = g_world->spheres[1];
    
    if (g_update)
    {
        sph1.origin.x = sin(sphere_angle) * sphere_r;
        sph1.origin.z = cos(sphere_angle) * sphere_r;
        sph2.origin.x = sin(sphere_angle + M_PI) * sphere_r;
        sph2.origin.z = cos(sphere_angle + M_PI) * sphere_r;

        sphere_angle += sphere_angle_step;
        while (sphere_angle >= 2 * M_PI) {
            sphere_angle -= (2 * M_PI);
        }
        
        if (expanding) {
            sphere_r += sphere_r_step;
            if (sphere_r >= max_r) {
                expanding = false;
            }
        }
        else
        {
            sphere_r -= sphere_r_step;
            if (sphere_r <= min_r) {
                expanding = true;
            }
        }
        
        g_cloth->step(0.01f);
    }
    
    glutPostRedisplay();
}

#define REQUIRE_EXTENSION(name)                             \
    if (!glewIsSupported(name)) {                           \
        fprintf(stderr, "Error: " name " not supported\n"); \
        return 1;                                           \
    }

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Cloth");
    
    glutReshapeFunc(&reshape);
    glutDisplayFunc(&render);
    glutIdleFunc(&update);
    
    glutKeyboardFunc(&on_keyboard);
    glutMouseFunc(&on_mouse);
    glutMotionFunc(&on_mouse_move);

    g_last_fps_update = ptime();
    
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Error: Could not initialize GLEW\n");
        return 1;
    }
    REQUIRE_EXTENSION("GL_ARB_vertex_buffer_object");

    g_world = new World();
    /*g_world->planes.push_back(mk_plane(glm::vec3(-1.0f, 0.5f, -1.0f),
                                       glm::vec3(-1.0f, 0.5f, 1.0f),
                                       glm::vec3(1.0f, -0.5f, 1.0f)));*/

    g_world->planes.push_back(mk_plane(glm::vec3(0.0f, -0.9f, 0.0f),
                                       glm::vec3(0.0f, -0.9f, 1.0f),
                                       glm::vec3(1.0f, -0.9f, 0.0f)));

    /*g_world->planes.push_back(mk_plane(glm::vec3(0.0f, 0.1f, 0.0f),
                                       glm::vec3(1.0f, 0.1f, 0.0f),
                                       glm::vec3(0.0f, 0.1f, 1.0f)));*/
    
    Sphere tmp_sphere;
    tmp_sphere.origin = glm::vec3(0.0f, -0.5f, 0.1f);
    tmp_sphere.r = 0.4f;
    g_world->spheres.push_back(tmp_sphere);
    g_world->spheres.push_back(tmp_sphere); // two spheres

    g_cloth = new Cloth(2.0f, 2.0f, 32, 32, *g_world);

    glutMainLoop();

    delete g_cloth;
    delete g_world;
    
    return 0;
}
