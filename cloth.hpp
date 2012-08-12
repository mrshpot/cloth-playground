#ifndef CLOTH_HPP__INCLUDED
#define CLOTH_HPP__INCLUDED

#include <glm/glm.hpp>


struct Point
{
    glm::vec3 pos;
};

class Cloth
{
    Point *m_points;
    unsigned int m_vbo, m_ibo;
    bool m_locked;
    
    float m_width, m_height;
    size_t m_rows, m_cols;
    size_t m_num_points, m_num_indices;

public:
    Cloth(float width, float height, size_t rows, size_t cols);
    ~Cloth();

    void lock();
    void unlock();
    
    Point& at(int i, int j);
    void draw();

    size_t rows() { return m_rows; }
    size_t cols() { return m_cols; }
    float width() { return m_width; }
    float height() { return m_height; }

private:
    void upload();
    void gen_indices();
};

#endif // CLOTH_HPP__INCLUDED
