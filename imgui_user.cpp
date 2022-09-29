#include "imgui.h"
#include "imgui_internal.h"
#include "Types/Color.h"
#include "Types/String.h"
#include "Types/Containers/HashMap.h"


namespace ImGuiEx
{
using namespace ImGui;

void PushDefaultInputStyle()
{
	PushStyleColor(ImGuiCol_FrameBg, Color::DARK_GREY);
	PushStyleColor(ImGuiCol_Border, Color::VERY_DARK_GREY);
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
    PushStyleColor(ImGuiCol_FrameBg, Color::LIGHT_GREY);
    PushStyleColor(ImGuiCol_Border, Color::VERY_DARK_GREY);
}

void PopReadOnly()
{
    PopStyleColor(3);
    PopItemFlag();
}


int32 StringResizedCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        String* inStr = static_cast<String*>(data->UserData);
        ASSERT(inStr->Data() == data->Buf);
        inStr->Resize(data->BufTextLen ? data->BufTextLen + 1 : 1);
        data->Buf = inStr->Data();
    }
    return 0;
}

bool InputText(const char* label, String& data, int32 flags)
{
    return ImGui::InputText(label, data.Data(), data.GetCapacity(), flags);
}

bool InputText(const char* label, TmpString& data, int32 flags)
{
    return ImGui::InputText(label, data.Data(), data.GetCapacity(), flags);
}

bool InputResizableString(const char* label, String* inStr, int32 flags)
{
    return ImGui::InputText(label, inStr->Data(), inStr->GetLength() + 1, flags | ImGuiInputTextFlags_CallbackResize, StringResizedCallback, inStr);
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

void Tooltip(const char* description)
{
    if (description && ImGui::IsItemHovered())
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8, 4 });
        ImGui::BeginTooltipEx(ImGuiTooltipFlags_OverridePreviousTooltip, ImGuiWindowFlags_None);

        ImGui::TextUnformatted(description);

        ImGui::EndTooltip();
        ImGui::PopStyleVar();
    }
}

bool IconButton(const char* icon, const ImVec2& size, const char* description)
{
    PushStyleColor(ImGuiCol_Button, Color::LIGHT_GREY);
    PushStyleColor(ImGuiCol_Border, Color::VERY_DARK_GREY);
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
}
