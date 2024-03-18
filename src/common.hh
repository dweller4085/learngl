#ifndef __learngl_common__
#define __learngl_common__

#include <limits>
#include <sal.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include <mkb/base.hh>

constexpr float kappa = 1.57079632679f;
constexpr float pi = 3.14159265359f;
constexpr float tau = 6.28318530717f;
constexpr float epsilon = std::numeric_limits<float>::epsilon();

using glm::vec2, glm::vec3, glm::vec4, glm::mat4;

#endif // __learngl_common__
