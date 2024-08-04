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
bool isReturning = false; // Nuevo estado para controlar la fase de retorno
bool isKeyPressed = false; // Estado de la tecla
float rotationAngle = 0.0f;
float maxAngle = 20.0f; // M�ximo �ngulo de rotaci�n
float animationDuration = 0.3f; // Duraci�n de la animaci�n en segundos
float returnDuration = 0.3f; // Duraci�n para regresar a 0 grados
float elapsedTime = 0.0f; // Tiempo transcurrido en la animaci�n
float returnElapsedTime = 0.0f; // Tiempo transcurrido en la fase de retorno
int rotationDirection = 1;

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
    //para poner una ubicacion solo esta funcionando el path completo (K: A mi me funcionó desde model)
    
    //Model asteroide("model/asteroide/asteroide.obj");
    Model balckHole("model/black hole/blackhole.obj");
    Model nave("model/nave/nave.obj");
    //Model planetas("model/planetas/planetas.obj");
    Model tierra ("model/sistema solar/tierra.obj");
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

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float anglePlaneta = 0.0f;
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

        // Render nave
        glm::vec3 cameraPosition = camera.GetPosition();
        glm::vec3 Diferencia = cameraPosition - inicialCamera;
       
        glm::mat4 modelNave = glm::mat4(1.0f);
        modelNave = glm::translate(modelNave, Diferencia); // Ajusta la posici�n de la nave
        modelNave = glm::scale(modelNave, glm::vec3(1.0f, 1.0f, 1.0f));// Ajusta la escala de la nave
        float angle = glm::radians(270.0f); // Convertir 90 grados a radianes
        modelNave = glm::rotate(modelNave, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        angle = glm::radians(180.0f);
        modelNave = glm::rotate(modelNave, angle, glm::vec3(1.0f, 0.0f, 0.0f));
        // Aplica la rotaci�n de animaci�n
        modelNave = glm::rotate(modelNave, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", modelNave);
        nave.Draw(ourShader);
        // Render sol (en el origen del sistema de coordenadas)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-75.0f, 0.0f, -280.0f)); // Posición fija del sol
        model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
        ourShader.setMat4("model", model);
        sol.Draw(ourShader);
		anglePlaneta += glm::radians(deltaTime * 15);
        // Render mercurio (órbita circular alrededor del sol)
        float mercurioDistance = 150.0f; // Distancia del sol
        glm::mat4 mercurioModel = glm::mat4(1.0f);
        mercurioModel = glm::translate(mercurioModel, glm::vec3(mercurioDistance * glm::cos(anglePlaneta), 0.0f, mercurioDistance * glm::sin(anglePlaneta)));
        mercurioModel = glm::scale(mercurioModel, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", mercurioModel);
        mercurio.Draw(ourShader);

        // Render venus (órbita circular alrededor del sol)
        float venusDistance = 270.0f; // Distancia del sol
        glm::mat4 venusModel = glm::mat4(1.0f);
        venusModel = glm::translate(venusModel, glm::vec3(venusDistance * glm::cos(anglePlaneta + glm::radians(45.0f)), 0.0f, venusDistance * glm::sin(anglePlaneta + glm::radians(45.0f))));
        venusModel = glm::scale(venusModel, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", venusModel);
        venus.Draw(ourShader);

        // Render tierra (órbita circular alrededor del sol)
        float tierraDistance = 420.0f; // Distancia del sol
        glm::mat4 tierraModel = glm::mat4(1.0f);
        tierraModel = glm::translate(tierraModel, glm::vec3(tierraDistance * glm::cos(anglePlaneta + glm::radians(90.0f)), 0.0f, tierraDistance * glm::sin(anglePlaneta + glm::radians(90.0f))));
        tierraModel = glm::scale(tierraModel, glm::vec3(15.0f, 15.0f, 15.0f));
        tierraModel = glm::rotate(tierraModel, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", tierraModel);
        tierra.Draw(ourShader);

        // Render marte (órbita circular alrededor del sol)
        float marteDistance = 670.0f; // Distancia del sol
        glm::mat4 marteModel = glm::mat4(1.0f);
        marteModel = glm::translate(marteModel, glm::vec3(marteDistance * glm::cos(anglePlaneta + glm::radians(180.0f)), 0.0f, marteDistance * glm::sin(anglePlaneta + glm::radians(180.0f))));
        marteModel = glm::scale(marteModel, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", marteModel);
        marte.Draw(ourShader);

		// Render Urano (órbita circular alrededor del sol)
		float uranoDistance = 1200.0f; // Distancia del sol
        model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(uranoDistance * glm::cos(anglePlaneta + glm::radians(270.0f)), 0.0f, uranoDistance * glm::sin(anglePlaneta + glm::radians(270.0f))));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		ourShader.setMat4("model", model);
		urano.Draw(ourShader);

		// Render saturno (órbita circular alrededor del sol)
		float saturnoDistance = 1500.0f; // Distancia del sol
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(saturnoDistance * glm::cos(anglePlaneta + glm::radians(315.0f)), 0.0f, saturnoDistance * glm::sin(anglePlaneta + glm::radians(315.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
		saturno.Draw(ourShader);

		// Render saturnoAnillo (órbita circular alrededor del sol)
		float saturnoAnilloDistance = 1500.0f; // Distancia del sol
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(saturnoAnilloDistance * glm::cos(anglePlaneta + glm::radians(315.0f)), 0.0f, saturnoAnilloDistance * glm::sin(anglePlaneta + glm::radians(315.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
		saturnoAnillo.Draw(ourShader);

		// Render neptuno (órbita circular alrededor del sol)
		float neptunoDistance = 1800.0f; // Distancia del sol
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(neptunoDistance * glm::cos(anglePlaneta + glm::radians(360.0f)), 0.0f, neptunoDistance * glm::sin(anglePlaneta + glm::radians(360.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
		neptuno.Draw(ourShader);

		// Render luna (órbita circular alrededor de la tierra)
		float lunaDistance = 50.0f + tierraDistance; // Distancia de la tierra
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(lunaDistance * glm::cos(anglePlaneta + glm::radians(90.0f)), 0.0f, lunaDistance * glm::sin(anglePlaneta + glm::radians(90.0f))));
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        ourShader.setMat4("model", model);
		luna.Draw(ourShader);

		// Render jupiter (órbita circular alrededor del sol)
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
		balckHole.Draw(ourShader);

        


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
        camera.ProcessKeyboard(FORWARD, deltaTime * 5.0);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * 5.0);

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
