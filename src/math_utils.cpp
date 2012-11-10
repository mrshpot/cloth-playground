#include <cmath>

#include "math_utils.hpp"


glm::mat4 gen_rotation_matrix(const glm::vec3 &from, const glm::vec3 &to)
{
    // Paper: http://www.acm.org/jgt/papers/MollerHughes99/
    // Reference code:  http://www.acm.org/jgt/papers/MollerHughes99/code.html
    
    const float abs_fx = fabs(from.x);
    const float abs_fy = fabs(from.y);
    const float abs_fz = fabs(from.z);

    glm::vec3 p;
    if (abs_fx < abs_fy && abs_fx < abs_fz)
    {
        p = glm::vec3(1.f, 0.f, 0.f);
    }
    else if (abs_fy < abs_fx && abs_fy < abs_fz)
    {
        p = glm::vec3(0.f, 1.f, 0.f);
    }
    else
    {
        p = glm::vec3(0.f, 0.f, 1.f);
    }
    
    const glm::vec3 u = p - from;
    const glm::vec3 v = p - to;
    const float uu = glm::dot(u, u);
    const float vv = glm::dot(v, v);
    const float c1 = 2.0f / glm::dot(u, u);
    const float c2 = 2.0f / glm::dot(v, v);
    const float c3 = c1 * c2 * glm::dot(u, v);

    glm::mat4 res(1.0f);

    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            res[i][j] = (- c1 * u[i] * u[j]
                         - c2 * v[i] * v[j]
                         + c3 * v[i] * u[j]);
        }
        res[i][i] += 1.0f;
    }

    return res;
}
