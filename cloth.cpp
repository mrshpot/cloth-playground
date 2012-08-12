#include <assert.h>
#include <stdio.h>

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


Cloth::Cloth(float width, float height, size_t rows, size_t cols)
    : m_locked(false)
    , m_width(width)
    , m_height(height)
    , m_rows(rows)
    , m_cols(cols)
{
    assert(rows >= 2);
    assert(cols >= 2);
    
    glGenBuffersARB(1, &m_vbo);
    glGenBuffersARB(1, &m_ibo);
    
    m_num_points = rows * cols;
    m_points = new Point[m_num_points];

    float width_half = width * 0.5f,
        height_half = height * 0.5f;
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
        {
            m_points[i * cols + j].pos = \
                glm::vec3(width * j / cols - width_half,
                          height * i / rows - height_half,
                          0.0f);
        }
    }

    gen_indices();
    upload();
}

Cloth::~Cloth()
{
    glDeleteBuffersARB(1, &m_vbo);
    glDeleteBuffersARB(1, &m_ibo);
    
    delete[] m_points;
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

Point& Cloth::at(int i, int j)
{
    return m_points[i * m_cols + j];
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
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, NULL);
    // glDrawArrays(GL_TRIANGLES, 0, m_num_indices);

    // glColor3f(0.2f, 0.3f, 0.9f);
    // glDrawArrays(GL_LINE_STRIP, 0, m_num_indices);
    
    glPointSize(5.0f);
    glColor3f(0.9f, 0.4f, 0.6f);
    glPolygonOffset(1.0, -1.0);
    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_LINE_STRIP, m_num_indices, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_POINTS, 0, m_num_indices);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
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
