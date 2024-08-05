#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Coordenadas de textura desde el vertex shader

uniform sampler2D texture1; // La textura que usarás

void main()
{
    FragColor = texture(texture1, TexCoords); // Usar la textura para definir el color
}