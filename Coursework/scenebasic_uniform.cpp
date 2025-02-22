#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

#include <GLFW/glfw3.h>

//For working out delta time
float lastFrameTime = 0.0f;

//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 10.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
//Fixed height of camera
float fixedY = 0.0f;

//For mouse controls
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

//For lighting animation
float brightness = 0.0f;
bool negative = true;

SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0f), sky(100.0f)
{
    //Objects
    floor = ObjMesh::load("media/floor.obj", true);
    windowWall = ObjMesh::load("media/windowWall.obj", true);
    wall = ObjMesh::load("media/wall.obj", true);
    ceiling = ObjMesh::load("media/ceiling.obj", true);
    doorframe = ObjMesh::load("media/doorframe.obj", true);

    //Textures
    floorTexture = Texture::loadTexture("media/textures/floor.png");
    wallTexture = Texture::loadTexture("media/textures/wall.png");
    doorframeTexture = Texture::loadTexture("media/textures/doorframe.png");
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);

    model = mat4(1.0f);
    view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    projection = mat4(1.0f);

    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, SceneBasic_Uniform::mouse_callback);

    //Skybox
    GLuint cubeTex = Texture::loadCubeMap("media/skybox/space");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    //Spot lights
    vec4 lightPositions[] = {
    vec4(-2.0f, 2.0f, -10.0f, 1.0f),
    vec4(2.0f, 2.0f, -10.0f, 1.0f),
    vec4(-2.0f, -2.0f, -10.0f, 1.0f),
    vec4(2.0f, -2.0f, -10.0f, 1.0f)
    };

    vec4 lightDirections[] = {
        normalize(vec4(0.0f, -1.0f,  1.0f, 0.0f)),
        normalize(vec4(0.0f, -1.0f,  1.0f, 0.0f)),
        normalize(vec4(0.0f,  1.0f,  1.0f, 0.0f)),
        normalize(vec4(0.0f,  1.0f,  1.0f, 0.0f))
    };

    for (int i = 0; i < 4; i++) {
        std::stringstream position;
        position << "lights[" << i << "].Position";
        prog.setUniform(position.str().c_str(), view * lightPositions[i]);

        std::stringstream direction;
        direction << "lights[" << i << "].Direction";
        prog.setUniform(direction.str().c_str(), vec3(view * lightDirections[i]));

        std::stringstream L;
        L << "lights[" << i << "].L";
        prog.setUniform(L.str().c_str(), vec3(0.8f, 0.0f, 0.0f));

        std::stringstream La;
        La << "lights[" << i << "].La";
        prog.setUniform(La.str().c_str(), vec3(brightness, 0.0f, 0.0f));
    }

    //Point light (The Star)
    prog.setUniform("lights[4].Position", view * vec4(-20.0f, 1.0f, 0.0f, 1.0f));
    prog.setUniform("lights[4].L", vec3(1.0f, 0.96f, 0.91f));
    prog.setUniform("lights[4].La", vec3(0.05f, 0.0f, 0.0f));
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    GLFWwindow* window = glfwGetCurrentContext();

    float deltaTime = t - lastFrameTime;
    lastFrameTime = t;

    vec3 positionBefore = cameraPosition;

    //Movement
    const float movementSpeed = 5.0f * deltaTime;
    vec3 forwardDir = normalize(vec3(cameraFront.x, 0.0f, cameraFront.z));
    vec3 rightDir = normalize(cross(forwardDir, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += movementSpeed * forwardDir;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= movementSpeed * forwardDir;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= rightDir * movementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += rightDir * movementSpeed;
    }

    //Fix Height
    cameraPosition.y = fixedY;

    float x = cameraPosition.x;
    float z = cameraPosition.z;

    //Wall Check
    if (x < -1.5f) {
        cameraPosition.x = -1.5f;
    }
    else if (x > 1.5f) {
        cameraPosition.x = 1.5f;
    }

    //Doorframe Check
    if (((z > 8.75f && z < 11.25f) || (z < -8.75f && z > -11.25f))) 
    {
        if (!(positionBefore.x < 0.5f && positionBefore.x > -0.5f)) 
        {
            cameraPosition.z = positionBefore.z;
        }
        else 
        {
            cameraPosition.x = positionBefore.x;
        }
    }

    std::cout << cameraPosition.x << ", " << cameraPosition.z << std::endl;
    //Update View
    view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);


    //Spot Lights
    vec4 lightPositions[] = {
    vec4(-2.0f, 2.0f, -10.0f, 1.0f),
    vec4(2.0f, 2.0f, -10.0f, 1.0f),
    vec4(-2.0f, 2.0f, 10.0f, 1.0f),
    vec4(2.0f, 2.0f, 10.0f, 1.0f)
    };

    vec4 lightDirections[] = {
        normalize(vec4(0.0f, -1.0f, 1.0f, 0.0f)),
        normalize(vec4(0.0f, -1.0f, 1.0f, 0.0f)),
        normalize(vec4(0.0f, -1.0f, -1.0f, 0.0f)),
        normalize(vec4(0.0f, -1.0f, -1.0f, 0.0f))
    };

    for (int i = 0; i < 4; i++) {
        std::stringstream position;
        position << "lights[" << i << "].Position";
        prog.setUniform(position.str().c_str(), view * lightPositions[i]);

        std::stringstream direction;
        direction << "lights[" << i << "].Direction";
        prog.setUniform(direction.str().c_str(), vec3(view * lightDirections[i]));
    }

    //Point Lights
    prog.setUniform("lights[4].Position", view * vec4(-20.0f, 1.0f, 0.0f, 1.0f));

    if (negative) {
        brightness -= deltaTime / 10;
        if (brightness < -0.2f) {
            brightness = -0.2f;
            negative = false;
        }
    }
    else {
        brightness += deltaTime / 10;
        if (brightness > -0.1f) {
            brightness = -0.1f;
            negative = true;
        }
    }

    prog.setUniform("lights[0].La", vec3(brightness, 0.0f, 0.0f));
    prog.setUniform("lights[1].La", vec3(brightness, 0.0f, 0.0f));
    prog.setUniform("lights[2].La", vec3(brightness, 0.0f, 0.0f));
    prog.setUniform("lights[3].La", vec3(brightness, 0.0f, 0.0f));
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //Skybox
    model = mat4(1.0f);
    setMatrices();
    sky.render();

    prog.setUniform("Material.Kd", vec3(0.4f, 0.4f, 0.4f));
    prog.setUniform("Material.Ka", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Ks", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Material.Shininess", 80.0f);

    //Floor
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -2.0f, 0.0f));
    setMatrices();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    floor->render();

    //Window Wall
    model = mat4(1.0f);
    model = glm::translate(model, vec3(-2.2f, 0.0f, 0.0f));
    setMatrices();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    windowWall->render();

    //Wall
    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.0f, 0.0f, 0.0f));
    setMatrices();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    wall->render();

    //Ceiling
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, 0.0f));
    setMatrices();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    ceiling->render();

    //Doorframes
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, -10.0f));
    setMatrices();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, doorframeTexture);
    doorframe->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, 10.0f));
    setMatrices();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, doorframeTexture);
    doorframe->render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices() 
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", mat3(mv));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}