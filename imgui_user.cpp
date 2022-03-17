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
	PushStyleColor(ImGuiCol_FrameBg, Color::FromHex(0x2a2a2aFF));
	PushStyleColor(ImGuiCol_Border, Color::FromHex(0x202020FF));
	PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
	PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
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
    SameLine(25);
    AlignTextToFramePadding();
    Text("%d", index);
    SameLine(25);
}


bool Input(Vector4f& prop, const Field* field)
{
	bool result = false;
	auto inputFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal;

    PushDefaultInputStyle();
	PushItemWidth(55);

    SameLine(12);
    AlignTextToFramePadding();
    Text("%s", field->GetEditorName());

    const char* axes[] = { "X", "Y", "Z" };
    for (uint32 i = 0; i < 3; i++)
    {
        PushID(field->GetEditorName() + i + 1);
        SameLine(75.f * (static_cast<float>(i) + 1.f));
        AlignTextToFramePadding();
        TextUnformatted(axes[i]);
        SameLine(0, 5);

        result |= InputFloat("", &prop[i], 0, 0, "%.3f", inputFlags);

        PopID();
    }

    PopDefaultInputStyle();
	PopItemWidth();


	NewLine();
	return result;
}

bool Input(String& prop, const Field* field)
{
	bool result = false;
	PushDefaultInputStyle();

    // Copy here to avoid immediate renaming of the field
	String buffer = prop;
	SameLine(12);
	AlignTextToFramePadding();
	Text("%s", field->GetEditorName());
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

    Color copy = prop;
    SameLine(12);
    AlignTextToFramePadding();
    Text("%s", field->GetEditorName());
    SameLine(80);
    if (ImGui::ColorEdit4("", &copy[0], flags))
    {
        prop.R = copy.R;
        prop.G = copy.G;
        prop.B = copy.B;
        prop.A = copy.A;
        return true;
    }
    return false;
}



bool IconButton(const char* icon, const char* tooltip)
{
	PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	PushStyleColor(ImGuiCol_Button, GetStyle().Colors[ImGuiCol_WindowBg]);
	bool res = SmallButton(icon);
	if (IsItemHovered())
	{
		SetTooltip("%s", tooltip);
	}
	PopStyleColor();
	PopStyleVar();
	return res;
}

bool ButtonDisabled(const char* label, const ImVec2& size, bool condition)
{
    BeginDisabled(condition);
    const bool result = ImGui::Button(label, size);
    EndDisabled();
    return result;
}

}
