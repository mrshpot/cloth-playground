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

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "w32_compat.hpp"
#include "time.hpp"
#include "cloth.hpp"
#include "world.hpp"
#include "script.hpp"
#include "surface.hpp"
#include "math_utils.hpp"


World *g_world = NULL;
Cloth *g_cloth = NULL;
Script *g_script = NULL;
Surface *g_plane_surface = NULL;

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

static const float sphere_r_bias = 1e-2;

static const float z_near = 3.0f;
static const float z_far = 30.0f;

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

void reset()
{
    Cloth &c = *g_cloth;
    c.lock();

    size_t rows = c.rows();
    size_t cols = c.cols();
    float width = c.width();
    float height = c.height();
    float width_half = width * 0.5f;
    float height_half = height * 0.5f;
    
    for (size_t i = 0; i < rows; ++i)
    {
        float fi = (float)i / (rows - 1);
        
        for (size_t j = 0; j < cols; ++j)
        {
            float fj = (float)j / (cols - 1);

            glm::vec3 &p = c.at(i, j).pos;
            p.x = fi * height - height_half;
            p.y = 1.0f;
            p.z = fj * width - width_half;
        }
    }

    c.unlock();
    c.reset_velocity();
}

void on_keyboard(unsigned char c, int, int)
{
    if (c == ' ')
    {
        g_update = !g_update;
    }
    else if (c == 'r')
    {
        reset();
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

    if (!alt_color)
        glColor3f(0.3f, 0.4f, 0.7f);
    else
        glColor3f(0.2f, 0.3f, 0.5f);

    for (World::sphere_array_t::const_iterator it = g_world->spheres.begin();
         it != g_world->spheres.end(); ++it)
    {
        Sphere *sp = *it;
        glPushMatrix();
        glTranslatef(sp->origin.x, sp->origin.y, sp->origin.z);
        glutSolidSphere(sp->r - sphere_r_bias, 20, 20);
        glPopMatrix();
    }

    for (World::plane_array_t::const_iterator it = g_world->planes.begin();
         it != g_world->planes.end(); ++it)
    {
        Plane *pl = *it;
        glPushMatrix();
        glm::vec3 offset = -pl->n * pl->d;
        glTranslatef(offset.x, offset.y, offset.z);
        glm::mat4 rot_matrix = gen_rotation_matrix(pl->n, glm::vec3(0.f, 1.f, 0.f));
        glMultMatrixf(glm::value_ptr(rot_matrix));
        g_plane_surface->draw();
        glPopMatrix();
    }

    if (!alt_color)
        glColor3f(0.4f, 0.7f, 0.8f);
    else
        glColor3f(0.75f, 0.3f, 0.25f);
    
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

    if (height == 0)
        height = 1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0, (GLdouble)width / height, 0.01, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
}

void update()
{
    double t = ptime();

    if (g_update)
    {
        g_cloth->lock();
        if (g_script != NULL)
        {
            g_script->update(0.01f);
        }
        g_cloth->step(0.01f);
        g_cloth->unlock();
    }
    
    glutPostRedisplay();
}

#define REQUIRE_EXTENSION(name)                             \
    if (!glewIsSupported(name))                             \
    {                                                       \
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
    
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Error: Could not initialize GLEW\n");
        return 1;
    }
    
    REQUIRE_EXTENSION("GL_ARB_vertex_buffer_object");

    g_world = new World();
    /*g_world->planes.push_back(Plane(glm::vec3(-1.0f, 0.5f, -1.0f),
                                    glm::vec3(-1.0f, 0.5f, 1.0f),
                                    glm::vec3(1.0f, -0.5f, 1.0f)));*/

    /*g_world->planes.push_back(new Plane(glm::vec3(0.0f, -0.9f, 0.0f),
                                        glm::vec3(0.0f, -0.9f, 1.0f),
                                        glm::vec3(1.0f, -0.9f, 0.0f)));*/

    /*g_world->planes.push_back(Plane(glm::vec3(0.0f, 0.1f, 0.0f),
                                    glm::vec3(1.0f, 0.1f, 0.0f),
                                    glm::vec3(0.0f, 0.1f, 1.0f)));*/
    
    g_cloth = new Cloth(2.0f, 2.0f, 32, 32, *g_world);
    reset();

    if (argc > 1)
    {
        std::string error_msg;
        g_script = new Script(*g_world);

        for (int i = 1; i < argc; ++i)
        {
            const char *fname = argv[i];
            if (!g_script->load(fname, &error_msg))
            {
                fprintf(stderr, "%s\n", error_msg.c_str());
                return 1;
            }
        }
        g_script->init();
    }

    g_plane_surface = new Surface(2, 2);
    // make a XZ-oriented plane spanning (-1, 0, -1) to (+1, 0, +1)
    g_plane_surface->lock();
    g_plane_surface->make_plane(glm::vec3(-1.f, 0.f, -1.f),
                                glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f),
                                2.f, 2.f);
    g_plane_surface->unlock();

    glutMainLoop();
    
    delete g_cloth;
    delete g_world;
    if (g_script != NULL) delete g_script;
    
    return 0;
}
