#include <Core/Core.h>

#include "imgui.h"
#include "Types/ResourceTypes.h"
#include "Types/String.h"
#include "Math/Vector4.h"

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


int StringResizedCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		String* inStr = static_cast<String*>(data->UserData);
		IM_ASSERT(inStr->Data() == data->Buf);
        inStr->Resize(data->BufTextLen ? data->BufTextLen + 1 : 1); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
		data->Buf = inStr->Data();
	}
	return 0;
}

bool InputResizableString(const char* label, String* inStr, ImGuiInputTextFlags flags)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	return ImGui::InputText(label,
	                        inStr->Data(),
	                        inStr->GetLength() + 1,
	                        flags | ImGuiInputTextFlags_CallbackResize, StringResizedCallback,
	                        inStr);
}


bool InputSubClassBegin(const Field* field)
{
    SameLine(12);
    AlignTextToFramePadding();
    return ImGui::TreeNode(field->GetEditorName());
}

void InputSubClassEnd()
{
    ImGui::TreePop();
}

void InputSubArrayItem(uint32 index)
{
    String number = String::Printf("%d", index);
    TextAligned(number.CStr(), 25);
    SameLine(25);
}


bool Input(Vector4f& prop, const Field* field)
{
	bool result = false;
	auto inputFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal;

    PushDefaultInputStyle();
	PushItemWidth(55);

    TextAligned(field->GetEditorName(), 12);

    const char* axes[] = { "X", "Y", "Z" };
    for (uint32 i = 0; i < 3; i++)
    {
        PushID(field->GetEditorName() + i + 1);
        TextAligned(axes[i], 75.f * (static_cast<float>(i) + 1.f));
        SameLine(0, 5);

        result |= InputFloat("", &prop[i], 0, 0, "%.3f", inputFlags);

        PopID();
    }

    PopDefaultInputStyle();
	PopItemWidth();

	return result;
}

bool Input(String& prop, const Field* field)
{
	bool result = false;
	PushDefaultInputStyle();

    // Copy here to avoid immediate renaming of the field
	String buffer = prop;
    TextAligned(field->GetEditorName(), 12);
	SameLine(80);

	PushID(field->GetEditorName());
	if (InputText("", buffer.Data(), 32,
	              ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		prop = buffer.CStr();
		result = true;
	}

	PopID();

	PopDefaultInputStyle();
	return result;
}

bool InputMaterial(Material& prop, const Field* field)
{
	// TODO: Implement this
	if (prop.Albedo)
	{
		Image(prop.Albedo->GetColorAttachment(),
		      Vector2f(64, 64),
		      Vector2f(1, 0),
		      Vector2f(0, 1));
	}

	return false;
}

bool Input(Color& prop, const Field* field)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaPreview;

    TextAligned(field->GetEditorName(), 12);
    SameLine(80);
    if (ImGui::ColorEdit4("", &prop[0], flags))
    {
        return true;
    }
    return false;
}



bool IconButton(const char* icon, const ImVec2& size, const char* tooltip)
{
    PushStyleColor(ImGuiCol_Button, Color::LIGHT_GREY);
    PushStyleColor(ImGuiCol_Border, Color::VERY_DARK_GREY);
    PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
    PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    bool res = ImGui::Button(icon, size);
    if (IsItemHovered())
	{
		SetTooltip("%s", tooltip);
	}
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
    SameLine(offsetFromStart);
    AlignTextToFramePadding();
    ImGui::TextUnformatted(text, nullptr);
}

void TextCentered(const char* text)
{
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) * 0.5f);
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
