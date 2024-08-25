#pragma once

#include "Types/Containers/ContainersFwd.h"
#include "Types/BasicTypes.h"
#include "Math/MathFwd.h"
#include "imconfig.h"


class Enum;
class Object;
class Field;
class Texture;
class StaticMeshResource;
class Material;
struct ImVec2;
class Texture2D;
class Resource;
class Texture;
class Guid;
class Entity;
class Class;
enum CursorType : uint8;

template <typename T>
class SharedPtr;

struct ImGuiInputTextCallbackData;

namespace ImGuiEx
{
IMGUI_API void PushDefaultInputStyle();
IMGUI_API void PopDefaultInputStyle();
IMGUI_API void PushZeroPadding();
IMGUI_API void PopZeroPadding();
IMGUI_API void PushReadOnly();
IMGUI_API void PopReadOnly();

IMGUI_API int32 DefaultStringResizedCallback(ImGuiInputTextCallbackData* data);
IMGUI_API bool InputText(const char* label, String& data, int32 flags = 16 | 32 /*ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue*/, int (*callback)(ImGuiInputTextCallbackData*) = nullptr, void* user_data = nullptr);
IMGUI_API bool InputText(const char* label, TmpString& data, int32 flags = 16 | 32 /*ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue*/, int (*callback)(ImGuiInputTextCallbackData*) = nullptr, void* user_data = nullptr);
IMGUI_API bool InputTextN(ArrayView<const char*> labels, ArrayView<TmpString> datas, int32 flags = 16 | 32 /*ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue*/);
IMGUI_API bool InputResizableString(const char* label, String* inStr, int32 flags = 16 | 32 /*ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue*/);
IMGUI_API bool InputEnum(const Enum* enumClass, TmpString& data, int64 currentValue);
IMGUI_API bool InputClass(Class* objClass, TmpString& data);
IMGUI_API bool SliderText(const char* label, TmpString& data, const String& minStr, const String& maxStr, Field* field);

IMGUI_API void Label(const char* text);
IMGUI_API void Tooltip(const String& description);
IMGUI_API bool IconButton(const char* icon, const ImVec2& size, const char* description);

IMGUI_API bool ButtonDisabled(const char* label, const ImVec2& size, bool condition);
IMGUI_API void TextIcon(const char* icon, const char* text);
IMGUI_API void TextAligned(const String& text, float offsetFromStart = 0.0f);
IMGUI_API void TextAligned(const char* text, float offsetFromStart = 0.0f);
IMGUI_API void TextCentered(const char* text);
IMGUI_API void TextCenteredInColumn(const char* text);
IMGUI_API void TextColored(StringView<char> text, Color color);
IMGUI_API void TextKeyValue(StringView<char> key, StringView<char> value);
IMGUI_API void Centered(float width);
IMGUI_API void Centered(const ImVec2& size);
IMGUI_API bool Search(String* buffer, float width);
IMGUI_API CursorType GetMouseCursor();
IMGUI_API void FullscreenTexture(ImTextureID texture);

struct IMGUI_API IDScope
{
    IDScope(const void* ptr_id);
    IDScope(int int_id);
    ~IDScope();
};

struct IMGUI_API StyleScope
{
    StyleScope(int idx, Vector2f val);
    ~StyleScope();
};

}
