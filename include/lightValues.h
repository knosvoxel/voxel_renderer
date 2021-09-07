#ifndef LIGHTVALUES_H
#define LIGHTVALUES_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

struct LightValues
{
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
};

#endif