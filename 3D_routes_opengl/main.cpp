#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include "shader.h"
#include "grid.h"
#include "drawing_primitives.h"
#include "scene.h"
#include "handle_input.h"


// Window dimensions
const GLuint WIDTH = 1800, HEIGHT = 1000;


// Function to initialize GLFW and GLEW, and create a window
GLFWwindow* initializeWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }

    glViewport(0, 0, width, height);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture the mouse

    return window;
}









void initializeTrajectoryBuffer(GLuint& trajectoryVAO, GLuint& trajectoryVBO, const std::vector<GLfloat>& trajectoryVertices) {
    glGenVertexArrays(1, &trajectoryVAO);
    glGenBuffers(1, &trajectoryVBO);

    glBindVertexArray(trajectoryVAO);

    glBindBuffer(GL_ARRAY_BUFFER, trajectoryVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // Изначально буфер пустой

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}


void updateTrajectoryBuffer(GLuint& trajectoryVBO, const std::vector<GLfloat>& trajectoryVertices) {
    glBindBuffer(GL_ARRAY_BUFFER, trajectoryVBO);
    glBufferData(GL_ARRAY_BUFFER, trajectoryVertices.size() * sizeof(GLfloat), &trajectoryVertices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int main() {
    std::cout << "In main" << std::endl;
    GLFWwindow* window = initializeWindow(WIDTH, HEIGHT, "3D Scene");

    Shader ourShader("../shaders/default.vs", "../shaders/default.fs");


    std::cout << "not Created scene" << std::endl;
    Scene scene(300);

    std::cout << "Created scene" << std::endl;

    GLuint obstVAO, obstVBO, obstEBO;
    glGenVertexArrays(1, &obstVAO);
    glGenBuffers(1, &obstVBO);
    glGenBuffers(1, &obstEBO);

    glBindVertexArray(obstVAO);

    glBindBuffer(GL_ARRAY_BUFFER, obstVBO);
    glBufferData(GL_ARRAY_BUFFER, scene.cur_frame().sphereVertices.size() * sizeof(GLfloat), &scene.cur_frame().sphereVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obstEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.cur_frame().sphereIndices.size() * sizeof(GLuint), &scene.cur_frame().sphereIndices[0], GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    std::cout << "Init obsts" << std::endl;

    // VAO, VBO и EBO для Object
    GLuint controlledObjectVAO, controlledObjectVBO, controlledObjectEBO;
    glGenVertexArrays(1, &controlledObjectVAO);
    glGenBuffers(1, &controlledObjectVBO);
    glGenBuffers(1, &controlledObjectEBO);

    glBindVertexArray(controlledObjectVAO);

    glBindBuffer(GL_ARRAY_BUFFER, controlledObjectVBO);
    glBufferData(GL_ARRAY_BUFFER, scene.cur_frame().controlledObjectVertices.size() * sizeof(GLfloat), &scene.cur_frame().controlledObjectVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, controlledObjectEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.cur_frame().controlledObjectIndices.size() * sizeof(GLuint), &scene.cur_frame().controlledObjectIndices[0], GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    std::cout << "Init obj" << std::endl;


    // VAO, VBO и EBO для Target
    GLuint targetVAO, targetVBO, targetEBO;
    glGenVertexArrays(1, &targetVAO);
    glGenBuffers(1, &targetVBO);
    glGenBuffers(1, &targetEBO);

    glBindVertexArray(targetVAO);

    glBindBuffer(GL_ARRAY_BUFFER, targetVBO);
    glBufferData(GL_ARRAY_BUFFER, scene.cur_frame().targetVertices.size() * sizeof(GLfloat), &scene.cur_frame().targetVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, controlledObjectEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.cur_frame().targetIndices.size() * sizeof(GLuint), &scene.cur_frame().targetIndices[0], GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    std::cout << "Init target" << std::endl;

        
    // VAO и VBO для траектории
    GLuint trajectoryVAO,  trajectoryVBO;
    initializeTrajectoryBuffer(trajectoryVAO, trajectoryVBO, scene.cur_frame().trajectoryVertices);
    std::cout << "Init traj" << std::endl;

    // Generate grid
    std::vector<GLfloat> gridVertices;
    createGrid(gridVertices);

    GLuint gridVAO, gridVBO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);

    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(GLfloat), &gridVertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    

    // Init camera & mouse
    CameraState camera;
    MouseState mouse;

    // Set the required callback functions
    WindowData windowData = { mouse, camera };
    glfwSetWindowUserPointer(window, &windowData);

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        mouse_callback(window, data->mouseState, data->cameraState, xpos, ypos);
        });


    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        scroll_callback(window, data->mouseState, xoffset, yoffset);
        });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData* data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        mouse_button_callback(window, data->mouseState, button, action, mods);
        });


    std::cout << "All Init ok!" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        GLfloat currentFrame = glfwGetTime();
        camera.setTime(currentFrame - camera.lastFrame(), currentFrame);

        processInput(window, scene, camera);

        glClearColor(0.1f, 0.1f, 0.1f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 view = glm::lookAt(camera.cameraPos(), camera.cameraPos() + camera.cameraFront(), camera.cameraUp());
        glm::mat4 projection = glm::perspective(glm::radians(mouse.getFov()), (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);

        GLint modelLoc = glGetUniformLocation(ourShader.ID, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.ID, "view");
        GLint projLoc = glGetUniformLocation(ourShader.ID, "projection");
        GLint colorLoc = glGetUniformLocation(ourShader.ID, "objectColor");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw the grid
        glm::mat4 gridModel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gridModel));
        glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);  // Серый цвет для сетки

        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
        glBindVertexArray(0);
        
        //std::cout << "Grid ok!" << std::endl;

        // Draw obstacles
        glBindVertexArray(obstVAO);
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);  // Белый цвет для препятствий
        for (int i = 0; i < scene.cur_frame().spherePositions.size(); ++i) {
            glm::mat4 sphereModel = glm::translate(glm::mat4(1.0f), scene.cur_frame().spherePositions[i]);
            //glm::mat4 sphereModel = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sphereModel));
            glDrawElements(GL_TRIANGLES, scene.cur_frame().sphereIndices.size(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

        //std::cout << "Obsts ok!" << std::endl;

        // Draw controlled object
        glBindVertexArray(controlledObjectVAO);
        glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f);  // Синий цвет для объекта управления

        glm::mat4 controlledObjectModel = glm::translate(glm::mat4(1.0f), scene.cur_frame().controlledObjectPosition);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(controlledObjectModel));
        glDrawElements(GL_TRIANGLES, scene.cur_frame().controlledObjectIndices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        
        //std::cout << "Obj ok!" << std::endl;

        // Draw target
        glBindVertexArray(targetVAO);
        glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f);  // Зеленый цвет для целевой позиции

        glm::mat4 targetModel = glm::translate(glm::mat4(1.0f), scene.cur_frame().targetPosition);
        //glm::mat4 targetModel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(targetModel));
        glDrawElements(GL_TRIANGLES, scene.cur_frame().targetIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Draw the trajectory
        updateTrajectoryBuffer(trajectoryVBO, scene.cur_frame().trajectoryVertices);

        glm::mat4 trajModel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(trajModel));

        glBindVertexArray(trajectoryVAO);
        glUniform3f(colorLoc, 1.0f, 0.7f, 0.3f);  // Оранжевый цвет для траектории
        glDrawArrays(GL_LINES, 0, scene.cur_frame().trajectoryVertices.size() / 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &controlledObjectVAO);
    glDeleteBuffers(1, &controlledObjectVBO);
    glDeleteBuffers(1, &controlledObjectEBO);

    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);

    glfwTerminate();
    return 0;
}
