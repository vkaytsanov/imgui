#pragma once

#include "imgui.h"
#include "Types/String.h"

class Object;
class Field;

namespace ImGuiEx
{
void PushDefaultInputStyle();
void PopDefaultInputStyle();
bool InputResizableString(const char* label, String* str, ImGuiInputTextFlags flags = 0);

bool InputVector4f(Object* instance, const Field* prop);
bool InputString(Object* instance, const Field* prop);
bool InputMaterial(Object* instance, const Field* prop);
bool InputMeshData(Object* instance, const Field* prop);
bool InputColor(Object* instance, const Field* prop);
bool IconButton(const char* icon, const char* tooltip);
}
