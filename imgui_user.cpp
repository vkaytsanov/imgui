#include <Core/Core.h>

#include "imgui.h"
#include "Types/ResourceTypes.h"
#include "Types/String.h"
#include "Math/Vector4.h"
#include "Reflection/Field.h"
#include "Reflection/Function.h"
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

template <typename T>
bool DisplayVectorGroupInternal(T* value, uint32 count)
{
    count = Math::Min(count, 3u);
    bool result = false;

    float charWidth = ImGui::CalcTextSize("X ").x * (1 + 1 / count);
    float inputWidth = (ImGui::GetContentRegionAvail().x - count * charWidth) / count;

    ImGui::BeginGroup();
    const char* axes[] = { "X", "Y", "Z" };
    for (uint32 i = 0; i < count; i++)
    {
        ImGui::PushID(axes[i]);

        ImGui::PushItemWidth(charWidth);
        ImGuiEx::TextAligned(axes[i], 0);
        ImGui::PopItemWidth();

        ImGui::SameLine(0, 2);

        ImGui::PushItemWidth(inputWidth);
        result |= ImGuiEx::DisplayValue(&value[i]);
        ImGui::PopItemWidth();

        ImGui::PopID();
    }
    ImGui::EndGroup();

    return result;
}

bool DisplayValue(bool& value)
{
    PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    bool result = ImGui::Checkbox("", &value);
    PopStyleVar();
    return result;
}

bool DisplayValue(int8& value)   { return ImGui::InputScalar("", ImGuiDataType_S8,  &value, nullptr, nullptr, "%d", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal); }
bool DisplayValue(uint8& value)  { return ImGui::InputScalar("", ImGuiDataType_S8,  &value, nullptr, nullptr, "%d", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal); }
bool DisplayValue(int16& value)  { return ImGui::InputScalar("", ImGuiDataType_S16, &value, nullptr, nullptr, "%d", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal); }
bool DisplayValue(uint16& value) { return ImGui::InputScalar("", ImGuiDataType_S16, &value, nullptr, nullptr, "%d", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal); }
bool DisplayValue(int32& value)  { return ImGui::InputScalar("", ImGuiDataType_S32, &value, nullptr, nullptr, "%d", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal); }
bool DisplayValue(uint32& value) { return ImGui::InputScalar("", ImGuiDataType_S32, &value, nullptr, nullptr, "%d", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal); }
bool DisplayValue(int64& value)  { return ImGui::InputScalar("", ImGuiDataType_S64, &value, nullptr, nullptr, "%d", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal); }
bool DisplayValue(uint64& value) { return ImGui::InputScalar("", ImGuiDataType_S64, &value, nullptr, nullptr, "%d", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal); }
bool DisplayValue(float& value)  { return ImGui::InputScalar("", ImGuiDataType_Float, &value, nullptr, nullptr, "%.3f", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsScientific); }
bool DisplayValue(double& value) { return ImGui::InputScalar("", ImGuiDataType_Double, &value, nullptr, nullptr, "%.3f", ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsScientific); }


bool DisplayValue(Entity& value)
{
    PushReadOnly();

    EntityData* data = value.GetComponent<EntityData>();
    DisplayValue(data->Name);

    PopReadOnly();
    return false;
}

void InputSubClassEnd()
{
    ImGui::TreePop();
}

bool BeginArrayColumn(uint32& size, const Field* field)
{
    return false;
}

void DisplayArrayColumnItem(uint32 index)
{
    String number = String::Printf("Element %d", index);
    Label(*number);
}

bool DisplayValue(String& prop)
{
	bool result = false;
    // Copy here to avoid immediate renaming of the field
	String buffer = prop;
	if (InputText("", buffer.Data(), 32,
	              ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		prop = buffer.CStr();
		result = true;
	}
	return result;
}

bool DisplayValue(HString& prop)
{
    bool result = false;
    // Copy here to avoid immediate renaming of the field
    char buffer[32] = { 0 };
    if (prop)
    {
        Memory::Memcpy(buffer, *prop, 32);
    }

    if (InputText("", buffer, 32,
        ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
    {
        prop = HString(buffer);
        result = true;
    }
    return result;
}

bool DisplayValue(Resource*& prop)
{
    bool result = false;

    ImGui::BeginGroup();
    if (ImGui::BeginCombo("", *prop->GetHName()))
    {
        for (MultiObjectIterator<Resource> iter(prop->GetClass()); iter; ++iter)
        {
            bool isSelected = *iter == prop;
            if (ImGui::Selectable(*iter->GetHName(), isSelected))
            {
                prop = *iter;
                result = true;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (prop->GetThumbnail())
    {
        ImGui::Image(prop->GetThumbnail(), ImVec2(64, 64));
    }
    ImGui::EndGroup();

    return result;
}

bool DisplayValue(const Guid& value)
{
    String guidString = value.ToString(GuidFormat::DigitsAndHyphensInBrackets);
    ImGui::InputText("", guidString.Data(), guidString.GetSize());
    // Never should be editted
    return false;
}

bool DisplayValue(Color& prop)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaPreview;
    return ImGui::ColorEdit4("", &prop[0], flags);
}

bool DisplayEnum(Enum* enumClass, uint64& currentValue)
{
    bool result = false;
    if (ImGui::BeginCombo("", enumClass->GetCStrFromValue(currentValue)))
    {
        for (EnumIterator iter(enumClass); iter; ++iter)
        {
            bool isSelected = iter->Value == currentValue;
            if (ImGui::Selectable(iter->Name, isSelected))
            {
                currentValue = iter->Value;
                result = true;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        EndCombo();
    }
    return result;
}

bool DisplayValue(Object* object)
{
    Class* propClass = object->GetClass();
    if (propClass->HasBaseClass())
    {
        object->OnGUI();
    }

    return false;
}

bool DisplayDummyValue(const char* className)
{
    TextUnformatted(className);
    return false;
}

bool DisplayVectorGroup(int32* value, uint32 count)
{
    return DisplayVectorGroupInternal(value, count);
}

bool DisplayVectorGroup(float* value, uint32 count)
{
    return DisplayVectorGroupInternal(value, count);
}

bool BeginFieldNameColumn(const Field* field)
{
    bool isOpened = true;
    if (field->IsObject())
    {
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
        nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
        nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        isOpened = ImGui::TreeNodeEx(field->GetEditorName(), nodeFlags);
    }
    else
    {
        Label(field->GetEditorName());

        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
    }
    Tooltip(field->GetDescription() ? field->GetDescription() : field->GetEditorName());

    ImGui::PushID(field->GetEditorName());

    if (isOpened)
    {
        if (field->HasFlags(FF_ReadOnly))
        {
            PushReadOnly();
        }
        else
        {
            PushDefaultInputStyle();
        }
    }
    return isOpened;
}

void EndFieldNameColumn(const Field* field)
{
    if (field->HasFlags(FF_ReadOnly))
    {
        PopReadOnly();
    }
    else
    {
        PopDefaultInputStyle();
    }

    if (field->IsObject())
    {
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void Label(const char* text)
{
    ImGuiWindow* window = GetCurrentWindow();
    float fullWidth = GetContentRegionAvail().x;
    float itemWidth = fullWidth * 0.7f;
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImRect textRect;
    textRect.Min = ImGui::GetCursorScreenPos();
    textRect.Max = textRect.Min;
    textRect.Max.x += fullWidth - itemWidth;
    textRect.Max.y += textSize.y;

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
    SameLine(offsetFromStart);
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
