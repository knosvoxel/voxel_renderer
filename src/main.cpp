#define STB_IMAGE_IMPLEMENTATION
#define OGT_VOX_IMPLEMENTATION
#define OGT_VOXEL_MESHIFY_IMPLEMENTATION

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
#include "ownImgui.h"
#include "lightValues.h"

#include "vox_loading.h"

#if defined(_MSC_VER)
#include <io.h>
#endif
#include <stdio.h>

//#include <cubeLight.h>

//#include <imgui/backends/imgui_impl_opengl3.h>
//#include <imgui/backends/imgui_impl_opengl3_loader.h>
#include <iostream>
#include <experimental/filesystem>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

unsigned int loadTexture(char const *path);

const ogt_vox_scene *load_vox_scene(const char *filename, uint32_t scene_read_flags = 0);
const ogt_vox_scene *load_vox_scene_with_groups(const char *filename);
uint32_t count_solid_voxels_in_model(const ogt_vox_model *model);
void save_vox_scene(const char *pcFilename, const ogt_vox_scene *scene);
std::vector<glm::vec3> getVoxelPositions(const ogt_vox_model *model);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float cameraMovementSpeed = 4.0f;
float cameraBoostSpeed = 8.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool escapePressed = false;
static bool shiftKeyPressed = false;
bool vsyncOn = true;
static bool showSpotlight = false;
static bool showGUI = false;

namespace fs = std::experimental::filesystem;

int main()
{
    fs::current_path("../");

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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Renderer", NULL, NULL);
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
    glEnable(GL_CULL_FACE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("shaders/lighting.vs", "shaders/lighting.fs");
    Shader lightCubeShader("shaders/light_cube.vs", "shaders/light_cube.fs");

    Model ourModel("res/objects/backpack/backpack.obj");

    float cubeVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // top-left
        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // top-right
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // top-right
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-right
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-right
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-right
                                         // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    // top-left
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    // top-left
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   // bottom-left
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  // top-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // bottom-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // bottom-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-right
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
        // Top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // bottom-right
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f   // bottom-left
    };

    glm::vec3 pointLightPositions[]{
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    // second, configure the light's VAO
    unsigned int VBO, lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    static float framesPerSeconds = 0.0f;
    static float currentFPS = 0.0f;
    static float lastTime = 0.0f;

    LightValues lightValues;
    std::vector<std::vector<glm::vec3>> model_positions;

    const ogt_vox_scene *scene = load_vox_scene("res/vox/temple.vox");
    std::vector<ogt_mesh *> meshes;
    uint32_t total_voxel_count;
    std::vector<unsigned int> voxVAOs;
    std::vector<unsigned int> voxVBOs;
    std::vector<unsigned int> voxEBOs;
    //std::vector<glm::vec3> voxel_positions;
    /* for (uint32_t model_index = 0; model_index < scene->num_models; model_index++)
        {
            const ogt_vox_model *model = scene->models[model_index];
            total_voxel_count += count_solid_voxels_in_model(model);
            ogt_voxel_meshify_context ctx;
            memset(&ctx, 0, sizeof(ctx));
            ogt_mesh *mesh = ogt_mesh_from_paletted_voxels_simple(&ctx, model->voxel_data, model->size_x, model->size_y, model->size_z, (const ogt_mesh_rgba *)&scene->palette.color[0]);
            meshes.push_back(mesh);
        }

        std::cout << "Total voxel count: " << total_voxel_count << std::endl;

        ogt_vox_destroy_scene(scene);

        for (int i = 0; i < meshes.size(); ++i)
        {
            unsigned int voxVAO, voxVBO, voxEBO;

            glGenVertexArrays(1, &voxVAO);
            glGenBuffers(1, &voxVBO);
            glGenBuffers(1, &voxEBO);

            glBindVertexArray(voxVAO);
            glBindBuffer(GL_ARRAY_BUFFER, voxVBO);

            glBufferData(GL_ARRAY_BUFFER, meshes[i]->vertex_count * sizeof(ogt_mesh_vertex), &meshes[i]->vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->index_count * sizeof(uint32_t), &meshes[i]->indices[0], GL_STATIC_DRAW);

            // vertex positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ogt_mesh_vertex), (void *)0);
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ogt_mesh_vertex), (void *)offsetof(ogt_mesh_vertex, normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ogt_mesh_rgba), (void *)offsetof(ogt_mesh_vertex, color));

            glBindVertexArray(0);

            voxVAOs.push_back(voxVAO);
            voxVBOs.push_back(voxVBO);
            voxEBOs.push_back(voxEBO);*/
    GLuint global_vao = 0;
    glGenVertexArrays(1, &global_vao);
    glBindVertexArray(global_vao);

    //---------------------------------------------------------------------------------------------------
    // create a giant vertex buffer and a giant index buffer that will contain all meshes for the scene
    //---------------------------------------------------------------------------------------------------
    static const uint32_t k_vertex_buffer_size = 2000 * 2000 * 1000; // 16 MiB. bump this if it's not big enough for your scene(s)
    static const uint32_t k_index_buffer_size = 512 * 1024 * 1024;   // 4 MiB. bump this if it's not big enough for your scene(s)
    VoxLoad::buffer vertex_buffer, index_buffer;
    VoxLoad::create_static_buffer(&vertex_buffer, k_vertex_buffer_size, NULL);
    VoxLoad::create_static_buffer(&index_buffer, k_index_buffer_size, NULL);

    //---------------------------------------------------------------------------------------------------
    // turn each voxel model into a mesh and upload it to the vertex buffer and index buffer
    //---------------------------------------------------------------------------------------------------
    struct model_desc_gpu
    {
        uint32_t offset_in_vertex_buffer;
        uint32_t offset_in_index_buffer;
        uint32_t num_indices;
    };
    model_desc_gpu *model_descs = new model_desc_gpu[scene->num_models];

    // create a mesh for each model in the scene and upload to the vertex buffer and index buffer
    {
        uint32_t next_vb_offset = 0;
        uint32_t next_ib_offset = 0;
        for (uint32_t model_index = 0; model_index < scene->num_models; model_index++)
        {
            const ogt_vox_model *model = scene->models[model_index];

            // generate a mesh from the voxel field for this model
            ogt_voxel_meshify_context ctx;
            memset(&ctx, 0, sizeof(ctx));
            ogt_mesh *mesh = ogt_mesh_from_paletted_voxels_simple(&ctx, model->voxel_data, model->size_x, model->size_y, model->size_z, (const ogt_mesh_rgba *)&scene->palette.color[0]);

            // make sure this mesh can fit in the vertex buffer and index buffer
            uint32_t mesh_size_in_vb = mesh->vertex_count * sizeof(ogt_mesh_vertex);
            uint32_t mesh_size_in_ib = mesh->index_count * sizeof(uint32_t);
            assert((next_vb_offset + mesh_size_in_vb) <= k_vertex_buffer_size); // if you hit this, bump up k_vertex_buffer_size
            assert((next_ib_offset + mesh_size_in_ib) <= k_index_buffer_size);  // if you hit this, bump up k_index_buffer_size

            // update the descriptor for this model that remembers where in the VB/IB the mesh lives
            model_descs[model_index].offset_in_vertex_buffer = next_vb_offset;
            model_descs[model_index].offset_in_index_buffer = next_ib_offset;
            model_descs[model_index].num_indices = mesh->index_count;

            // upload data to the VB/IB
            next_vb_offset += mesh_size_in_vb;
            next_ib_offset += mesh_size_in_ib;

            // free the mesh (it's already been uploaded to VB/IB so is no longer needed)
            ogt_mesh_destroy(&ctx, mesh);
        }
    }
    //------------------------------------------------------------------------------------------
    // remember instance transforms and model index so we don't need to keep the scene around
    //------------------------------------------------------------------------------------------
    struct instance_desc_gpu
    {
        uint32_t model_index;        // which model in the model_descs array does this instance refer to?
        ogt_vox_transform transform; // what is the object-space -> world-space transform for this instance
    };

    instance_desc_gpu *instance_descs = new instance_desc_gpu[scene->num_instances];
    uint32_t num_instances = scene->num_instances;
    for (uint32_t i = 0; i < num_instances; i++)
    {
        instance_descs[i].model_index = scene->instances[i].model_index;
        instance_descs[i].transform = scene->instances[i].transform;
    }

    // we have all our data capture into instance_descs and model_descs at this point, so we no longer need the scene
    ogt_vox_destroy_scene(scene);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        if (vsyncOn)
        {
            glfwSwapInterval(1);
        }
        else
        {
            glfwSwapInterval(0);
        }

        if (shiftKeyPressed)
            camera.boostMovementSpeed(cameraBoostSpeed);
        if (!shiftKeyPressed)
            camera.resetMovementSpeed(cameraMovementSpeed);

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

        ShowLightSettingsWindow(&showGUI, lightValues, &showSpotlight, cameraMovementSpeed, cameraBoostSpeed, lightingShader);
        ShowPerformanceWindow(&showGUI, vsyncOn);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);

        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);

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
        //ourModel.Draw(lightingShader);

        // we have all our models in the same vertex buffer and index buffer, so we only need to bind VB/IB once
        // for all models, and not once for each model. If you decided to create a new VB/IB for each model, you'd
        // have to bind VB/IB just before drawing any instance that used that model.

        VoxLoad::bind_vertex_buffer(&vertex_buffer);
        VoxLoad::bind_index_buffer(&index_buffer);

        // iterate over all instances.
        // TODO: we could sort all instances within instance_desc by model_index and then issue less draw calls
        // by using an "instancing" shader.
        for (uint32_t instance_index = 0; instance_index < num_instances; instance_index++)
        {
            const instance_desc_gpu *instance_desc = &instance_descs[instance_index];
            const model_desc_gpu *model_desc = &model_descs[instance_desc->model_index];
            // TODO: upload the instance->transform to the correct uniform buffer.
            // TODO: bind the correct vertex/pixel shader that consumes position/normal/color from the correct input slots in the vertex shader.

            // issue the draw call.
            uint32_t base_vertex_start = model_desc->offset_in_vertex_buffer / sizeof(ogt_mesh_vertex);
            glDrawElementsInstancedBaseVertex(GL_TRIANGLES, model_desc->num_indices, GL_UNSIGNED_INT, (const void *)((uint64_t)model_desc->offset_in_index_buffer), 1, base_vertex_start);
        }

        // also draw the lamp objects
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setVec3("lightCubeColor", lightValues.lightCubeColor.x, lightValues.lightCubeColor.y, lightValues.lightCubeColor.z);

        glBindVertexArray(lightCubeVAO);

        for (unsigned int i = 0; i < 4; i++)
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

    // shutdown
    delete[] instance_descs;
    delete[] model_descs;

    // unbind and destroy vertex buffer
    VoxLoad::bind_vertex_buffer(NULL);
    VoxLoad::destroy_buffer(&vertex_buffer);

    // unbind and destroy vertex buffer
    VoxLoad::bind_index_buffer(NULL);
    VoxLoad::destroy_buffer(&index_buffer);

    // unbind/destroy the vao
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &global_vao);
    global_vao = 0;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (!escapePressed)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            shiftKeyPressed = true;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            shiftKeyPressed = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
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

    if (!escapePressed)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        //glfwSetWindowShouldClose(window, true);
        if (!escapePressed)
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
        if (showSpotlight)
        {
            showSpotlight = false;
        }
        else
        {
            showSpotlight = true;
        }
    }

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        if (showGUI)
        {
            showGUI = false;
        }
        else
        {
            showGUI = true;
        }
    }
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
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

const ogt_vox_scene *load_vox_scene(const char *filename, uint32_t scene_read_flags)
{
    // open the file
#if defined(_MSC_VER) && _MSC_VER >= 1400
    FILE *fp;
    if (0 != fopen_s(&fp, filename, "rb"))
        fp = 0;
#else
    FILE *fp = fopen(filename, "rb");
#endif
    if (!fp)
        return NULL;

    // get the buffer size which matches the size of the file
    fseek(fp, 0, SEEK_END);
    uint32_t buffer_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // load the file into a memory buffer
    uint8_t *buffer = new uint8_t[buffer_size];
    fread(buffer, buffer_size, 1, fp);
    fclose(fp);

    // construct the scene from the buffer
    const ogt_vox_scene *scene = ogt_vox_read_scene_with_flags(buffer, buffer_size, scene_read_flags);

    // the buffer can be safely deleted once the scene is instantiated
    delete[] buffer;

    return scene;
}

const ogt_vox_scene *load_vox_scene_with_groups(const char *filename)
{
    return load_vox_scene(filename, k_read_scene_flags_groups);
}

std::vector<glm::vec3> getVoxelPositions(const ogt_vox_model *model)
{
    std::vector<glm::vec3> voxel_positions;
    uint32_t voxel_index = 0;
    //uint8_t counter = 0;
    for (uint32_t z = 0; z < model->size_z; z++)
    {
        for (uint32_t y = 0; y < model->size_y; y++)
        {
            for (uint32_t x = 0; x < model->size_x; x++, voxel_index++)
            {
                // if color index == 0, this voxel is empty, otherwise it is solid.
                uint32_t color_index = model->voxel_data[voxel_index];
                if (color_index != 0)
                    voxel_positions.push_back(glm::vec3(x, y, z));
            }
        }
    }
    return voxel_positions;
}
uint32_t count_solid_voxels_in_model(const ogt_vox_model *model)
{
    uint32_t solid_voxel_count = 0;
    uint32_t voxel_index = 0;
    //uint8_t counter = 0;
    for (uint32_t z = 0; z < model->size_z; z++)
    {
        for (uint32_t y = 0; y < model->size_y; y++)
        {
            for (uint32_t x = 0; x < model->size_x; x++, voxel_index++)
            {
                // if color index == 0, this voxel is empty, otherwise it is solid.
                uint32_t color_index = model->voxel_data[voxel_index];
                /*if(color_index != 0)
                    std::cout << color_index << ' ';
                if(counter >= 10)
                    std::cout << std::endl;*/

                bool is_voxel_solid = (color_index != 0);
                // add to our accumulator
                solid_voxel_count += (is_voxel_solid ? 1 : 0);
            }
        }
    }
    return solid_voxel_count;
}

void save_vox_scene(const char *pcFilename, const ogt_vox_scene *scene)
{
    // save the scene back out.
    uint32_t buffersize = 0;
    uint8_t *buffer = ogt_vox_write_scene(scene, &buffersize);
    if (!buffer)
        return;

        // open the file for write
#if defined(_MSC_VER) && _MSC_VER >= 1400
    FILE *fp;
    if (0 != fopen_s(&fp, pcFilename, "wb"))
        fp = 0;
#else
    FILE *fp = fopen(pcFilename, "wb");
#endif
    if (!fp)
    {
        ogt_vox_free(buffer);
        return;
    }

    fwrite(buffer, buffersize, 1, fp);
    fclose(fp);
    ogt_vox_free(buffer);
}