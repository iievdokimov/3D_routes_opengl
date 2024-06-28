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

#include "algorithm_base.h"


TrajectoryBuilder TB;


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



void refreshScene(Scene& scene) {
    scene.next_frame();
}


// Function to handle keyboard input
void processInput(GLFWwindow* window, Scene& scene) {
    GLfloat cameraSpeed = 10.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        std::cout << "Space pressed" << std::endl;
        std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
        //initialCameraPos = cameraPos;
        refreshScene(scene);
       // cameraPos = initialCameraPos;
        std::cout << cameraPos.x << "-" << cameraPos.y << "-" << cameraPos.z << std::endl;
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




void handleSpacebarPress(Scene& scene) {
    // Recreate spheres with updated centers
    refreshScene(scene);
}





int main() {
    GLFWwindow* window = initializeWindow(WIDTH, HEIGHT, "3D Sphere with Grid");

    Shader ourShader("../shaders/default.vs", "../shaders/default.fs");

    

    Scene scene(20);



    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, scene.cur_frame().sphereVertices.size() * sizeof(GLfloat), &scene.cur_frame().sphereVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.cur_frame().sphereIndices.size() * sizeof(GLuint), &scene.cur_frame().sphereIndices[0], GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

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
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);

        GLint modelLoc = glGetUniformLocation(ourShader.ID, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.ID, "view");
        GLint projLoc = glGetUniformLocation(ourShader.ID, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw the grid
        glm::mat4 gridModel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gridModel));

        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
        glBindVertexArray(0);

        // Draw all spheres
        glBindVertexArray(VAO);
        for (int i = 0; i < scene.cur_frame().spherePositions.size(); ++i) {
            // Вычисляем матрицу модели для текущей сферы (например, сдвигаем на разные позиции)
            glm::mat4 sphereModel = glm::translate(glm::mat4(1.0f), scene.cur_frame().spherePositions[i]);

            // Передаем матрицы в шейдерную программу
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sphereModel));

            // Рисуем сферу
            glDrawElements(GL_TRIANGLES, scene.cur_frame().sphereIndices.size(), GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);

    glfwTerminate();
    return 0;
}
