#version 330 core
out vec4 FragColor;

in vec3 vertexColor; // Получаем цвет из вершинного шейдера

void main()
{
    FragColor = vec4(vertexColor, 1.0); // Используем полученный цвет
}