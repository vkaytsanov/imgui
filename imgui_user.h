#pragma once

#include "Types/Containers/ContainersFwd.h"
#include "Types/BasicTypes.h"


class Object;
class Field;
class Texture;
class StaticMeshResource;
class Material;
struct Color;
struct ImVec2;
class Texture2D;
class Resource;
class Texture;
class Guid;
class Entity;

template <typename T>
struct Vector4;
template <typename T>
struct Vector3;
template <typename T>
struct Vector2;
template <typename T>
class SharedPtr;

namespace ImGuiEx
{
void PushDefaultInputStyle();
void PopDefaultInputStyle();
void PushZeroPadding();
void PopZeroPadding();
void PushReadOnly();
void PopReadOnly();

bool InputText(const char* label, String& data, int32 flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
bool InputText(const char* label, TmpString& data, int32 flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);

void Label(const char* text);
void Tooltip(const char* description);
bool IconButton(const char* icon, const ImVec2& size, const char* description);

bool ButtonDisabled(const char* label, const ImVec2& size, bool condition);
void TextIcon(const char* icon, const char* text);
void TextAligned(const char* text, float offsetFromStart = 0.0f);
void TextCentered(const char* text);
void TextCenteredInColumn(const char* text);
void Centered(float width);
void Centered(const ImVec2& size);
bool Search(String* buffer, float width);
}
