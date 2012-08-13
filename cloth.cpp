#include <cassert>
#include <cstdio>
#include <cmath>

#include <GL/glew.h>
#include <GL/gl.h>

#include "cloth.hpp"


const char* gl_error_str(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR:
            return "No error";
        case GL_INVALID_ENUM:
            return "Enum argument out of range";
        case GL_INVALID_VALUE:
            return "Numeric argument out of range";
        case GL_INVALID_OPERATION:
            return "Operation illegal in current state";
        case GL_STACK_OVERFLOW:
            return "Command would cause a stack overflow";
        case GL_STACK_UNDERFLOW:
            return "Command would cause a stack underflow";
        case GL_OUT_OF_MEMORY:
            return "Not enough memory left to execute the command";
        case GL_TABLE_TOO_LARGE:
            return "The specified table is too large";
        default:
            return "Unknown error";
    }
}

#define CHECK_GL_ERROR()                                                \
    do {                                                                \
        GLenum tmp__error = glGetError();                               \
        if (tmp__error != GL_NO_ERROR) {                                \
            fprintf(stderr, "OpenGL error %d: %s", gl_error_str(tmp__error)); \
            assert(tmp__error == GL_NO_ERROR);                          \
        }                                                               \
    } while(false)


Cloth::Cloth(float width, float height, size_t rows, size_t cols, const World &world)
    : m_locked(false)
    , m_prev_dt(-1.0f)
    , m_gravity(glm::vec3(0.0f, -0.9f, 0.0f))
    , m_world(world)
    , m_width(width)
    , m_height(height)
    , m_rows(rows)
    , m_cols(cols)
{
    assert(rows >= 2);
    assert(cols >= 2);
    
    glGenBuffersARB(1, &m_vbo);
    glGenBuffersARB(1, &m_ibo);

    m_dist_to_left = m_width / (m_cols - 1.0f);
    m_dist_to_bottom = m_height / (m_rows - 1.0f);
    
    m_num_points = rows * cols;
    m_points = new Point[m_num_points];
    m_tmp_points = new Point[m_num_points];
    m_prev_points = new Point[m_num_points];

    float width_half = width * 0.5f,
        height_half = height * 0.5f;
    for (size_t i = 0; i < rows; ++i)
    {
        float fi = (float)i / (rows - 1);
        for (size_t j = 0; j < cols; ++j)
        {
            float fj = (float)j / (cols - 1);
            
            m_points[i * cols + j].pos = \
                glm::vec3(fi * m_height - height_half,
                          1.0f,
                          fj * m_width - width_half);
        }
    }
    
    gen_indices();
    fill_prev_with_current();
    upload();
}

Cloth::~Cloth()
{
    glDeleteBuffersARB(1, &m_vbo);
    glDeleteBuffersARB(1, &m_ibo);
    
    delete[] m_points;
    delete[] m_tmp_points;
    delete[] m_prev_points;
}

void Cloth::lock()
{
    assert(!m_locked);
    m_locked = true;
}

void Cloth::unlock()
{
    assert(m_locked);
    m_locked = false;
    upload();
}

void Cloth::draw()
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ibo);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, sizeof(m_points[0]),
                    (GLvoid*)offsetof(Point, pos));
    glIndexPointer(GL_UNSIGNED_INT, sizeof(GLuint), (GLvoid*)NULL);

    glColor3f(0.4f, 0.7f, 0.8f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, NULL);
    
    glLineWidth(3.0f);
    glColor3f(0.9f, 0.4f, 0.6f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonOffset(0.0, -1.0);
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
    glPolygonOffset(0.0, 0.0);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
}

void Cloth::step(float dt)
{
    if (m_prev_dt < 0)
        m_prev_dt = dt;
    
    float dt2 = dt * dt;
    float dt_coeff = dt / m_prev_dt;

    // apply force
    for (size_t i = 0; i < m_rows; ++i)
    {
        for (size_t j = 0; j < m_cols; ++j)
        {
            size_t idx = i * m_cols + j;
            Point tmp = m_points[idx];
            Point &x = m_points[idx];
            Point &x_prev = m_prev_points[idx];
            m_points[idx].pos = x.pos +
                ((x.pos - x_prev.pos) * 0.99f * dt_coeff) +
                m_gravity * dt2;
            m_prev_points[idx] = tmp;
        }
    }

    apply_spring_constraints();

    apply_plane_constraints();
    apply_sphere_constraints();

    m_prev_dt = dt;
    upload();
}

void Cloth::upload()
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                    sizeof(m_points[0]) * m_num_points,
                    m_points,
                    GL_DYNAMIC_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void Cloth::gen_indices()
{
    // 2 triangles per each quad cell
    size_t cell_rows = m_rows - 1;
    size_t cell_cols = m_cols - 1;
    
    m_num_indices = cell_rows * cell_cols * 2 * 3;
    GLuint *indices = new GLuint[m_num_indices];

    // iterate over each cell (we have rows-1 x cols-1 quad cells)
    for (size_t i = 0; i < cell_rows; ++i)
    {
        for (size_t j = 0; j < cell_cols; ++j)
        {
            // add two CCW triangles for each
            size_t point_idx = i * m_cols + j;
            size_t idx_idx = (i * cell_cols + j) * 6;
            
            indices[idx_idx + 0] = point_idx;
            indices[idx_idx + 1] = point_idx + 1;
            indices[idx_idx + 2] = point_idx + m_cols;

            indices[idx_idx + 3] = point_idx + m_cols;
            indices[idx_idx + 4] = point_idx + 1;
            indices[idx_idx + 5] = point_idx + m_cols + 1;
        }
    }
    
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ibo);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    sizeof(indices[0]) * m_num_indices,
                    indices,
                    GL_STATIC_DRAW_ARB);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

    delete[] indices;
}

void Cloth::swap_arrays()
{
    Point *tmp = m_points;
    m_points = m_tmp_points;
    m_tmp_points = tmp;
}

void Cloth::fill_prev_with_current()
{
    for (size_t idx = 0; idx < m_num_points; ++idx)
    {
        m_prev_points[idx] = m_points[idx];
    }
}

void Cloth::apply_plane_constraints()
{
    for (World::plane_array_t::const_iterator it = m_world.planes.begin();
         it != m_world.planes.end(); ++it)
    {
        for (size_t idx = 0; idx < m_num_points; ++idx)
        {
            Point &p = m_points[idx];
            float d = it->equ(p.pos);
            if (d < 0)
            {
                p.pos -= it->n * d;
            }
        }
    }
}

void Cloth::apply_sphere_constraints()
{
    for (World::sphere_array_t::const_iterator it = m_world.spheres.begin();
         it != m_world.spheres.end(); ++it)
    {
        float r2 = it->r * it->r;
        
        for (size_t idx = 0; idx < m_num_points; ++idx)
        {
            Point &p = m_points[idx];
            float d = it->equ(p.pos);
            if (d < r2)
            {
                glm::vec3 v = p.pos - it->origin;
                p.pos = (it->r / sqrtf(d)) * v + it->origin;
            }
        }
    }
    
}

static void satisfy_spring(glm::vec3 &a, glm::vec3 &b, float distance)
{
    glm::vec3 d = b - a;
    float l = sqrt(glm::dot(d, d));
    float dl = (distance - l);
    glm::vec3 offset = (0.5f * dl / l) * d;
    a -= offset;
    b += offset;
}

static glm::vec3 solve_spring(const glm::vec3 &a, const glm::vec3 &b, float distance)
{
    glm::vec3 d = b - a;
    float l = sqrt(glm::dot(d, d));
    float dl = (distance - l);
    return (-0.5f * dl / l) * d;
}

void Cloth::apply_spring_constraints()
{
    static const int num_iterations = 4;
    
    for (int iter = 0; iter < num_iterations; ++iter)
    {
        for (size_t i = 0; i < m_rows; ++i)
        {
            for (size_t j = 0; j < m_cols; ++j)
            {
                size_t idx = i * m_cols + j;
                Point &p = m_points[idx];
                glm::vec3 dx = glm::vec3(0.0f);
                if (i > 0) {
                    dx += solve_spring(p.pos, m_points[idx - m_cols].pos,
                                         m_dist_to_bottom);
                }
                if (j > 0) {
                    dx += solve_spring(p.pos, m_points[idx - 1].pos,
                                         m_dist_to_left);
                }
                if (i < (m_rows - 1)) {
                    dx += solve_spring(p.pos, m_points[idx + m_cols].pos,
                                         m_dist_to_bottom);
                }
                if (j < (m_cols - 1)) {
                    dx += solve_spring(p.pos, m_points[idx + 1].pos,
                                         m_dist_to_left);
                }
                p.pos += dx;
            }
        }
        
        // swap_arrays();
    }
}
