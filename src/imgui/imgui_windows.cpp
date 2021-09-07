#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "learnopengl/shader.h"
#include "ownImgui.h"
#include "lightValues.h"

void ShowLightSettingsWindow(bool *window_active, struct LightValues &lightValues, bool *spotLightActive, float &cameraMovementSpeed, float &cameraBoostSpeed, Shader lightingShader)
{
    if (*window_active)
    {
        ImGui::Begin("Lights");

        ImGui::Text("Directional Light");
        ImGui::DragFloat3("Direction", (float *)&lightValues.dirDirection, 0.01f);
        ImGui::ColorEdit3("Dir Ambient", (float *)&lightValues.dirAmbient);
        ImGui::ColorEdit3("Dir Diffuse", (float *)&lightValues.dirDiffuse);
        ImGui::ColorEdit3("Dir Specular", (float *)&lightValues.dirSpecular);

        ImGui::Text("Point Lights");
        ImGui::ColorEdit3("Point Light Color", (float *)&lightValues.lightCubeColor);
        ImGui::DragFloat("Point Ambient", (float *)&lightValues.pointAmbientValue, 0.001f);
        if (lightValues.pointAmbientValue > 1.0f)
            lightValues.pointAmbientValue = 1.0f;
        if (lightValues.pointAmbientValue < 0.0f)
            lightValues.pointAmbientValue = 0.0f;
        ImGui::DragFloat("Point Diffuse", (float *)&lightValues.pointDiffuseValue, 0.001f);
        if (lightValues.pointDiffuseValue > 1.0f)
            lightValues.pointDiffuseValue = 1.0f;
        if (lightValues.pointDiffuseValue < 0.0f)
            lightValues.pointDiffuseValue = 0.0f;
        ImGui::ColorEdit3("Point Specular", (float *)&lightValues.pointSpecular);
        ImGui::InputFloat("Point Linear", &lightValues.pointLinear);
        ImGui::InputFloat("Point Quadratic", &lightValues.pointQuadratic);

        ImGui::Text("Spotlight");
        ImGui::ColorEdit3("Spot Ambient", (float *)&lightValues.spotAmbient);
        ImGui::ColorEdit3("Spot Diffuse", (float *)&lightValues.spotDiffuse);
        ImGui::ColorEdit3("Spot Specular", (float *)&lightValues.spotSpecular);
        ImGui::InputFloat("Spot Linear", &lightValues.spotLinear);
        ImGui::InputFloat("Spot Quadratic", &lightValues.spotQuadratic);
        ImGui::InputFloat("Cutoff", &lightValues.spotCutOff);
        ImGui::InputFloat("Outer Cutoff", &lightValues.spotOuterCutOff);
        ImGui::Checkbox("Spotlight Active", spotLightActive);

        ImGui::Text("Camera");
        ImGui::InputFloat("Movement Speed", &cameraMovementSpeed);
        ImGui::InputFloat("Boosted Speed", &cameraBoostSpeed);
        ImGui::End();
    }

    lightValues.pointDiffuse.x = lightValues.lightCubeColor.x * lightValues.pointDiffuseValue;
    lightValues.pointDiffuse.y = lightValues.lightCubeColor.y * lightValues.pointDiffuseValue;
    lightValues.pointDiffuse.z = lightValues.lightCubeColor.z * lightValues.pointDiffuseValue;

    lightValues.pointAmbient.x = lightValues.pointDiffuse.x * lightValues.pointAmbientValue;
    lightValues.pointAmbient.y = lightValues.pointDiffuse.y * lightValues.pointAmbientValue;
    lightValues.pointAmbient.z = lightValues.pointDiffuse.z * lightValues.pointAmbientValue;

    lightingShader.use();

    // directional light
    lightingShader.setVec3("dirLight.direction", lightValues.dirDirection.x, lightValues.dirDirection.y, lightValues.dirDirection.z);
    lightingShader.setVec3("dirLight.ambient", lightValues.dirAmbient.x, lightValues.dirAmbient.y, lightValues.dirAmbient.z);
    lightingShader.setVec3("dirLight.diffuse", lightValues.dirDiffuse.x, lightValues.dirDiffuse.y, lightValues.dirDiffuse.z);
    lightingShader.setVec3("dirLight.specular", lightValues.dirSpecular.x, lightValues.dirSpecular.y, lightValues.dirSpecular.z);
    // point light 1
    lightingShader.setVec3("pointLights[0].ambient", lightValues.pointAmbient.x, lightValues.pointAmbient.y, lightValues.pointAmbient.z);
    lightingShader.setVec3("pointLights[0].diffuse", lightValues.pointDiffuse.x, lightValues.pointDiffuse.y, lightValues.pointDiffuse.z);
    lightingShader.setVec3("pointLights[0].specular", lightValues.pointSpecular.x, lightValues.pointSpecular.y, lightValues.pointSpecular.z);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", lightValues.pointLinear);
    lightingShader.setFloat("pointLights[0].quadratic", lightValues.pointQuadratic);
    // point light 2
    lightingShader.setVec3("pointLights[1].ambient", lightValues.pointAmbient.x, lightValues.pointAmbient.y, lightValues.pointAmbient.z);
    lightingShader.setVec3("pointLights[1].diffuse", lightValues.pointDiffuse.x, lightValues.pointDiffuse.y, lightValues.pointDiffuse.z);
    lightingShader.setVec3("pointLights[1].specular", lightValues.pointSpecular.x, lightValues.pointSpecular.y, lightValues.pointSpecular.z);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", lightValues.pointLinear);
    lightingShader.setFloat("pointLights[1].quadratic", lightValues.pointQuadratic);
    // point light 3
    lightingShader.setVec3("pointLights[2].ambient", lightValues.pointAmbient.x, lightValues.pointAmbient.y, lightValues.pointAmbient.z);
    lightingShader.setVec3("pointLights[2].diffuse", lightValues.pointDiffuse.x, lightValues.pointDiffuse.y, lightValues.pointDiffuse.z);
    lightingShader.setVec3("pointLights[2].specular", lightValues.pointSpecular.x, lightValues.pointSpecular.y, lightValues.pointSpecular.z);
    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    lightingShader.setFloat("pointLights[2].linear", lightValues.pointLinear);
    lightingShader.setFloat("pointLights[2].quadratic", lightValues.pointQuadratic);
    // point light 4
    lightingShader.setVec3("pointLights[3].ambient", lightValues.pointAmbient.x, lightValues.pointAmbient.y, lightValues.pointAmbient.z);
    lightingShader.setVec3("pointLights[3].diffuse", lightValues.pointDiffuse.x, lightValues.pointDiffuse.y, lightValues.pointDiffuse.z);
    lightingShader.setVec3("pointLights[3].specular", lightValues.pointSpecular.x, lightValues.pointSpecular.y, lightValues.pointSpecular.z);
    lightingShader.setFloat("pointLights[3].constant", 1.0f);
    lightingShader.setFloat("pointLights[3].linear", lightValues.pointLinear);
    lightingShader.setFloat("pointLights[3].quadratic", lightValues.pointQuadratic);
    //cubeLights.setLightValues(0, lightingShader);
    // spotLight
    lightingShader.setVec3("spotLight.ambient", lightValues.spotAmbient.x, lightValues.spotAmbient.y, lightValues.spotAmbient.z);
    lightingShader.setVec3("spotLight.diffuse", lightValues.spotDiffuse.x, lightValues.spotDiffuse.y, lightValues.spotDiffuse.z);
    lightingShader.setVec3("spotLight.specular", lightValues.spotSpecular.x, lightValues.spotSpecular.y, lightValues.spotSpecular.z);

    lightingShader.setFloat("spotLight.constant", 1.0f);
    lightingShader.setFloat("spotLight.linear", lightValues.spotLinear);
    lightingShader.setFloat("spotLight.quadratic", lightValues.spotQuadratic);
    if (*spotLightActive)
    {
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(lightValues.spotCutOff)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(lightValues.spotOuterCutOff)));
    }
    else
    {
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(0.0f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(0.0f)));
    }
}

void ShowPerformanceWindow(bool *window_active, bool &vsyncOn)
{
    if (*window_active)
    {
        ImGui::Begin("Performance");
        ImGui::Text("VSync");
        ImGui::Checkbox("VSync", &vsyncOn);

        ImGui::Text("Frametime: %.3f ms (FPS %.1f)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}