#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <windows.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>
#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1000;

// camera
glm::vec3 inicialCamera(100.0f, 0.0f, 0.0f);
Camera camera(inicialCamera);

// nave
bool isAnimating = false;
bool isReturning = false;
bool isKeyPressed = false;
float maxAngle = 10.0f;
float animationDuration = 1.0f;
float returnDuration = 1.0f;
float elapsedTime = 0.0f;
float returnElapsedTime = 0.0f;
int rotationDirection = 1;
float rotationAngle = 0.0f;
float targetRotationAngle = 0.0f;
float maxRotationAngle = 20.0f; // Maximum rotation angle
float rotationSpeed = 5.0f; // Speed of rotation (adjust as needed)

bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float aceleracion = 10.0f;
float shiftAcceleration = 40.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float verticalOffset = 0.0f;
float targetVerticalOffset = 0.0f;
float maxVerticalOffset = 1.0f; // Maximum vertical offset
float verticalSpeed = 30.0f; // Speed of vertical movement (adjust as needed)

int main()
{  
    // Reproducir el archivo de sonido de forma asíncrona
    if (PlaySound(TEXT("ost/ost.wav"), NULL, SND_FILENAME | SND_ASYNC)) {
        std::cout << "Reproduccion del sonido iniciada." << std::endl;
    }
    else {
        std::cerr << "No se pudo reproducir el sonido." << std::endl;
    }

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SpaceA", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader ourShader("shaders/shader_sa.vs", "shaders/shader_sa.fs");
    Shader lightCubeShader("shaders/shader_light.vs", "shaders/shader_light.fs");
    Model blackHole("model/black hole/blackhole.obj");
    Model nave("model/nave/nave.obj");
    Model sol("model/sistema solar/sol.obj");
    Model luna("model/sistema solar/luna.obj");

    // load models
    Model planets[9] = {
        Model("model/sistema solar/mercurio.obj"),
        Model("model/sistema solar/venus.obj"),
        Model("model/sistema solar/tierra.obj"),
        Model("model/sistema solar/marte.obj"),
        Model("model/sistema solar/jupiter.obj"),
        Model("model/sistema solar/saturnoPlaneta.obj"),
        Model("model/sistema solar/saturnoAnillos.obj"),
        Model("model/sistema solar/urano.obj"),
        Model("model/sistema solar/neptuno.obj")
    };

    // Array of asteroid models
    Model asteroides[10] = {
        Model("model/asteroides/asteroide1.obj"),
        Model("model/asteroides/asteroide2.obj"),
        Model("model/asteroides/asteroide3.obj"),
        Model("model/asteroides/asteroide4.obj"),
        Model("model/asteroides/asteroide5.obj"),
        Model("model/asteroides/asteroide6.obj"),
        Model("model/asteroides/asteroide7.obj"),
        Model("model/asteroides/asteroide8.obj"),
        Model("model/asteroides/asteroide9.obj"),
        Model("model/asteroides/asteroide10.obj")
    };

    glClearColor(0.02f, 0.02f, 0.1f, 1.0f);
    float anglePlaneta = 0.0f;

    // Sun position and color
    glm::vec3 sunPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sunColor = glm::vec3(1.0f, 1.0f, 0.0f);

    // Posiciones espaciales para cubos
    glm::vec3 planetPositions[9] = {
    glm::vec3(10.0f,  0.0f,  0.0f), // Mercurio
    glm::vec3(20.0f,  0.0f,  0.0f), // Venus
    glm::vec3(30.0f,  0.0f,  0.0f), // Tierra
    glm::vec3(40.0f,  0.0f,  0.0f), // Marte
    glm::vec3(50.0f,  0.0f,  0.0f), // Jupiter
    glm::vec3(60.0f,  0.0f,  0.0f), // Saturno 
    glm::vec3(60.0f,  0.0f,  0.0f), // Saturno Anillos
    glm::vec3(70.0f, 0.0f,  0.0f), // Urano
    glm::vec3(80.0f, 0.0f,  0.0f)  // Neptuno
    };

    float rotationSpeeds[9] = {
    2.9f, // Mercurio
    2.1f, // Venus
    1.8f, // Tierra
    1.5f, // Marte
    0.8f, // Jupiter
    0.6f, // Saturno
    0.6f, // Saturno Anillos
    0.4f, // Urano
    0.3f  // Neptuno
    };

    glm::vec3 moonPosition = glm::vec3(3.0f, 0.0f, 0.0f); // Posición de la luna relativa a la Tierra
    float moonOrbitSpeed = 1.0f; // Velocidad de órbita de la luna

    // Asteroid positions and velocities
    glm::vec3 asteroidePositions[30] = {
        glm::vec3(100.0f, 50.0f, -100.0f),
        glm::vec3(50.0f, 930.0f, -2340.0f),
        glm::vec3(5.0f, 950.0f, -2400.0f),
        glm::vec3(-120.0f, 80.0f, -150.0f),
        glm::vec3(180.0f, -70.0f, -210.0f),
        glm::vec3(-200.0f, 0.0f, -260.0f),
        glm::vec3(270.0f, 110.0f, -320.0f),
        glm::vec3(-1540.0f, 353.0f, -2010.0f),
        glm::vec3(1580.0f, 800.0f, -2070.0f),
        glm::vec3(-310.0f, 40.0f, -370.0f),
        glm::vec3(380.0f, -95.0f, -420.0f),
        glm::vec3(-420.0f, -130.0f, -470.0f),
        glm::vec3(540.0f, 210.0f, 5.0f),
        glm::vec3(-580.0f, 190.0f, -690.0f),
        glm::vec3(0.0f, -160.0f, -750.0f),
        glm::vec3(34.0f, 980.0f, -2450.0f),
        glm::vec3(5.0f, 0.0f, -2510.0f),
        glm::vec3(-660.0f, -210.0f, -800.0f),
        glm::vec3(90.0f, 750.0f, -2620.0f),
        glm::vec3(-2020.0f, 24.0f, -2670.0f),
        glm::vec3(780.0f, -250.0f, -970.0f),
        glm::vec3(-820.0f, -290.0f, -1020.0f),
        glm::vec3(860.0f, 350.0f, 5.0f),
        glm::vec3(-900.0f, 370.0f, -1130.0f),
        glm::vec3(940.0f, 400.0f, -1190.0f),
        glm::vec3(-980.0f, 430.0f, 5.0f),
        glm::vec3(1020.0f, 0.0f, 34.0f),
        glm::vec3(-1060.0f, 480.0f, -1350.0f),
        glm::vec3(5.0f, 5.0f, -1410.0f),
        glm::vec3(-1140.0f, 520.0f, 0.0f)
    };
    glm::vec3 asteroideVelocities[30];
    for (int i = 0; i < 30; i++) {
        asteroideVelocities[i] = glm::vec3(
            static_cast<float>(rand() % 100 - 50) / 10.0f,
            static_cast<float>(rand() % 100 - 50) / 10.0f,
            static_cast<float>(rand() % 100 - 50) / 10.0f
        );
    }
    glm::vec3 naveOffset = glm::vec3(0.0f, -0.1f, 0.5f);


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.use();
        ourShader.setVec3("light.position", sunPos);
        ourShader.setVec3("viewPos", camera.Position);

        //Exercise 14 Part 2
       // light properties
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        ourShader.setFloat("material.shininess", 20.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 6000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Sun light configuration
        ourShader.setVec3("sunPos", sunPos);
        ourShader.setVec3("sunColor", sunColor);

        // Calculate nave position relative to camera
        glm::vec3 navePosition = camera.Position + camera.Front * naveOffset.z + camera.Up * naveOffset.y + camera.Right * naveOffset.x;

        // Render nave
        glm::mat4 modelNave = glm::mat4(1.0f);
        modelNave = glm::translate(modelNave, navePosition);

        // Calculate rotation based on camera orientation
        glm::vec3 naveDirection = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));
        float yaw = atan2(naveDirection.x, naveDirection.z);
        float pitch = -asin(camera.Front.y); // Negative because OpenGL uses a right-handed coordinate system

        // Apply rotations
        modelNave = glm::rotate(modelNave, yaw, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis (yaw)
        modelNave = glm::rotate(modelNave, pitch, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X-axis (pitch)

        // Apply additional rotations and scaling
        modelNave = glm::rotate(modelNave, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelNave = glm::rotate(modelNave, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelNave = glm::scale(modelNave, glm::vec3(0.1f, 0.1f, 0.1f)); // Adjust scale as needed

        // Apply the smooth rotation animation
        modelNave = glm::rotate(modelNave, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));

        ourShader.setMat4("model", modelNave);
        nave.Draw(ourShader);

        glm::mat4 model = glm::mat4(1.0f);

        anglePlaneta += glm::radians(deltaTime * 15);

        // Actualizar posiciones de los planet
        // Actualizar posiciones de los planetas
        for (int i = 0; i < 9; i++) {
            glm::mat4 model = glm::mat4(1.0f);

            // Rotación alrededor del sol (más lenta)
            model = glm::rotate(model, currentFrame * rotationSpeeds[i] * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

            // Traslación desde el sol
            model = glm::translate(model, planetPositions[i]);

            // Rotación sobre su propio eje
            model = glm::rotate(model, anglePlaneta, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            ourShader.setMat4("model", model);

            // Dibujar el modelo del planeta correspondiente
            planets[i % 9].Draw(ourShader);

            // Si el planeta es la Tierra, renderizar la luna
            if (i == 2) { // Tierra es el tercer planeta (índice 2)
                glm::mat4 moonModel = glm::mat4(1.0f);

                // Rotación de la luna alrededor de la Tierra
                moonModel = glm::rotate(moonModel, currentFrame * moonOrbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));

                // Traslación desde la Tierra
                moonModel = glm::translate(moonModel, moonPosition);

                // Aplicar la transformación de la Tierra a la luna
                moonModel = model * moonModel;

                // Rotación sobre su propio eje
                moonModel = glm::rotate(moonModel, anglePlaneta, glm::vec3(0.0f, 1.0f, 0.0f));

                ourShader.setMat4("model", moonModel);
                luna.Draw(ourShader);
            }
        }


        glm::mat4 modelBlackHole = glm::mat4(1.0f);
        modelBlackHole = glm::mat4(1.0f);
        modelBlackHole = glm::translate(modelBlackHole, glm::vec3(0.0f, 0.0f, 1200.0f));
        modelBlackHole = glm::scale(modelBlackHole, glm::vec3(20.0f, 20.0f, 20.0f));
        ourShader.setMat4("model", modelBlackHole);
        blackHole.Draw(ourShader);


        // Actualizar posiciones de los asteroides
        for (int i = 0; i < 30; i++) {
            // Actualiza la posici n con la velocidad
            asteroidePositions[i] += asteroideVelocities[i] * deltaTime;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, asteroidePositions[i]);

            // Ajusta el  ngulo de rotaci n para cada asteroide
            float angle = glfwGetTime() * 25.0f * (i % 2 == 0 ? 1 : -1); // Rotaci n horaria y antihoraria alternada

            // Escalar los asteroides para que se vean m s grandes
            model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            // Dibujar el modelo del asteroide correspondiente
            asteroides[i % 10].Draw(ourShader); // Reutilizar los modelos c clicamente
        }




        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        lightCubeShader.setMat4("model", model);
        sol.Draw(lightCubeShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    bool isShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

    // Determinar la aceleración en función de si Shift está presionado
    float currentAcceleration = isShiftPressed ? shiftAcceleration : aceleracion;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime * currentAcceleration);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * currentAcceleration);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime * currentAcceleration);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime * currentAcceleration);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime * currentAcceleration);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime * currentAcceleration);

    // Handling rotation animation
    bool keyA = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool keyD = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    if (keyA && !keyD)
    {
        targetRotationAngle = maxRotationAngle;
    }
    else if (keyD && !keyA)
    {
        targetRotationAngle = -maxRotationAngle;
    }
    else
    {
        targetRotationAngle = 0.0f;
    }

    // Smoothly interpolate current rotation angle towards target rotation angle
    float rotationDifference = targetRotationAngle - rotationAngle;
    if (std::abs(rotationDifference) > 0.01f)
    {
        rotationAngle += rotationDifference * rotationSpeed * deltaTime;
    }
    else
    {
        rotationAngle = targetRotationAngle;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}