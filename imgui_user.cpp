#include <Core/Core.h>

#include "Types/ResourceTypes.h"
#include "Types/String.h"

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

int MyResizeCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		String* my_str = (String*)data->UserData;
		IM_ASSERT(my_str->Data() == data->Buf);
		my_str->Resize(data->BufTextLen ? data->BufTextLen + 1 : 1); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
		data->Buf = my_str->Data();
	}
	return 0;
}

bool InputResizableString(const char* label, String* my_str, ImGuiInputTextFlags flags)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	return ImGui::InputText(label,
	                        my_str->Data(),
	                        my_str->GetLength() + 1,
	                        flags | ImGuiInputTextFlags_CallbackResize, MyResizeCallback,
	                        my_str);
}

bool InputVector4f(class Object* instance, const Field* prop)
{
	bool result = false;
	auto& vec = prop->GetValue<Vector4f>(instance);
	auto inputFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal;

	SameLine(12);
	AlignTextToFramePadding();
	Text("%s", prop->GetEditorName());

	PushStyleColor(ImGuiCol_FrameBg, Color::FromHex(0x2a2a2aFF));
	PushStyleColor(ImGuiCol_Border, Color::FromHex(0x202020FF));
	PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
	PushItemWidth(55);

	PushID(prop->GetEditorName() + 1);
	SameLine(75);
	AlignTextToFramePadding();
	Text("X");
	SameLine(0, 5);
	if (InputFloat("", &vec.x, 0, 0, "%.3f", inputFlags))
	{
		result = true;
	}
	PopID();
	PushID(prop->GetEditorName() + 2);
	SameLine(150);
	AlignTextToFramePadding();
	Text("Y");
	SameLine(0, 5);
	if (InputFloat("", &vec.y, 0, 0, "%.3f", inputFlags))
	{
		result = true;
	}
	PopID();
	PushID(prop->GetEditorName() + 3);
	SameLine(225);
	AlignTextToFramePadding();
	Text("Z");
	SameLine(0, 5);
	if (InputFloat("", &vec.z, 0, 0, "%.3f", inputFlags))
	{
		result = true;
	}
	PopID();

	PopStyleColor(2);
	PopStyleVar();
	PopItemWidth();


	NewLine();
	return result;
}

bool InputString(Object* instance, const Field* prop)
{
	bool result = false;
	PushDefaultInputStyle();

	String buffer = prop->GetValue<String>(instance).CStr();
	SameLine(12);
	AlignTextToFramePadding();
	Text("%s", prop->GetEditorName());
	SameLine(80);

	PushID(prop->GetEditorName());
	if (InputText("", buffer.Data(), 32,
	              ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		auto& str = prop->GetValue<String>(instance);
		str = buffer.CStr();
		result = true;
	}

	PopID();

	PopDefaultInputStyle();
	return result;
}

bool InputMaterial(Object* instance, const Field* prop)
{
	// TODO: Implement this
	auto material = prop->GetValue<Material*>(instance);
	if (material != nullptr && material->Albedo)
	{
		Image(material->Albedo->GetColorAttachment(),
		      Vector2f(64, 64),
		      Vector2f(1, 0),
		      Vector2f(0, 1));
	}

	return false;
}

bool InputMeshData(Object* instance, const Field* prop)
{
	// TODO:
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


bool InputColor(Object* instance, const Field* prop)
{
	Color& color = prop->GetValue<Color>(instance);
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaPreview;

	float colors[4] = { color.r, color.g, color.b, color.a };
	SameLine(12);
	AlignTextToFramePadding();
	Text("%s", prop->GetEditorName());
	SameLine(80);
	if (ImGui::ColorEdit4("", colors, flags))
	{
		color.r = colors[0];
		color.g = colors[1];
		color.b = colors[2];
		color.a = colors[3];
		return true;
	}
	return false;
}


}
