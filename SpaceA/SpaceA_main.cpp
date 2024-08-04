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
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
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
bool isReturning = false; // Nuevo estado para controlar la fase de retorno
float rotationAngle = 0.0f;
float maxAngle = 20.0f; // M�ximo �ngulo de rotaci�n
float animationDuration = 0.5f; // Duraci�n de la animaci�n en segundos
float returnDuration = 0.5f; // Duraci�n para regresar a 0 grados
float elapsedTime = 0.0f; // Tiempo transcurrido en la animaci�n
float returnElapsedTime = 0.0f; // Tiempo transcurrido en la fase de retorno
int rotationDirection = 1;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
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
    //glfwSetCursorPosCallback(window, mouse_callback);
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
    //para referencia un omodelo utilice .obj y puedes hacerlo desde model 
    
    //Model asteroide("model/asteroide/asteroide.obj");
    //Model balckHole("model/black hole/blackhole.obj");
    Model nave("model/nave/nave.obj");
    //Model planetas("model/planetas/planetas.obj");
    Model tierra ("model/sistema solar/tierra.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
   


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

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        // En tu bucle de renderizado, aseg�rate de que el valor de Zoom es el que deseas
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Render nave
        glm::vec3 cameraPosition = camera.GetPosition();
        glm::vec3 Diferencia = cameraPosition - inicialCamera;
       
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Diferencia); // Ajusta la posici�n de la nave
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));// Ajusta la escala de la nave
        float angle = glm::radians(270.0f); // Convertir 90 grados a radianes
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        angle = glm::radians(180.0f);
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
        // Aplica la rotaci�n de animaci�n
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        nave.Draw(ourShader);

        // Render sistema solar
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, 0.0f, -20.0f)); // Ajusta la posici�n del sistema solar
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));         // Ajusta la escala del sistema solar
        ourShader.setMat4("model", model);
        tierra.Draw(ourShader);


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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime * 3.0);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * 3.0);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime * 3.0);
        if (!isAnimating) {
            isAnimating = true;
            isReturning = false;
            elapsedTime = 0.0f; // Reinicia el tiempo de animaci�n
            returnElapsedTime = 0.0f; // Reinicia el tiempo de retorno
            rotationDirection = 1; // Direcci�n de rotaci�n hacia adelante
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime * 3.0);
        if (!isAnimating) {
            isAnimating = true;
            isReturning = false;
            elapsedTime = 0.0f; // Reinicia el tiempo de animaci�n
            returnElapsedTime = 0.0f; // Reinicia el tiempo de retorno
            rotationDirection = -1; // Direcci�n de rotaci�n hacia atr�s
        }
    }
    if (isAnimating) {
        if (!isReturning) {
            elapsedTime += deltaTime;
            float progress = elapsedTime / animationDuration;
            if (progress < 1.0f) {
                rotationAngle = maxAngle * sin(progress * 3.14159265f / 2.0f) * rotationDirection; // Rotaci�n hacia el m�ximo
            }
            else {
                rotationAngle = maxAngle * rotationDirection;
                isReturning = true; // Comienza la fase de retorno
                returnElapsedTime = 0.0f; // Reinicia el tiempo de retorno
            }
        }
        else {
            returnElapsedTime += deltaTime;
            float returnProgress = returnElapsedTime / returnDuration;
            if (returnProgress < 1.0f) {
                rotationAngle = maxAngle * (1.0f - returnProgress) * rotationDirection; // Desaceleraci�n hacia 0
            }
            else {
                rotationAngle = 0.0f;
                isAnimating = false; // Finaliza la animaci�n
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