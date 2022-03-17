#pragma once

#include "Types/String.h"

class Object;
class Field;
template <typename T>
struct Vector4;
struct Material;
struct Color;
struct ImVec2;

template <typename T>
class SharedPtr;


namespace ImGuiEx
{
void PushDefaultInputStyle();
void PopDefaultInputStyle();
void PushZeroPadding();
void PopZeroPadding();
bool InputResizableString(const char* label, String* str, int flags = 0);

bool Input(Vector4<float>& prop, const Field* field);
bool Input(String& prop, const Field* field);
bool Input(Material& prop, const Field* field);
bool Input(Color& prop, const Field* field);

bool InputSubClassBegin(const Field* field);
void InputSubClassEnd();

bool InputSubArrayBegin(const Field* field);
void InputSubArrayItem(uint32 index);
void InputSubArrayEnd();

template <typename T>
typename EnableIf<!IsBaseOf<Object, T>::Value, bool>::Type Input(T& notImplemented, const Field* field)
{
    return false;
}

template <typename T>
typename EnableIf<IsBaseOf<Object, T>::Value, bool>::Type Input(T& prop, const Field* field)
{
    if (!InputSubClassBegin(field))
    {
        return false;
    }

    prop.GetClass()->OnGUIInternal(&prop);

    InputSubClassEnd();
    return true;
}

template <typename T>
bool Input(T* prop, const Field* field)
{
    return Input(*prop, field);
}

template <typename T, typename Allocator>
bool Input(Array<T, Allocator>& prop, const Field* field)
{
    if (!InputSubArrayBegin(field))
    {
        return false;
    }

    if (prop)
    {
        for (uint32 i = 0; i < prop.GetSize(); i++)
        {
            InputSubArrayItem(i);
            //Input(prop, field);
        }
    }

    InputSubArrayEnd();
    return true;
}

template <typename T>
bool Input(SharedPtr<T>& prop, const Field* field)
{
    return Input(*prop, field);
}

bool IconButton(const char* icon, const char* tooltip);

bool ButtonDisabled(const char* label, const ImVec2& size, bool condition);
}
