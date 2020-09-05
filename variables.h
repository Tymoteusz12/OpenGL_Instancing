#include <vector>
namespace variables {

    float* pos = NULL;
	float width = 800.0f;
	float height = 600.0f;
	float viewAngle = 45.0f;
	float deltaTime = 0.0f, currentFrame = 0.0f, lastFrame = 0.0f;

    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 gravityForce = glm::vec3(0.0f, 0.0f, 0.0f);

}
