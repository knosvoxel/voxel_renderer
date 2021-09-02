#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
//#include <cubeLight.h>

//#include <imgui/backends/imgui_impl_opengl3.h>
//#include <imgui/backends/imgui_impl_opengl3_loader.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const* path);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool escapePressed = false;
bool vsyncOn = false;
bool spotLightOn = false;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Renderer", NULL, NULL);
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
    glfwSetKeyCallback(window, keyboard_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // tell stb_image.h to flip loaded textures on the y-axis (before loading the model)
    stbi_set_flip_vertically_on_load(true);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool show_demo_window = false;
    bool show_another_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImVec4 dirAmbient = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    ImVec4 dirDiffuse = ImVec4(0.4f, 0.4f, 0.4f, 1.00f);
    ImVec4 dirSpecular = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
    ImVec4 dirDirection = ImVec4(-0.2f, -1.0f, -0.3f, 1.00f);

    ImVec4 lightCubeColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 pointAmbient = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    ImVec4 pointDiffuse = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);
    ImVec4 pointSpecular = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    float pointAmbientValue = 0.2f;
    float pointDiffuseValue = 0.5f;
    float pointLinear = 0.09f;
    float pointQuadratic = 0.032f;

    ImVec4 spotAmbient = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    ImVec4 spotDiffuse = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    ImVec4 spotSpecular = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    float spotLinear = 0.09f;
    float spotQuadratic = 0.032f;
    float spotCutOff = 12.5f;
    float spotOuterCutOff = 15.0f;

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("../shaders/learnopengl/lighting.vs", "../shaders/learnopengl/lighting.fs");
    Shader lightCubeShader("../shaders/learnopengl/light_cube.vs", "../shaders/learnopengl/light_cube.fs");

    Model ourModel("../res/objects/backpack/backpack.obj");

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

   glm::vec3 pointLightPositions[]{
       glm::vec3( 0.7f,  0.2f,  2.0f),
       glm::vec3( 2.3f, -3.3f, -4.0f),
       glm::vec3(-4.0f,  2.0f, -12.0f),
       glm::vec3( 0.0f,  0.0f, -3.0f)
   };

   // second, configure the light's VAO
    unsigned int VBO, lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    /*static float framesPerSeconds = 0.0f;
    static float lastTime = 0.0f;*/
    //CubeLights cubeLights;
    //cubeLights.addLight(pointLightPositions[0]);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        /*
        ++framesPerSeconds;

        if(currentFrame - lastTime > 1.0f)
        {
            lastTime = currentFrame;
            std::cout << int(framesPerSeconds) << std::endl;

            framesPerSeconds = 0;
        }*/

        if(vsyncOn)
        {
            glfwSwapInterval(0);
        }
        else
        {
            glfwSwapInterval(1);
        }

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /*if(show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

                {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }*/

        ImGui::Begin("Lights");
        
        ImGui::Text("Directional Light");
        ImGui::DragFloat3("Direction", (float*)&dirDirection, 0.01f);
        ImGui::ColorEdit3("Dir Ambient", (float*)&dirAmbient);  
        ImGui::ColorEdit3("Dir Diffuse", (float*)&dirDiffuse);  
        ImGui::ColorEdit3("Dir Specular", (float*)&dirSpecular);   

        ImGui::Text("Point Lights");
        ImGui::ColorEdit3("Point Light Color", (float*)&lightCubeColor);
        ImGui::DragFloat("Point Ambient", (float*)&pointAmbientValue, 0.001f);
        if(pointAmbientValue > 1.0f)
            pointAmbientValue = 1.0f;
        if(pointAmbientValue < 0.0f)
            pointAmbientValue = 0.0f;
        ImGui::DragFloat("Point Diffuse", (float*)&pointDiffuseValue, 0.001f);  
        if(pointDiffuseValue > 1.0f)
            pointDiffuseValue = 1.0f;
        if(pointDiffuseValue < 0.0f)
            pointDiffuseValue = 0.0f;
        ImGui::ColorEdit3("Point Specular", (float*)&pointSpecular);   
        ImGui::InputFloat("Point Linear", &pointLinear);
        ImGui::InputFloat("Point Quadratic", &pointQuadratic);

        ImGui::Text("Spotlight");
        ImGui::ColorEdit3("Spot Ambient", (float*)&spotAmbient);  
        ImGui::ColorEdit3("Spot Diffuse", (float*)&spotDiffuse);  
        ImGui::ColorEdit3("Spot Specular", (float*)&spotSpecular);   
        ImGui::InputFloat("Spot Linear", &spotLinear);
        ImGui::InputFloat("Spot Quadratic", &spotQuadratic);
        ImGui::InputFloat("Cutoff", &spotCutOff);
        ImGui::InputFloat("Outer Cutoff", &spotOuterCutOff);
        ImGui::Checkbox("Spotlight Active", &spotLightOn);

        ImGui::Text("VSync");
        ImGui::Checkbox("VSync", &vsyncOn);
        ImGui::End();

        pointDiffuse.x = lightCubeColor.x * pointDiffuseValue;
        pointDiffuse.y = lightCubeColor.y * pointDiffuseValue;
        pointDiffuse.z = lightCubeColor.z * pointDiffuseValue;

        pointAmbient.x = pointDiffuse.x * pointAmbientValue;
        pointAmbient.y = pointDiffuse.y * pointAmbientValue;
        pointAmbient.z = pointDiffuse.z * pointAmbientValue;

        /*if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }*/

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // directional light
        lightingShader.setVec3("dirLight.direction", dirDirection.x, dirDirection.y, dirDirection.z);
        lightingShader.setVec3("dirLight.ambient", dirAmbient.x, dirAmbient.y, dirAmbient.z);
        lightingShader.setVec3("dirLight.diffuse", dirDiffuse.x, dirDiffuse.y, dirDiffuse.z);
        lightingShader.setVec3("dirLight.specular", dirSpecular.x, dirSpecular.y, dirSpecular.z);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", pointAmbient.x, pointAmbient.y, pointAmbient.z);
        lightingShader.setVec3("pointLights[0].diffuse", pointDiffuse.x, pointDiffuse.y, pointDiffuse.z);
        lightingShader.setVec3("pointLights[0].specular", pointSpecular.x, pointSpecular.y, pointSpecular.z);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", pointLinear);
        lightingShader.setFloat("pointLights[0].quadratic", pointQuadratic);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", pointAmbient.x, pointAmbient.y, pointAmbient.z);
        lightingShader.setVec3("pointLights[1].diffuse", pointDiffuse.x, pointDiffuse.y, pointDiffuse.z);
        lightingShader.setVec3("pointLights[1].specular", pointSpecular.x, pointSpecular.y, pointSpecular.z);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", pointLinear);
        lightingShader.setFloat("pointLights[1].quadratic", pointQuadratic);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", pointAmbient.x, pointAmbient.y, pointAmbient.z);
        lightingShader.setVec3("pointLights[2].diffuse", pointDiffuse.x, pointDiffuse.y, pointDiffuse.z);
        lightingShader.setVec3("pointLights[2].specular", pointSpecular.x, pointSpecular.y, pointSpecular.z);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", pointLinear);
        lightingShader.setFloat("pointLights[2].quadratic", pointQuadratic);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", pointAmbient.x, pointAmbient.y, pointAmbient.z);
        lightingShader.setVec3("pointLights[3].diffuse", pointDiffuse.x, pointDiffuse.y, pointDiffuse.z);
        lightingShader.setVec3("pointLights[3].specular", pointSpecular.x, pointSpecular.y, pointSpecular.z);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", pointLinear);
        lightingShader.setFloat("pointLights[3].quadratic", pointQuadratic);
        //cubeLights.setLightValues(0, lightingShader);
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", spotAmbient.x, spotAmbient.y, spotAmbient.z);
        lightingShader.setVec3("spotLight.diffuse", spotDiffuse.x, spotDiffuse.y, spotDiffuse.z);
        lightingShader.setVec3("spotLight.specular", spotSpecular.x, spotSpecular.y, spotSpecular.z);

        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", spotLinear);
        lightingShader.setFloat("spotLight.quadratic", spotQuadratic);
        if(spotLightOn)
        {
            lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotCutOff)));
            lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotOuterCutOff))); 
        }
        else
        {
            lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(0.0f)));
            lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(0.0f))); 
        }


        // material properties
        lightingShader.setFloat("material.shininess", 64.0f);


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setMat4("model", model);
        ourModel.Draw(lightingShader);

 // also draw the lamp objects
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setVec3("lightCubeColor", lightCubeColor.x, lightCubeColor.y, lightCubeColor.z);

        glBindVertexArray(lightCubeVAO);

        for(unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //cubeLights.Draw(lightCubeShader);
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
void processInput(GLFWwindow *window)
{
    if(!escapePressed)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
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

    if(!escapePressed)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
        //glfwSetWindowShouldClose(window, true);
            if(!escapePressed)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                escapePressed = true;
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                escapePressed = false;
            }
        
        }

        if (key == GLFW_KEY_F && action == GLFW_PRESS)
        {
            if(spotLightOn)
            {
                spotLightOn = false;
            }
            else
            {
                spotLightOn = true;
            }
        }
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

     int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if(data)
    {
        GLenum format;
        if(nrComponents == 1)
            format = GL_RED;
        else if(nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}