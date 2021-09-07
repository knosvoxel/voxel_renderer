#ifndef OWNIMGUI_H
#define OWNIMGUI_H

extern void ShowLightSettingsWindow(bool *window_active, struct LightValues &lightValues, bool *spotLightActive, float &cameraMovementSpeed, float &cameraBoostSpeed, Shader shader);
extern void ShowPerformanceWindow(bool *window_active, bool &vsyncOn);

#endif