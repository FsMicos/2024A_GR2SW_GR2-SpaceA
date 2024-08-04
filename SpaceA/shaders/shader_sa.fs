#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

uniform vec3 sunPos;
uniform vec3 sunColor;
uniform vec3 viewPos;

void main()
{    
    vec3 ambient = 0.2 * sunColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(sunPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * sunColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * sunColor;

    vec3 lighting = (ambient + diffuse + specular);

    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 color = lighting * textureColor;
    FragColor = texture(texture_diffuse1, TexCoords);
}