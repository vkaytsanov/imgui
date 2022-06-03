#pragma once

#include "Types/Containers/ContainersFwd.h"
#include "Types/BasicTypes.h"
#include "Types/TypeTraits.h"
#include "Reflection/Enum.h"


class Object;
class Field;
class Texture;
class Mesh;
struct Material;
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

namespace ImGui
{
void NewLine();
}

namespace ImGuiEx
{
void PushDefaultInputStyle();
void PopDefaultInputStyle();
void PushZeroPadding();
void PopZeroPadding();
void PushReadOnly();
void PopReadOnly();

bool InputResizableString(const char* label, String* str, int flags = 0);

bool DisplayValue(Entity& value);
bool DisplayValue(bool& value);
bool DisplayValue(int8& value);
bool DisplayValue(uint8& value);
bool DisplayValue(int16& value);
bool DisplayValue(uint16& value);
bool DisplayValue(int32& value);
bool DisplayValue(uint32& value);
bool DisplayValue(int64& value);
bool DisplayValue(uint64& value);
bool DisplayValue(float& value);
bool DisplayValue(double& value);
bool DisplayValue(String& value);
bool DisplayValue(HString& value);
bool DisplayValue(Color& value);
bool DisplayValue(Resource*& value);
bool DisplayValue(const Guid& value);
bool DisplayValue(Object* object);
bool DisplayEnum(Enum* enumClass, uint64& currentValue);
bool DisplayDummyValue(const char* className);
bool DisplayVectorGroup(int32* value, uint32 count);
bool DisplayVectorGroup(float* value, uint32 count);

bool BeginFieldNameColumn(const Field* field);
void EndFieldNameColumn(const Field* field);

bool BeginArrayColumn(uint32& size, const Field* field);
void DisplayArrayColumnItem(uint32 index);


template <typename T>
bool DisplayValue(Vector4<T>& value)
{
    ASSERT(sizeof(T) == 4, "Not Implemented in DisplayVectorGroup");
    return DisplayVectorGroup(&value[0], 4);
}

template <typename T>
bool DisplayValue(Vector3<T>& value)
{
    ASSERT(sizeof(T) == 4, "Not Implemented in DisplayVectorGroup");
    return DisplayVectorGroup(&value[0], 3);
}

template <typename T>
bool DisplayValue(Vector2<T>& value)
{
    ASSERT(sizeof(T) == 4, "Not Implemented in DisplayVectorGroup");
    return DisplayVectorGroup(&value[0], 2);
}

template <typename T>
typename EnableIf<IsEnum<T>::Value, bool>::Type DisplayValue(T& enumValue)
{
    bool result = false;
    if (Enum* reflectedEnum = Enum::GetEnum<T>())
    {
        uint64 enumAsUint64 = static_cast<typename UnderlyingType<T>::Type>(enumValue);
        result |= DisplayEnum(reflectedEnum, enumAsUint64);
        enumValue = static_cast<T>(enumAsUint64);
    }
    return result;
}

template <typename T, typename Allocator>
bool DisplayValue(Array<T, Allocator>& prop)
{
    bool hasChanged = false;
    uint32 size = prop.GetSize();
    if (DisplayValue(size))
    {
        if (size != prop.GetSize())
        {
            prop.Resize(size);
        }
        hasChanged = true;
    }

    for (uint32 i = 0; i < size; i++)
    {
        DisplayArrayColumnItem(i);
        hasChanged |= DisplayValue(prop[i]);
    }
    return hasChanged;
}

template <typename T>
bool DisplayValue(SharedPtr<T>& prop)
{
    return DisplayValue(prop.Get());
}

template <typename T>
typename EnableIf<!IsBaseOf<Object, T>::Value, bool>::Type DisplayValue(T* prop)
{
    return DisplayValue(*prop);
}

template <typename T>
typename EnableIf<IsBaseOf<Resource, T>::Value, bool>::Type DisplayValue(T*& prop)
{
    return DisplayValue((Resource*&)prop);
}

template <typename T>
bool DisplayFieldWithValue(T& value, const Field* field)
{
    bool result = false;
    if (BeginFieldNameColumn(field))
    {
        result |= DisplayValue(value);
        EndFieldNameColumn(field);
    }
    return result;
}


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
