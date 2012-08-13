#ifndef CLOTH_HPP__INCLUDED
#define CLOTH_HPP__INCLUDED

#include <glm/glm.hpp>

#include "world.hpp"


struct Point
{
    glm::vec3 pos;
};

class Cloth
{
    Point *m_points, *m_tmp_points, *m_prev_points;
    unsigned int m_vbo, m_ibo;
    bool m_locked;
    float m_prev_dt;
    glm::vec3 m_gravity;
    const World &m_world;
    
    float m_width, m_height;
    size_t m_rows, m_cols;
    float m_dist_to_left, m_dist_to_bottom;
    
    size_t m_num_points, m_num_indices;

public:
    Cloth(float width, float height, size_t rows, size_t cols, const World &world);
    ~Cloth();

    void lock();
    void unlock();
    
    Point& at(int i, int j) { return m_points[i * m_cols + j]; }
    void draw();

    void step(float timestep);

    size_t rows() { return m_rows; }
    size_t cols() { return m_cols; }
    float width() { return m_width; }
    float height() { return m_height; }

private:
    void upload();
    void gen_indices();
    void swap_arrays();
    void fill_prev_with_current();

    void apply_plane_constraints();
    void apply_sphere_constraints();
    void apply_spring_constraints();
};

#endif // CLOTH_HPP__INCLUDED
