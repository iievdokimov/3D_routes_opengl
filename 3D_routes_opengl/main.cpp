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



// Window dimensions
const GLuint WIDTH = 1800, HEIGHT = 1000;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat deltaTime = 0.1f;  // Time between current frame and last frame
GLfloat lastFrame = 0.0f;  // Time of last frame

// Mouse control
bool firstMouse = true;
float yaw = -90.0f;  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right
float pitch = 0.0f;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;
float fov = 45.0f;


// Mouse button state
bool mouseButtonPressed = false;

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


GLfloat cameraSpeed = 100.5f * deltaTime;

// Function to handle keyboard input
void processInput(GLFWwindow* window, Scene& scene) {
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed = 15 * 100.5f * deltaTime;
    else {
        cameraSpeed = 100.5f * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        std::cout << "Frame: " << scene.get_frame_id() << std::endl;
        std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
        scene.next_frame();
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        std::cout << "Frame: " << scene.get_frame_id() << std::endl;
        scene.prev_frame();
    }
}

// Function to handle mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouseButtonPressed) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

// Function to handle mouse scroll
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

// Function to handle mouse button input
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseButtonPressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouseButtonPressed = false;
        firstMouse = true;  // Reset the firstMouse flag to avoid a jump when pressing again
    }
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

    // Set the required callback functions
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    std::cout << "All Init ok!" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, scene);

        glClearColor(0.1f, 0.1f, 0.1f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);

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
