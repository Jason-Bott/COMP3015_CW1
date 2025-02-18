#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/objmesh.h"
#include "glm/glm.hpp"

class SceneBasic_Uniform : public Scene
{
private:
    float angle;
    std::unique_ptr<ObjMesh> mesh;

    GLSLProgram prog;
    void setMatrices();
    void compile();
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H