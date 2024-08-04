#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
float processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

// camera
glm::vec3 inicialCamera(0.0f, 0.4f, 2.0f);
Camera camera(inicialCamera);
//nave
bool isAnimating = false;
bool isReturning = false; // Estado de retorno
bool isKeyPressed = false; // Estado de la tecla
float rotationAngle = 0.0f;
float maxAngle = 10.0f; // Máximo ángulo de rotación
float animationDuration = 1.0f; // Duración para alcanzar el ángulo máximo
float returnDuration = 1.0f; // Duración para regresar a 0 grados
float elapsedTime = 0.0f; // Tiempo transcurrido en la animación
float returnElapsedTime = 0.0f; // Tiempo transcurrido en la fase de retorno
int rotationDirection = 1; //

bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float aceleracion = 30.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
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
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("shaders/shader_sa.vs", "shaders/shader_sa.fs");

    // load models
    //para poner una ubicacion solo esta funcionando el path completo (K: A mi me funcion� desde model)

    Model blackHole("model/black hole/blackhole.obj");
    Model nave("model/nave/nave.obj");
    Model tierra("model/sistema solar/tierra.obj");
    Model sol("model/sistema solar/sol.obj");
    Model urano("model/sistema solar/urano.obj");
    Model venus("model/sistema solar/venus.obj");
    Model saturno("model/sistema solar/saturnoPlaneta.obj");
    Model saturnoAnillo("model/sistema solar/saturnoAnillos.obj");
    Model neptuno("model/sistema solar/neptuno.obj");
    Model mercurio("model/sistema solar/mercurio.obj");
    Model luna("model/sistema solar/luna.obj");
    Model jupiter("model/sistema solar/jupiter.obj");
    Model marte("model/sistema solar/marte.obj");

    // Array de modelos de asteroides
    // Array de modelos de asteroides
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


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.02f, 0.02f, 0.1f, 1.0f); // Un azul oscuro que simula el espacio
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Pruebas porque fallatab la textura
    float anglePlaneta = 0.0f;
    // render loop
    // -----------
    
    //Pos y color del sol
    glm::vec3 sunPos = glm::vec3(-75.0f, 0.0f, -200.0f);
    glm::vec3 sunColor = glm::vec3(1.0f, 1.0f, 0.0f);


    // Suponiendo que cubePositions es un array que define las posiciones de cada asteroide
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
    // Inicializa los vectores de velocidad de los asteroides
    glm::vec3 asteroideVelocities[30];
    for (int i = 0; i < 30; i++) {
        asteroideVelocities[i] = glm::vec3(
            static_cast<float>(rand() % 100 - 50) / 10.0f, // Velocidad en X (-5 a 5)
            static_cast<float>(rand() % 100 - 50) / 10.0f, // Velocidad en Y (-5 a 5)
            static_cast<float>(rand() % 100 - 50) / 10.0f  // Velocidad en Z (-5 a 5)
        );
    }

    // Variables para el c�lculo del tiempo
    float lastFrameTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
     // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        float rotationAngle = processInput(window);
        processInput(window);

        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 6000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        // Config de la luz del sol
        sunPos = glm::vec3(-75.0f, 0.0f, -280.0f);
        ourShader.setVec3("sunPos", sunPos);
        sunColor = glm::vec3(1.0f, 1.0f, 0.0f);
        ourShader.setVec3("sunColor", sunColor);

        // Render nave
        glm::vec3 cameraPosition = camera.GetPosition();
        glm::vec3 Diferencia = cameraPosition - inicialCamera;

        glm::mat4 modelNave = glm::mat4(1.0f);
        modelNave = glm::translate(modelNave, Diferencia); // Ajusta la posici?n de la nave
        modelNave = glm::scale(modelNave, glm::vec3(1.0f, 1.0f, 1.0f));// Ajusta la escala de la nave
        float angle = glm::radians(270.0f); // Convertir 90 grados a radianes
        modelNave = glm::rotate(modelNave, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        angle = glm::radians(180.0f);
        modelNave = glm::rotate(modelNave, angle, glm::vec3(1.0f, 0.0f, 0.0f));
        
        // Aplica la rotaci?n de animaci?n
        modelNave = glm::rotate(modelNave, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", modelNave);
        nave.Draw(ourShader);

        // Render sol (en el origen del sistema de coordenadas)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, sunPos); // Posici�n fija del sol
        model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
        ourShader.setMat4("model", model);
        sol.Draw(ourShader);
        anglePlaneta += glm::radians(deltaTime * 15);

        // Render mercurio (�rbita circular alrededor del sol)
        float mercurioDistance = 150.0f; // Distancia del sol
        glm::mat4 mercurioModel = glm::mat4(1.0f);
        mercurioModel = glm::translate(mercurioModel, glm::vec3(mercurioDistance * glm::cos(anglePlaneta), 0.0f, mercurioDistance * glm::sin(anglePlaneta)));
        mercurioModel = glm::scale(mercurioModel, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", mercurioModel);
        mercurio.Draw(ourShader);

        // Render venus (�rbita circular alrededor del sol)
        float venusDistance = 270.0f; // Distancia del sol
        glm::mat4 venusModel = glm::mat4(1.0f);
        venusModel = glm::translate(venusModel, glm::vec3(venusDistance * glm::cos(anglePlaneta + glm::radians(45.0f)), 0.0f, venusDistance * glm::sin(anglePlaneta + glm::radians(45.0f))));
        venusModel = glm::scale(venusModel, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", venusModel);
        venus.Draw(ourShader);

        // Render tierra (�rbita circular alrededor del sol)
        float tierraDistance = 420.0f; // Distancia del sol
        glm::mat4 tierraModel = glm::mat4(1.0f);
        tierraModel = glm::translate(tierraModel, glm::vec3(tierraDistance * glm::cos(anglePlaneta + glm::radians(90.0f)), 0.0f, tierraDistance * glm::sin(anglePlaneta + glm::radians(90.0f))));
        tierraModel = glm::scale(tierraModel, glm::vec3(15.0f, 15.0f, 15.0f));
        tierraModel = glm::rotate(tierraModel, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", tierraModel);
        tierra.Draw(ourShader);

        // Render marte (�rbita circular alrededor del sol)
        float marteDistance = 670.0f; // Distancia del sol
        glm::mat4 marteModel = glm::mat4(1.0f);
        marteModel = glm::translate(marteModel, glm::vec3(marteDistance * glm::cos(anglePlaneta + glm::radians(180.0f)), 0.0f, marteDistance * glm::sin(anglePlaneta + glm::radians(180.0f))));
        marteModel = glm::scale(marteModel, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", marteModel);
        marte.Draw(ourShader);

        // Render Urano (�rbita circular alrededor del sol)
        float uranoDistance = 1200.0f; // Distancia del sol
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(uranoDistance * glm::cos(anglePlaneta + glm::radians(270.0f)), 0.0f, uranoDistance * glm::sin(anglePlaneta + glm::radians(270.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
        urano.Draw(ourShader);

        // Render saturno (�rbita circular alrededor del sol)
        float saturnoDistance = 1500.0f; // Distancia del sol
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(saturnoDistance * glm::cos(anglePlaneta + glm::radians(315.0f)), 0.0f, saturnoDistance * glm::sin(anglePlaneta + glm::radians(315.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
        saturno.Draw(ourShader);

        // Render saturnoAnillo (�rbita circular alrededor del sol)
        float saturnoAnilloDistance = 1500.0f; // Distancia del sol
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(saturnoAnilloDistance * glm::cos(anglePlaneta + glm::radians(315.0f)), 0.0f, saturnoAnilloDistance * glm::sin(anglePlaneta + glm::radians(315.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
        saturnoAnillo.Draw(ourShader);

        // Render neptuno (�rbita circular alrededor del sol)
        float neptunoDistance = 1800.0f; // Distancia del sol
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(neptunoDistance * glm::cos(anglePlaneta + glm::radians(360.0f)), 0.0f, neptunoDistance * glm::sin(anglePlaneta + glm::radians(360.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
        neptuno.Draw(ourShader);

        // Render luna (�rbita circular alrededor de la tierra)
        float lunaDistance = 50.0f + tierraDistance; // Distancia de la tierra
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lunaDistance * glm::cos(anglePlaneta + glm::radians(90.0f)), 0.0f, lunaDistance * glm::sin(anglePlaneta + glm::radians(90.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
        luna.Draw(ourShader);

        // Render jupiter (�rbita circular alrededor del sol)
        float jupiterDistance = 1200.0f; // Distancia del sol
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(jupiterDistance * glm::cos(anglePlaneta + glm::radians(270.0f)), 0.0f, jupiterDistance * glm::sin(anglePlaneta + glm::radians(270.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
        jupiter.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1200.0f));
        model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
        ourShader.setMat4("model", model);
        blackHole.Draw(ourShader);

        // Actualizar posiciones de los asteroides
        for (int i = 0; i < 30; i++) {
            // Actualiza la posici�n con la velocidad
            asteroidePositions[i] += asteroideVelocities[i] * deltaTime;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, asteroidePositions[i]);

            // Ajusta el �ngulo de rotaci�n para cada asteroide
            float angle = glfwGetTime() * 25.0f * (i % 2 == 0 ? 1 : -1); // Rotaci�n horaria y antihoraria alternada

            // Escalar los asteroides para que se vean m�s grandes
            model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            // Dibujar el modelo del asteroide correspondiente
            asteroides[i % 10].Draw(ourShader); // Reutilizar los modelos c�clicamente
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
float processInput(GLFWwindow* window)
{   
    bool keyA = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool keyD = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime * aceleracion);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * aceleracion);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
        camera.ProcessKeyboard(LEFT, deltaTime * aceleracion);  
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
        camera.ProcessKeyboard(RIGHT, deltaTime * aceleracion);       
    // Detectar si se ha presionado A o D
    bool anyKeyPressed = keyA || keyD;
    if (anyKeyPressed) {
        if (!isKeyPressed) {
            isKeyPressed = true;
            if (!isAnimating) {
                isAnimating = true;
                isReturning = false;
                elapsedTime = 0.0f; // Reinicia el tiempo de animación
                returnElapsedTime = 0.0f; // Reinicia el tiempo de retorno
                rotationDirection = keyA ? 1 : -1; // Dirección de rotación basada en la tecla
            }
        }
    }
    else {
        if (isKeyPressed) {
            isKeyPressed = false;
            if (isAnimating && !isReturning) {
                isReturning = true; // Comienza la fase de retorno si la tecla se suelta
                returnElapsedTime = 0.0f; // Reinicia el tiempo de retorno
            }
        }
    }

    if (isAnimating) {
        if (!isReturning) {
            elapsedTime += deltaTime;
            float progress = elapsedTime / animationDuration;
            if (progress < 1.0f) {
                rotationAngle = maxAngle * sin(progress * 3.14159265f / 2.0f) * rotationDirection; // Rotación hacia el máximo
            }
            else {
                rotationAngle = maxAngle * rotationDirection; // Mantiene el ángulo máximo
            }
        }
        else {
            returnElapsedTime += deltaTime;
            float returnProgress = returnElapsedTime / returnDuration;
            if (returnProgress < 1.0f) {
                rotationAngle = maxAngle * (1.0f - returnProgress) * rotationDirection; // Desaceleración hacia 0
            }
            else {
                rotationAngle = 0.0f;
                isAnimating = false; // Finaliza la animación
            }
        }
    }
    return rotationAngle;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
