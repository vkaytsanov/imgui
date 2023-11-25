#include "imgui_user.h"
#include "Types/Color.h"
#include "Types/EngineString.h"
#include "Types/Containers/ArrayView.h"
#include "Types/Containers/HashMap.h"
#include "Platform/Cursor.h"
#include "Logging/LogManager.h"
#include "Math/Math.h"
#include "Reflection/Enum.h"
#include "Reflection/Field.h"
#include "Reflection/Class.h"
#include "Reflection/Reflection.h"
#include "Parsers/StringParser.h"

#include <imgui.h>
#include <imgui_internal.h>


namespace ImGuiEx
{
using namespace ImGui;

void PushDefaultInputStyle()
{
	PushStyleColor(ImGuiCol_FrameBg, Color::DarkGrey);
	PushStyleColor(ImGuiCol_Border, Color::VeryDarkGrey);
	PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
	PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3.5));
}

void PopDefaultInputStyle()
{
	PopStyleColor(2);
	PopStyleVar(3);
}

void PushZeroPadding()
{
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
}

void PopZeroPadding()
{
    PopStyleVar(3);
}

void PushReadOnly()
{
    PushItemFlag(ImGuiItemFlags_ReadOnly, true);
    PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    PushStyleColor(ImGuiCol_FrameBg, Color::LightGrey);
    PushStyleColor(ImGuiCol_Border, Color::VeryDarkGrey);
}

void PopReadOnly()
{
    PopStyleColor(3);
    PopItemFlag();
}


int32 DefaultStringResizedCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        String* inStr = static_cast<String*>(data->UserData);
        if (data->BufTextLen)
        {
            inStr->Resize(data->BufTextLen + 1);
            data->Buf = inStr->Data();
        }
    }
    return 0;
}

bool InputText(const char* label, String& data, int32 flags, int (*callback)(ImGuiInputTextCallbackData*), void* user_data)
{
    return ImGui::InputText(label, data.Data(), callback ? data.Size() + 1: data.Capacity(), flags, callback, user_data);
}

bool InputText(const char* label, TmpString& data, int32 flags, int (*callback)(ImGuiInputTextCallbackData*), void* user_data)
{
    return ImGui::InputText(label, data.Data(), callback ? data.Size() + 1 : data.Capacity(), flags, callback, user_data);
}

bool InputTextN(ArrayView<const char*> labels, ArrayView<TmpString> datas, int32 flags)
{
    bool isModified = false;
    float charWidth = ImGui::CalcTextSize(labels[0]).x;
    float inputWidth = Math::Min(((ImGui::CalcItemWidth() / float(datas.Size())) - (datas.Size() * charWidth)), 60.f);

    ImGui::BeginGroup();
    ImGui::PushID(&datas);
    for (uint32 i = 0; i < datas.Size(); i++)
    {
        if (i > 0)
        {
            ImGui::SameLine(0.0f);
        }

        ImGui::BeginGroup();
        {
            ImGuiEx::IDScope IDScope(&labels[i]);
            ImGui::SetNextItemWidth(charWidth);

            ImGuiEx::TextAligned(labels[i]);

            ImGui::SameLine(0.0f);
            ImGui::SetNextItemWidth(inputWidth);

            isModified |= InputText("##Label", datas[i], flags);
        }
        ImGui::EndGroup();
    }
    ImGui::PopID();
    ImGui::EndGroup();
    return isModified;
}

bool InputEnum(const Enum* enumClass, TmpString& data, int64 currentValue)
{
    bool isModified = false;
    if (ImGui::BeginCombo("##Enum", *data))
    {
        for (EnumIterator iter(enumClass); iter; ++iter)
        {
            bool isSelected = iter->Value == currentValue;
            if (ImGui::Selectable(iter->Name, isSelected))
            {
                data = iter->Name;
                isModified = !isSelected;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return isModified;
}

bool InputClass(Class* objClass, TmpString& data)
{
    bool isModified = false;
    if (ImGui::BeginCombo("##Class", *data))
    {
        Array<Class*> classes;
        GetClassesOfType(objClass, classes);

        Class* selectedClass = Reflection::GetClass(*data);
        for (Class* iter : classes)
        {
            bool isSelected = iter == selectedClass;
            if (ImGui::Selectable(iter->GetName(), isSelected))
            {
                data = iter->GetName();
                isModified = !isSelected;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return isModified;
}

template <typename T>
bool SliderTextTyped(ImGuiDataType_ dataType, const char* label, TmpString& data, const String& minStr, const String& maxStr)
{
    T value, min, max;
    ENSURE(StringParser::ParseNumber(minStr, min));
    ENSURE(StringParser::ParseNumber(maxStr, max));
    ENSURE(StringParser::ParseNumber(data, value));
    bool isModified = ImGui::SliderScalar(label, dataType, &value, &min, &max);
    if (isModified)
    {
        data = value;
    }
    return isModified;
}

bool SliderText(const char* label, TmpString& data, const String& minStr, const String& maxStr, Field* field)
{
    if (field->Is<Int8Field>())
    {
        return SliderTextTyped<int8>(ImGuiDataType_S8, label, data, minStr, maxStr);
    }
    if (field->Is<UInt8Field>())
    {
        return SliderTextTyped<uint8>(ImGuiDataType_U8, label, data, minStr, maxStr);
    }

    if (field->Is<Int16Field>())
    {
        return SliderTextTyped<int16>(ImGuiDataType_S16, label, data, minStr, maxStr);
    }
    if (field->Is<UInt16Field>())
    {
        return SliderTextTyped<uint16>(ImGuiDataType_U16, label, data, minStr, maxStr);
    }

    if (field->Is<Int32Field>())
    {
        return SliderTextTyped<int32>(ImGuiDataType_S32, label, data, minStr, maxStr);
    }
    if (field->Is<UInt32Field>())
    {
        return SliderTextTyped<uint32>(ImGuiDataType_U32, label, data, minStr, maxStr);
    }

    if (field->Is<Int64Field>())
    {
        return SliderTextTyped<int64>(ImGuiDataType_S64, label, data, minStr, maxStr);
    }
    if (field->Is<UInt64Field>())
    {
        return SliderTextTyped<uint64>(ImGuiDataType_U64, label, data, minStr, maxStr);
    }

    if (field->Is<FloatField>())
    {
        return SliderTextTyped<float>(ImGuiDataType_Float, label, data, minStr, maxStr);
    }
    if (field->Is<DoubleField>())
    {
        return SliderTextTyped<double>(ImGuiDataType_Double, label, data, minStr, maxStr);
    }
    return false;
}

bool InputResizableString(const char* label, String* inStr, int32 flags)
{
    return ImGui::InputText(label, inStr->Data(), inStr->Length() + 1, flags | ImGuiInputTextFlags_CallbackResize, DefaultStringResizedCallback, inStr);
}

void Label(const char* text)
{
    ImGuiWindow* window = GetCurrentWindow();
    float fullWidth = GetContentRegionAvail().x;
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImRect textRect;
    textRect.Min = ImGui::GetCursorScreenPos();
    textRect.Max = textRect.Min + ImVec2(fullWidth * 0.3f, textSize.y);

    ImGui::AlignTextToFramePadding();
    textRect.Min.y += window->DC.CurrLineTextBaseOffset;
    textRect.Max.y += window->DC.CurrLineTextBaseOffset;

    ImGui::ItemSize(textRect);
    if (ImGui::ItemAdd(textRect, window->GetID(text)))
    {
        ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(),
                                  textRect.Min,
                                  textRect.Max,
                                  textRect.Max.x,
                                  textRect.Max.x,
                                  text,
                                  nullptr,
                                  &textSize);
    }
}

void Tooltip(const String& description)
{
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8, 4 });
        ImGui::BeginTooltipEx(ImGuiTooltipFlags_OverridePrevious, ImGuiWindowFlags_None);

        ImGui::TextUnformatted(description.begin(), description.end() - 1);

        ImGui::EndTooltip();
        ImGui::PopStyleVar();
    }
}

bool IconButton(const char* icon, const ImVec2& size, const char* description)
{
    PushStyleColor(ImGuiCol_Button, Color::LightGrey);
    PushStyleColor(ImGuiCol_Border, Color::VeryDarkGrey);
    PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
    PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));

    bool res = ImGui::Button(icon, size);
    Tooltip(description);
    PopStyleVar(2);
    PopStyleColor(2);
	return res;
}

bool ButtonDisabled(const char* label, const ImVec2& size, bool condition)
{
    BeginDisabled(condition);
    const bool result = ImGui::Button(label, size);
    EndDisabled();
    return result;
}

void TextIcon(const char* icon, const char* text)
{
    ImGui::SameLine(0, 4.0f);
    ImGui::TextUnformatted(icon);

    ImGui::SameLine(0, 4.0f);
    ImGui::TextUnformatted(text);
}

void TextAligned(const String& text, float offsetFromStart)
{
    AlignTextToFramePadding();
    ImGui::TextUnformatted(text.begin(), text.end() - 1);
}

void TextAligned(const char* text, float offsetFromStart)
{
    AlignTextToFramePadding();
    ImGui::TextUnformatted(text, nullptr);
}

void TextCentered(const char* text)
{
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) * 0.5f);
    ImGui::TextUnformatted(text);
}

void TextCenteredInColumn(const char* text)
{
    ImGui::SetCursorPosX((ImGui::GetColumnWidth() - ImGui::CalcTextSize(text).x) * 0.5f);
    ImGui::TextUnformatted(text);
}

void TextColored(StringView<char> text, Color color)
{
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextUnformatted(text.begin(), text.end());
    ImGui::PopStyleColor();
}

void TextKeyValue(StringView<char> key, StringView<char> value)
{
    ImGui::TextUnformatted(key.begin(), key.end());
    ImGui::SameLine();
    ImGui::TextUnformatted(":");
    ImGui::SameLine();
    ImGui::TextUnformatted(value.begin(), value.end());
}

void Centered(float width)
{
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
}

void Centered(const ImVec2& size)
{
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - size.x) * 0.5f);
}

bool Search(String* buffer, float width)
{
    bool result = false;
    ImGuiEx::PushDefaultInputStyle();
    ImGui::PushItemWidth(width);
    ImGui::PushID("Search");
    if (ImGuiEx::InputResizableString("", buffer, ImGuiInputTextFlags_AlwaysOverwrite | ImGuiInputTextFlags_AutoSelectAll))
    {
        result = true;
    }
    ImGui::PopID();

    ImGuiEx::PopDefaultInputStyle();
    ImGui::PopItemWidth();

    return result;
}

CursorType GetMouseCursor()
{
    switch (ImGui::GetMouseCursor())
    {
    case ImGuiMouseCursor_Arrow:      return Cursor_Arrow;
    case ImGuiMouseCursor_TextInput:  return Cursor_TextInput;
    case ImGuiMouseCursor_ResizeAll:  return Cursor_Resize;
    case ImGuiMouseCursor_ResizeEW:   return Cursor_ResizeEastWest;
    case ImGuiMouseCursor_ResizeNS:   return Cursor_ResizeNorthSouth;
    case ImGuiMouseCursor_ResizeNWSE: return Cursor_ResizeNorthWestSouthEast;
    case ImGuiMouseCursor_ResizeNESW: return Cursor_ResizeNorthEastSouthWest;
    case ImGuiMouseCursor_Hand:       return Cursor_Hand;
    case ImGuiMouseCursor_NotAllowed: return Cursor_Forbidden;
    case ImGuiMouseCursor_None:
        UNREACHED;
        break;
    default:
        LOG_W(None, "ImGui cursor not implemented!");
        return Cursor_Arrow;
    }
    return Cursor_Arrow;
}

IDScope::IDScope(const void* ptr_id)
{
    ImGui::PushID(ptr_id);
}

IDScope::IDScope(int int_id)
{
    ImGui::PushID(int_id);
}

IDScope::~IDScope()
{
    ImGui::PopID();
}

}
