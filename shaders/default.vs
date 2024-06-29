#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;  // Матрица модели (трансформации) для каждой сферы

uniform vec3 objectColor; // Новый uniform для цвета объекта

out vec3 vertexColor; // Передаваемая переменная для цвета

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertexColor = objectColor; // Передаем цвет во фрагментный шейдер
}