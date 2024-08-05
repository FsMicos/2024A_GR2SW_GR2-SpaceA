#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords; // Añadido para las coordenadas de textura

out vec2 TexCoords; // Variable de salida para las coordenadas de textura

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords; // Pasar las coordenadas de textura al fragment shader
}