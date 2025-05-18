#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <string>

#include "Shader.h"
#include "Model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Camera variables
glm::vec3 cameraPos = glm::vec3(2.0f, 1.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
const float moveSpeed = 2.0f;
const float rotationSpeed = 1.0f;

// Movement variables
glm::mat4 matOZ = glm::mat4(1.0f);    // Вертикальные поршни (движение по Z)
glm::mat4 matXY = glm::mat4(1.0f);     // Горизонтальная каретка (движение по X)
glm::mat4 matXZ1 = glm::mat4(1.0f);    // Схват 1 (вращение вокруг Z)
glm::mat4 matXZ2 = glm::mat4(1.0f);    // Схват 2 (вращение вокруг Z)

float positionZ = 0.0f;  // Положение поршней по Z
float positionX = 0.0f;  // Положение каретки по X
float rotationAngle1 = 0.0f; // Угол вращения схвата 1
float rotationAngle2 = 0.0f; // Угол вращения схвата 2

const float minZPosition = -0.3f;
const float maxZPosition = 0.6f;
const float minXPosition = -0.35f;
const float maxXPosition = 0.0f; 


void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    /////
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { 
        positionZ += moveSpeed * deltaTime;
  
        if (positionZ > maxZPosition) {
            positionZ = maxZPosition;
        }
        matOZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, positionZ, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { 
        positionZ -= moveSpeed * deltaTime;
        
        if (positionZ < minZPosition) {
            positionZ = minZPosition;
        }
        matOZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, positionZ, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { 
        positionX += moveSpeed * deltaTime;
       
        if (positionX > maxXPosition) {
            positionX = maxXPosition;
        }
        matXY = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, positionX));
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { 
        positionX -= moveSpeed * deltaTime;
       
        if (positionX < minXPosition) {
            positionX = minXPosition;
        }
        matXY = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, positionX));
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) { 
        rotationAngle1 += rotationSpeed * deltaTime;
        matXZ1 = glm::rotate(glm::mat4(1.0f), rotationAngle1, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) { 
        rotationAngle1 -= rotationSpeed * deltaTime;
        matXZ1 = glm::rotate(glm::mat4(1.0f), rotationAngle1, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) { 
        rotationAngle2 += rotationSpeed * deltaTime;
        matXZ2 = glm::rotate(glm::mat4(1.0f), rotationAngle2, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) { 
        rotationAngle2 -= rotationSpeed * deltaTime;
        matXZ2 = glm::rotate(glm::mat4(1.0f), rotationAngle2, glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Robotic Arm Simulation", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    Shader ourShader("vertex_shader.glsl", "fragment_shader.glsl");

    
    Model ourModel("asset/Model.obj");


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

       
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        ourShader.SetUniform("projection", glm::value_ptr(projection));
        ourShader.SetUniform("view", glm::value_ptr(view));

       
        glm::vec3 lightColor(0.8f, 0.8f, 0.8f);
        ourShader.SetUniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
        ourShader.SetUniform("material.ambient", 1.0f, 0.5f, 0.31f);
        ourShader.SetUniform("material.diffuse", 1.0f, 0.5f, 0.31f);
        ourShader.SetUniform("material.specular", 0.5f, 0.5f, 0.5f);
        ourShader.SetUniform("material.shininess", 32.0f);
        ourShader.SetUniform("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

        ourModel.Draw(ourShader, matOZ, matXY, matXZ1, matXZ2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}