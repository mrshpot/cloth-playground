#include <stdio.h>
#include <time.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "cloth.hpp"
#include "world.hpp"


World *g_world = NULL;
Cloth *g_cloth = NULL;

bool g_update = true;

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

void render()
{
    glClearColor(0.0, 0.0, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    // glutWireTorus(0.2, 0.4, 10, 10);

    g_cloth->draw();

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
    
    // float width = g_cloth->width();
    // float height = g_cloth->height();
    // size_t rows = g_cloth->rows();
    // size_t cols = g_cloth->cols();
    
    // float width_half = width * 0.5f,
    //     height_half = height * 0.5f;
    // g_cloth->lock();
    // for (size_t i = 0; i < rows; ++i)
    // {
    //     for (size_t j = 0; j < cols; ++j)
    //     {
    //         g_cloth->at(i, j).pos =
    //             glm::vec3(width * j / (cols - 1) - width_half + 0.1 * cos(3.2 * (double)j / (cols - 1) + 1.1 * t),
    //                       height * i / (rows - 1) - height_half,
    //                       0.0f + 0.15 * sin(2.4 * (double)i / (rows - 1) + 2.1 * t));
    //     }
    // }
    // g_cloth->unlock();

    if (g_update)
    {
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

    g_last_fps_update = ptime();
    
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Error: Could not initialize GLEW\n");
        return 1;
    }
    REQUIRE_EXTENSION("GL_ARB_vertex_buffer_object");

    g_world = new World();
    g_world->planes.push_back(mk_plane(glm::vec3(0.0f, -0.8f, 0.0f),
                                       glm::vec3(-0.15f, -0.9f, 0.0f),
                                       glm::vec3(0.0f, -0.9f, 0.1f)));

    /*g_world->planes.push_back(mk_plane(glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 1.0f),
                                       glm::vec3(1.0f, 0.0f, 0.0f)));*/

    /*g_world->planes.push_back(mk_plane(glm::vec3(0.0f, 0.1f, 0.0f),
                                       glm::vec3(1.0f, 0.1f, 0.0f),
                                       glm::vec3(0.0f, 0.1f, 1.0f)));*/
    
    Sphere tmp_sphere;
    tmp_sphere.origin = glm::vec3(0.0f, -0.5f, 0.1f);
    tmp_sphere.r = 0.7f;
    g_world->spheres.push_back(tmp_sphere);

    g_cloth = new Cloth(2.0f, 2.0f, 50, 50, *g_world);

    glutMainLoop();

    delete g_cloth;
    delete g_world;
    
    return 0;
}
