#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_addons.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <map>
#include <string>
#include <cmath>

using namespace ImGui;

static float AddonScale(float value)
{
    return value * ImGui::GetIO().FontGlobalScale;
}

static ImVec2 AddonScaleVec(float x, float y)
{
    const float scale = ImGui::GetIO().FontGlobalScale;
    return ImVec2(x * scale, y * scale);
}

ImVec4 ImAdd::HexToColorVec4(unsigned int hex_color, float alpha)
{
    ImVec4 color;

    color.x = ((hex_color >> 16) & 0xFF) / 255.0f;
    color.y = ((hex_color >> 8) & 0xFF) / 255.0f;
    color.z = (hex_color & 0xFF) / 255.0f;
    color.w = alpha;

    return color;
}

void ImAdd::DoubleText(ImVec4 color1, ImVec4 color2, const char* label1, const char* label2)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(std::string(label1 + std::string(label2)).c_str());
    const ImVec2 label1_size = CalcTextSize(label1, NULL, true);
    const ImVec2 label2_size = CalcTextSize(label2, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(-0.1f, g.FontSize), label1_size.x + label2_size.x, g.FontSize);

    const ImRect total_bb(pos, pos + size);
    ItemSize(total_bb);
    if (!ItemAdd(total_bb, id)) {
        return;
    }

    window->DrawList->AddText(pos, GetColorU32(color1), label1);
    window->DrawList->AddText(pos + ImVec2(size.x - ImGui::CalcTextSize(label2).x, 0), GetColorU32(color2), label2);
}

void ImAdd::SeparatorText(const char* label, float thickness)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(-0.1f, g.FontSize), label_size.x, g.FontSize);

    const ImRect total_bb(pos, pos + size);
    ItemSize(total_bb);
    if (!ItemAdd(total_bb, id)) {
        return;
    }

    window->DrawList->AddText(pos, GetColorU32(ImGuiCol_TextDisabled), label);

    if (thickness > 0)
    {
        ImVec2 line_start = pos + ImVec2(label_size.x + style.ItemInnerSpacing.x, size.y / 2);
        ImVec2 line_end = pos + ImVec2(size.x, size.y / 2);

        ImVec4 glow_col = GetStyleColorVec4(ImGuiCol_SliderGrab);
        ImVec4 core_col = GetStyleColorVec4(ImGuiCol_SliderGrab);

        glow_col.w = 0.10f;
        window->DrawList->AddLine(line_start, line_end, GetColorU32(glow_col), thickness + AddonScale(6.0f));

        glow_col.w = 0.22f;
        window->DrawList->AddLine(line_start, line_end, GetColorU32(glow_col), thickness + AddonScale(3.0f));

        core_col.w = 0.90f;
        window->DrawList->AddLine(line_start, line_end, GetColorU32(core_col), thickness);
    }
}

bool ImAdd::RadioFrame(const char* label, int* v, int radio_id, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held, active;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    active = *v == radio_id;
    if (pressed) {
        *v = radio_id;
    }

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(active ? ImGuiCol_ButtonActive : ImGuiCol_WindowBg);
    ImVec4 colLabel = GetStyleColorVec4(active ? ImGuiCol_SliderGrab : (hovered && !held) ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Label;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Label = colLabel;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);
    it_anim->second.Label = ImLerp(it_anim->second.Label, colLabel, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilledMultiColor(pos, pos + size, ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetColorU32(it_anim->second.Frame), ImGui::GetColorU32(it_anim->second.Frame));
    window->DrawList->AddRect(pos, pos + size, ImGui::GetColorU32(ImGuiCol_Border));
    window->DrawList->AddText(pos + ImVec2(size.x / 2 - label_size.x / 2, size.y / 2 - label_size.y / 2 + 1), GetColorU32(it_anim->second.Label), label);

    return pressed;
}

bool ImAdd::RadioFrameGradient(const char* label, int* v, int radio_id, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    bool active = *v == radio_id;
    if (pressed)
        *v = radio_id;

    ImVec4 colFrame = active ? GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);
    if (hovered && !active)
        colFrame = ImVec4(1.0f, 1.0f, 1.0f, 0.045f);

    ImVec4 colLabel = GetStyleColorVec4((active || hovered) ? ImGuiCol_Text : ImGuiCol_TextDisabled);
    ImVec4 colLine = GetStyleColorVec4(ImGuiCol_SliderGrab);
    colLine.w = active ? 1.0f : (hovered ? 0.45f : 0.0f);

    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Label;
        ImVec4 Line;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);
        it_anim->second.Frame = colFrame;
        it_anim->second.Label = colLabel;
        it_anim->second.Line = colLine;
    }

    const float speed = 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime;
    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, speed);
    it_anim->second.Label = ImLerp(it_anim->second.Label, colLabel, speed);
    it_anim->second.Line = ImLerp(it_anim->second.Line, colLine, speed);

    RenderNavHighlight(bb, id);

    if (it_anim->second.Frame.w > 0.001f)
        window->DrawList->AddRectFilled(pos, pos + size, GetColorU32(it_anim->second.Frame), style.FrameRounding);

    if (it_anim->second.Line.w > 0.001f)
    {
        const float barWidth = AddonScale(3.0f);
        window->DrawList->AddRectFilled(pos + AddonScaleVec(0.0f, 7.0f), pos + ImVec2(barWidth, size.y - AddonScale(7.0f)), GetColorU32(it_anim->second.Line), AddonScale(3.0f));
        window->DrawList->AddRectFilled(pos + ImVec2(barWidth, 0.0f), pos + ImVec2(size.x * 0.70f, size.y), GetColorU32(ImVec4(it_anim->second.Line.x, it_anim->second.Line.y, it_anim->second.Line.z, active ? 0.08f : 0.03f)), style.FrameRounding);
    }

    window->DrawList->AddText(pos + ImVec2(AddonScale(13.0f), size.y / 2 - label_size.y / 2 + AddonScale(1.0f)), GetColorU32(it_anim->second.Label), label);

    return pressed;
}

bool ImAdd::RadioFrameText(const char* label, int* v, int radio_id, bool underline, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

    const ImRect bb(pos, pos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held, active;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    active = *v == radio_id;
    if (pressed) {
        *v = radio_id;
    }

    // Colors
    ImVec4 colLabel = GetStyleColorVec4(active ? ImGuiCol_Text : (hovered && !held) ? ImGuiCol_SliderGrab : ImGuiCol_TextDisabled);

    ImVec4 colUnderlineMain = GetStyleColorVec4(ImGuiCol_SliderGrab);
    ImVec4 colUnderlineNull = colUnderlineMain; colUnderlineNull.w = 0.0f;
    ImVec4 colUnderline = (active ? colUnderlineMain : colUnderlineNull);

    // Animations
    struct stColors_State {
        ImVec4 Label;
        ImVec4 Underline;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Label = colLabel;
        it_anim->second.Underline = colUnderline;
    }

    it_anim->second.Label = ImLerp(it_anim->second.Label, colLabel, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);
    it_anim->second.Underline = ImLerp(it_anim->second.Underline, colUnderline, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddText(pos + ImVec2(size.x / 2 - label_size.x / 2, size.y / 2 - label_size.y / 2 + 1), GetColorU32(it_anim->second.Label), label);
    if (underline)
        window->DrawList->AddLine(pos + ImVec2(-1, label_size.y + style.FrameBorderSize * 2), pos + ImVec2(label_size.x, label_size.y + style.FrameBorderSize * 2), ImGui::GetColorU32(it_anim->second.Underline), style.FrameBorderSize);

    return pressed;
}

bool ImAdd::SelectableFrame(const char* str_id, int selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, g.FontSize, g.FontSize);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    // Colors
    ImVec4 colFrameTop = GetStyleColorVec4(selected ? ImGuiCol_SliderGrab : ImGuiCol_Button);
    ImVec4 colFrameBottom = GetStyleColorVec4(selected ? ImGuiCol_SliderGrabActive : ImGuiCol_ButtonActive);

    // Animations
    struct stColors_State {
        ImVec4 FrameTop;
        ImVec4 FrameBottom;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.FrameTop = colFrameTop;
        it_anim->second.FrameBottom = colFrameBottom;
    }

    it_anim->second.FrameTop = ImLerp(it_anim->second.FrameTop, colFrameTop, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);
    it_anim->second.FrameBottom = ImLerp(it_anim->second.FrameBottom, colFrameBottom, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilledMultiColor(pos, pos + size, ImGui::GetColorU32(it_anim->second.FrameTop), ImGui::GetColorU32(it_anim->second.FrameTop), ImGui::GetColorU32(it_anim->second.FrameBottom), ImGui::GetColorU32(it_anim->second.FrameBottom));
    if (style.FrameBorderSize > 0)
    {
        window->DrawList->AddRect(pos, pos + size, ImGui::GetColorU32(ImGuiCol_BorderShadow));
        window->DrawList->AddRect(pos + ImVec2(style.FrameBorderSize, style.FrameBorderSize), pos + size - ImVec2(style.FrameBorderSize, style.FrameBorderSize), ImGui::GetColorU32(ImGuiCol_Border));
    }

    return pressed;
}

void ImAdd::BeginChild(const char* label, const ImVec2& size_arg)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImGui::BeginChild(label, size_arg, ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground);

    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();

    const float rounding = style.ChildRounding;
    const float header_h = label_size.y + style.WindowPadding.y * 1.05f;

    ImVec4 card_col = GetStyleColorVec4(ImGuiCol_ChildBg);
    ImVec4 accent_col = GetStyleColorVec4(ImGuiCol_SliderGrab);
    ImVec4 top_glow = accent_col; top_glow.w = 0.08f;
    ImVec4 bottom_glow = accent_col; bottom_glow.w = 0.00f;

    pDrawList->AddRectFilled(pos, pos + size, GetColorU32(card_col), rounding);
    pDrawList->AddRectFilledMultiColor(pos, pos + ImVec2(size.x, header_h + AddonScale(4.0f)), GetColorU32(top_glow), GetColorU32(bottom_glow), GetColorU32(bottom_glow), GetColorU32(top_glow));

    ImVec2 label_pos = pos + ImVec2(style.WindowPadding.x, style.WindowPadding.y * 0.52f);
    pDrawList->AddText(label_pos, GetColorU32(ImGuiCol_Text), label);

    ImVec2 content_min = pos + ImVec2(style.WindowPadding.x, header_h + AddonScale(22.0f));
    ImGui::SetCursorScreenPos(content_min);
    ImGui::BeginChild(std::string(std::string(label) + "##Main").c_str(), ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground);
}

void ImAdd::EndChild()
{
    ImGui::EndChild();
    ImGui::EndChild();
}

bool ImAdd::Button(const char* label, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    // Colors
    ImVec4 colFrame = GetStyleColorVec4((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);
    
    window->DrawList->AddRectFilled(pos, pos + size, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    window->DrawList->AddRectFilledMultiColor(pos, pos + size, GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha));
    if (style.FrameBorderSize > 0)
    {
        window->DrawList->AddRect(pos, pos + size, GetColorU32(ImGuiCol_BorderShadow), style.FrameRounding, 0, style.FrameBorderSize);
        window->DrawList->AddRect(pos + ImVec2(style.FrameBorderSize, style.FrameBorderSize), pos + size - ImVec2(style.FrameBorderSize, style.FrameBorderSize), GetColorU32(ImGuiCol_Border), style.FrameRounding, 0, style.FrameBorderSize);
    }

    RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    return pressed;
}

bool ImAdd::CheckBox(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float switch_height = g.FontSize * 1.05f;
    const float switch_width = switch_height * 1.90f;
    const float switch_radius = switch_height * 0.5f;
    const float knob_radius = switch_radius - AddonScale(3.0f);
    const float inner_padding = AddonScale(3.0f);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(switch_width + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), ImMax(switch_height, label_size.y));

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered = false, held = false;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed)
        *v = !*v;

    const bool active = *v;

    const ImVec4 col_track_off = hovered ? GetStyleColorVec4(ImGuiCol_FrameBgHovered) : GetStyleColorVec4(ImGuiCol_FrameBg);
    const ImVec4 col_track_on = hovered ? ImLerp(GetStyleColorVec4(ImGuiCol_SliderGrab), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), 0.15f) : GetStyleColorVec4(ImGuiCol_SliderGrab);
    const ImVec4 col_knob = ImVec4(0.96f, 0.96f, 0.98f, 1.0f);
    const ImVec4 col_label = GetStyleColorVec4(active ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    struct stCheckboxState
    {
        ImVec4 Track;
        ImVec4 Label;
        float  ToggleT;
        float  HoverT;
    };

    static std::map<ImGuiID, stCheckboxState> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end())
    {
        anim.insert({ id, stCheckboxState() });
        it_anim = anim.find(id);
        it_anim->second.Track = active ? col_track_on : col_track_off;
        it_anim->second.Label = col_label;
        it_anim->second.ToggleT = active ? 1.0f : 0.0f;
        it_anim->second.HoverT = hovered ? 1.0f : 0.0f;
    }

    const float anim_speed = 1.0f / IMADD_ANIMATIONS_SPEED * GetIO().DeltaTime;
    it_anim->second.Track = ImLerp(it_anim->second.Track, active ? col_track_on : col_track_off, anim_speed);
    it_anim->second.Label = ImLerp(it_anim->second.Label, col_label, anim_speed);
    it_anim->second.ToggleT = ImLerp(it_anim->second.ToggleT, active ? 1.0f : 0.0f, anim_speed * 1.35f);
    it_anim->second.HoverT = ImLerp(it_anim->second.HoverT, hovered ? 1.0f : 0.0f, anim_speed * 1.25f);

    RenderNavHighlight(bb, id);

    const ImVec2 switch_pos = pos + ImVec2(0.0f, (size.y - switch_height) * 0.5f);
    const ImVec2 switch_min = switch_pos;
    const ImVec2 switch_max = switch_pos + ImVec2(switch_width, switch_height);
    const float knob_center_x = switch_min.x + switch_radius + inner_padding + (switch_width - (switch_radius + inner_padding) * 2.0f) * ImSaturate(it_anim->second.ToggleT);
    const float knob_center_y = switch_min.y + switch_radius;

    // subtle purple glow when enabled
    if (it_anim->second.ToggleT > 0.01f)
    {
        ImVec4 glow_col = GetStyleColorVec4(ImGuiCol_SliderGrab);
        glow_col.w = 0.16f * ImSaturate(it_anim->second.ToggleT);
        window->DrawList->AddRectFilled(switch_min - AddonScaleVec(1.5f, 1.5f), switch_max + AddonScaleVec(1.5f, 1.5f), GetColorU32(glow_col), switch_radius + AddonScale(2.0f));
    }

    window->DrawList->AddRectFilled(switch_min, switch_max, GetColorU32(it_anim->second.Track), switch_radius);
    window->DrawList->AddRect(switch_min, switch_max, GetColorU32(ImGuiCol_BorderShadow, 0.90f), switch_radius, 0, AddonScale(1.0f));
    window->DrawList->AddRect(switch_min + AddonScaleVec(1.0f, 1.0f), switch_max - AddonScaleVec(1.0f, 1.0f), GetColorU32(ImGuiCol_Border, 0.65f), switch_radius - AddonScale(1.0f), 0, AddonScale(1.0f));

    const ImVec2 knob_center(knob_center_x, knob_center_y);
    window->DrawList->AddCircleFilled(knob_center, knob_radius + AddonScale(1.0f), GetColorU32(ImGuiCol_BorderShadow, 0.25f), 24);
    window->DrawList->AddCircleFilled(knob_center, knob_radius, GetColorU32(col_knob), 24);
    window->DrawList->AddCircle(knob_center, knob_radius, GetColorU32(ImGuiCol_Border, 0.65f), 24, AddonScale(1.0f));

    if (label_size.x > 0.0f)
    {
        ImVec2 label_pos = pos + ImVec2(switch_width + style.ItemInnerSpacing.x, (size.y - label_size.y) * 0.5f);

        if (it_anim->second.HoverT > 0.01f)
        {
            ImVec4 text_glow = GetStyleColorVec4(ImGuiCol_SliderGrab);
            text_glow.w = 0.24f * ImSaturate(it_anim->second.HoverT);
            window->DrawList->AddText(label_pos + AddonScaleVec(-1.0f, 0.0f), GetColorU32(text_glow), label);
            window->DrawList->AddText(label_pos + AddonScaleVec( 1.0f, 0.0f), GetColorU32(text_glow), label);
            window->DrawList->AddText(label_pos + AddonScaleVec(0.0f, -1.0f), GetColorU32(text_glow), label);
            window->DrawList->AddText(label_pos + AddonScaleVec(0.0f,  1.0f), GetColorU32(text_glow), label);
        }

        window->DrawList->AddText(label_pos, GetColorU32(it_anim->second.Label), label);
    }

    return pressed;
}

bool ImAdd::ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(desc_id);
    const float default_size = GetFrameHeight();
    const ImVec2 size(size_arg.x == 0.0f ? default_size : size_arg.x, size_arg.y == 0.0f ? default_size : size_arg.y);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    if (flags & ImGuiColorEditFlags_NoAlpha)
        flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = col;
    if (flags & ImGuiColorEditFlags_InputHSV)
        ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    ImVec4 col_rgb_without_alpha(col_rgb.x, col_rgb.y, col_rgb.z, 1.0f);
    float grid_step = ImMin(size.x, size.y) / 2.99f;
    float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
    ImRect bb_inner = bb;
    float off = 0.0f;
    if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        off = -AddonScale(0.75f); // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
        bb_inner.Expand(off);
    }
    if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col_rgb.w < 1.0f)
    {
        float mid_x = IM_ROUND((bb_inner.Min.x + bb_inner.Max.x) * 0.5f);
        RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y), bb_inner.Max, GetColorU32(col_rgb), grid_step, ImVec2(-grid_step + off, off), rounding, ImDrawFlags_RoundCornersRight);
        window->DrawList->AddRectFilled(bb_inner.Min, ImVec2(mid_x, bb_inner.Max.y), GetColorU32(col_rgb_without_alpha), rounding, ImDrawFlags_RoundCornersLeft);
    }
    else
    {
        // Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
        ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col_rgb : col_rgb_without_alpha;
        if (col_source.w < 1.0f)
            RenderColorRectWithAlphaCheckerboard(window->DrawList, bb_inner.Min, bb_inner.Max, GetColorU32(col_source), grid_step, ImVec2(off, off), rounding);
        else
            window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), rounding);
    }
    RenderNavHighlight(bb, id);
    if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        if (g.Style.FrameBorderSize > 0.0f)
        {
            PushStyleColor(ImGuiCol_BorderShadow, ImVec4());
            RenderFrameBorder(bb.Min, bb.Max, rounding);
            PopStyleColor();
            window->DrawList->AddRectFilledMultiColor(bb.Min, bb.Max, GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha));
        }
        else
        {
            window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), rounding); // Color button are often in need of some sort of border
            window->DrawList->AddRectFilledMultiColor(bb.Min, bb.Max, GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha));
        }
    }

    // Drag and Drop Source
    // NB: The ActiveId test is merely an optional micro-optimization, BeginDragDropSource() does the same test.
    if (g.ActiveId == id && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropSource())
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
            SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col_rgb, sizeof(float) * 3, ImGuiCond_Once);
        else
            SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col_rgb, sizeof(float) * 4, ImGuiCond_Once);
        ColorButton(desc_id, col, flags);
        SameLine();
        TextEx("Color");
        EndDragDropSource();
    }

    // Tooltip
    if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered && IsItemHovered(ImGuiHoveredFlags_ForTooltip))
        ColorTooltip(desc_id, &col.x, flags & (ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

    return pressed;
}

bool ImAdd::ColorEdit4(const char* label, float col[4])
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec4 col_v4(col[0], col[1], col[2], col[3]);

    ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview;

    PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_BorderShadow]);
    PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
    bool result = ImAdd::ColorButton(label, col_v4, flags, ImVec2(g.FontSize * 2, g.FontSize));
    PopStyleVar();
    PopStyleColor();
    if (result)
    {
        OpenPopup(std::string(std::string(label) + "##ColorEdit4").c_str());
    }
    if (BeginPopup(std::string(std::string(label) + "##ColorEdit4").c_str()))
    {
        PushStyleColor(ImGuiCol_BorderShadow, ImVec4());
        ColorPicker4(label, col, flags);
        PopStyleColor();
        EndPopup();
    }

    return result;
}

bool ImAdd::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec2 value_size = CalcTextSize(value_buf, value_buf_end, true);

    // The slider bar itself is intentionally compact. The formatted value/unit is
    // rendered beside the bar so units are always visible next to the control.
    const float available_w = ImMax(1.0f, GetContentRegionAvail().x);
    float w = ImMin(available_w * 0.42f, g.FontSize * 14.0f);
    w = ImMax(w, g.FontSize * 8.0f);
    const float value_gap = style.ItemInnerSpacing.x * 1.25f;
    const float value_block_w = value_gap + value_size.x;

    ImVec2 pos = window->DC.CursorPos;

    const float label_height = label_size.x > 0 ? g.FontSize + style.ItemInnerSpacing.y : 0.0f;
    const ImRect frame_bb(pos + ImVec2(0, label_height), pos + ImVec2(w, g.FontSize + label_height));
    const ImRect total_bb(pos, pos + ImVec2(w + value_block_w, g.FontSize + label_height));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;

    const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
    const bool clicked = hovered && IsMouseClicked(0, ImGuiInputFlags_None, id);
    const bool make_active = (clicked || g.NavActivateId == id);

    if (make_active)
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
    }

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImVec4 colGrab = GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Grab;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Grab = colGrab;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Grab = ImLerp(it_anim->second.Grab, colGrab, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, 0, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    ImVec2 grab_padding = ImVec2(grab_bb.Min.y - frame_bb.Min.y, 0); grab_padding.y = grab_padding.x;

    // Render
    RenderNavHighlight(frame_bb, id);
    PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_BorderShadow]);
    PushStyleColor(ImGuiCol_BorderShadow, ImVec4());
    RenderFrame(frame_bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize), frame_bb.Max - ImVec2(style.FrameBorderSize, style.FrameBorderSize), GetColorU32(it_anim->second.Frame), true, g.Style.FrameRounding);
    PopStyleColor(2);

    // Render grab
    if (grab_bb.Max.x > grab_bb.Min.x)
    {
        window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize) * 2, grab_bb.Max + grab_padding - ImVec2(style.FrameBorderSize, style.FrameBorderSize) * 2, GetColorU32(it_anim->second.Grab), style.FrameRounding);
    }

    window->DrawList->AddRectFilledMultiColor(frame_bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize) * 2, frame_bb.Max - ImVec2(style.FrameBorderSize, style.FrameBorderSize) * 2, GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha));

    // Display the formatted value/unit visibly beside the slider bar.
    const ImVec2 value_pos = ImVec2(frame_bb.Max.x + value_gap, frame_bb.Min.y + (frame_bb.GetHeight() - value_size.y) * 0.5f);
    window->DrawList->AddText(value_pos, GetColorU32(ImGuiCol_TextDisabled), value_buf, value_buf_end);

    if (label_size.x > 0.0f)
        RenderText(pos, label);

    return value_changed;
}

bool ImAdd::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format)
{
    return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format);
}

bool ImAdd::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format)
{
    return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format);
}

bool ImAdd::Selectable(const char* label, bool selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    float borderSize = style.FrameBorderSize;

    // Colors
    ImVec4 colFrameMain = GetStyleColorVec4((hovered && !selected) ? held ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered : ImGuiCol_Header);
    ImVec4 colFrameNull = colFrameMain; colFrameNull.w = 0.0f;
    ImVec4 colFrame = ((!hovered && !selected) ? colFrameNull : colFrameMain);

    ImVec4 colBorderMain = GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 colBorderNull = colBorderMain; colBorderNull.w = 0.0f;
    ImVec4 colBorder = (selected ? colBorderMain : colBorderNull);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Border = colBorder;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);

    if (borderSize > 0)
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, borderSize);

    RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    return pressed;
}

bool ImAdd::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together
    if (flags & ImGuiComboFlags_WidthFitPreview)
        IM_ASSERT((flags & (ImGuiComboFlags_NoPreview | ImGuiComboFlags_CustomPreview)) == 0);

    const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float preview_width = ((flags & ImGuiComboFlags_WidthFitPreview) && (preview_value != NULL)) ? CalcTextSize(preview_value, NULL, true).x : 0.0f;
    const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : ((flags & ImGuiComboFlags_WidthFitPreview) ? (arrow_size + preview_width + style.FramePadding.x * 2.0f) : CalcItemWidth());

    const ImRect bb(window->DC.CursorPos + ImVec2(0.0f, label_size.x > 0 ? label_size.y + style.ItemInnerSpacing.y : 0.0f), window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f) + ImVec2(0.0f, label_size.x > 0 ? label_size.y + style.ItemInnerSpacing.y : 0.0f));
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f) + ImVec2(0.0f, label_size.x > 0 ? label_size.y + style.ItemInnerSpacing.y : 0.0f));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &bb))
        return false;

    // Open on click
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
    bool popup_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    if (pressed && !popup_open)
    {
        OpenPopupEx(popup_id, ImGuiPopupFlags_None);
        popup_open = true;
    }

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImVec4 colText = GetStyleColorVec4((popup_open || hovered) ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Text;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Text = colText;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Text = ImLerp(it_anim->second.Text, colText, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render shape
    const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
    RenderNavHighlight(bb, id);
    if (!(flags & ImGuiComboFlags_NoPreview)) {
        window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);
        window->DrawList->AddRectFilledMultiColor(bb.Min, bb.Max, GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, 0.0f), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha), GetColorU32(ImGuiCol_BorderShadow, style.DisabledAlpha));
    }
    if (!(flags & ImGuiComboFlags_NoArrowButton))
    {
        if (value_x2 + arrow_size - style.FramePadding.x <= bb.Max.x)
        {
            RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y) + ImVec2(style.FrameBorderSize, -style.FrameBorderSize), GetColorU32(ImGuiCol_BorderShadow), ImGuiDir_Down, 1.0f);
            RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y) + ImVec2(-style.FrameBorderSize, -style.FrameBorderSize), GetColorU32(ImGuiCol_BorderShadow), ImGuiDir_Down, 1.0f);
            RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y) + ImVec2(0, style.FrameBorderSize * 2), GetColorU32(ImGuiCol_BorderShadow), ImGuiDir_Down, 1.0f);
            RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y), GetColorU32(it_anim->second.Text), ImGuiDir_Down, 1.0f);
        }
    }

    PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_BorderShadow]);
    PushStyleColor(ImGuiCol_BorderShadow, ImVec4());
    RenderFrameBorder(bb.Min, bb.Max, style.FrameRounding);
    PopStyleColor(2);

    PushStyleColor(ImGuiCol_BorderShadow, ImVec4());
    RenderFrameBorder(bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize), bb.Max - ImVec2(style.FrameBorderSize, style.FrameBorderSize), style.FrameRounding);
    PopStyleColor();

    // Custom preview
    if (flags & ImGuiComboFlags_CustomPreview)
    {
        g.ComboPreviewData.PreviewRect = ImRect(bb.Min.x, bb.Min.y, value_x2, bb.Max.y);
        IM_ASSERT(preview_value == NULL || preview_value[0] == 0);
        preview_value = NULL;
    }

    // Render preview and label
    if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
    {
        if (g.LogEnabled)
            LogSetNextTextDecoration("{", "}");
        PushStyleColor(ImGuiCol_Text, GetStyleColorVec4(ImGuiCol_TextDisabled));
        RenderTextClipped(bb.Min + style.FramePadding, ImVec2(value_x2, bb.Max.y), preview_value, NULL, NULL);
        PopStyleColor();
    }
    if (label_size.x > 0)
        RenderText(total_bb.Min, label);

    if (!popup_open)
        return false;

    g.NextWindowData.Flags = backup_next_window_data_flags;
    return BeginComboPopup(popup_id, bb, flags);
}

static float CalcMaxPopupHeightFromItemCount(int items_count)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ImAdd::BeginComboPopup(ImGuiID popup_id, const ImRect& bb, ImGuiComboFlags flags)
{
    ImGuiContext& g = *GImGui;
    if (!IsPopupOpen(popup_id, ImGuiPopupFlags_None))
    {
        g.NextWindowData.ClearFlags();
        return false;
    }

    // Set popup size
    float w = bb.GetWidth();
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
    {
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
    }
    else
    {
        if ((flags & ImGuiComboFlags_HeightMask_) == 0)
            flags |= ImGuiComboFlags_HeightRegular;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_)); // Only one
        int popup_max_height_in_items = -1;
        if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
        ImVec2 constraint_min(0.0f, 0.0f), constraint_max(FLT_MAX, FLT_MAX);
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.x <= 0.0f) // Don't apply constraints if user specified a size
            constraint_min.x = w;
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.y <= 0.0f)
            constraint_max.y = CalcMaxPopupHeightFromItemCount(popup_max_height_in_items);
        SetNextWindowSizeConstraints(constraint_min, constraint_max);
    }

    // This is essentially a specialized version of BeginPopupEx()
    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    // Set position given a custom constraint (peak into expected window size so we can position it)
    // FIXME: This might be easier to express with an hypothetical SetNextWindowPosConstraints() function?
    // FIXME: This might be moved to Begin() or at least around the same spot where Tooltips and other Popups are calling FindBestWindowPosForPopupEx()?
    if (ImGuiWindow* popup_window = FindWindowByName(name))
        if (popup_window->WasActive)
        {
            // Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
            ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
            popup_window->AutoPosLastDirection = (flags & ImGuiComboFlags_PopupAlignLeft) ? ImGuiDir_Left : ImGuiDir_Down; // Left = "Below, Toward Left", Down = "Below, Toward Right (default)"
            ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
            ImVec2 pos = FindBestWindowPosForPopupEx(bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, bb, ImGuiPopupPositionPolicy_ComboBox);
            SetNextWindowPos(pos + ImVec2(0, g.Style.ItemSpacing.y));
        }

    // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
    PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding);
    PushStyleVar(ImGuiStyleVar_PopupRounding, g.Style.FrameRounding);
    bool ret = Begin(name, NULL, window_flags);
    PopStyleVar(2);
    if (!ret)
    {
        EndPopup();
        IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
        return false;
    }
    return true;
}

bool ImAdd::Combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items)
{
    ImGuiContext& g = *GImGui;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        preview_value = getter(user_data, *current_item);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

    if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        const char* item_text = getter(user_data, i);
        if (item_text == NULL)
            item_text = "*Unknown item*";

        PushID(i);
        const bool item_selected = (i == *current_item);
        if (Selectable(item_text, item_selected, ImVec2(-0.1f, 0)) && *current_item != i)
        {
            value_changed = true;
            *current_item = i;
            //CloseCurrentPopup();
        }
        if (item_selected)
            SetItemDefaultFocus();
        PopID();
    }

    EndCombo();

    if (value_changed)
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

// Getter for the old Combo() API: const char*[]
static const char* Items_ArrayGetter(void* data, int idx)
{
    const char* const* items = (const char* const*)data;
    return items[idx];
}

// Getter for the old Combo() API: "item1\0item2\0item3\0"
static const char* Items_SingleStringGetter(void* data, int idx)
{
    const char* items_separated_by_zeros = (const char*)data;
    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while (*p)
    {
        if (idx == items_count)
            break;
        p += strlen(p) + 1;
        items_count++;
    }
    return *p ? p : NULL;
}

bool ImAdd::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
    const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
    return value_changed;
}

bool ImAdd::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, float width, int height_in_items)
{
    int items_count = 0;
    const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
    while (*p)
    {
        p += strlen(p) + 1;
        items_count++;
    }

    const float w = CalcItemSize(ImVec2(width, 0), CalcItemWidth(), 0).x;
    PushItemWidth(w);
    bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
    PopItemWidth();

    return value_changed;
}

bool ImAdd::KeyBind(const char* str_id, int* k, float custom_width)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);

    char buf_display[32] = "None";
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 buf_display_size = ImGui::CalcTextSize(buf_display, NULL, true);
    float width = custom_width == 0 ? ImGui::CalcItemSize(ImVec2(-0.1f, 0), 0, 0).x : custom_width;
    float height = ImGui::GetFontSize();

    ImVec2 size = ImVec2(width, height);
    ImRect frame_bb(pos, pos + size);
    ImRect total_bb(pos, frame_bb.Max + ImVec2(size.x, 0));

    ImGui::ItemSize(total_bb);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    const bool hovered = ImGui::ItemHoverable(frame_bb, id, 0);

    if (hovered)
    {
        ImGui::SetHoveredID(id);
        g.MouseCursor = ImGuiMouseCursor_Hand;
    }

    const bool user_clicked = hovered && io.MouseClicked[0];
    bool just_activated = false;

    if (user_clicked)
    {
        if (g.ActiveId != id)
        {
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *k = 0;
            just_activated = true;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (io.MouseClicked[0])
    {
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }

    bool value_changed = false;
    int key = *k;

    if (g.ActiveId == id)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            *k = 0;
            ImGui::ClearActiveID();
            return true;
        }

        // Mouse-button binds use the standard Win32 virtual-key values:
        // 0x01 = LButton, 0x02 = RButton, 0x04 = MButton,
        // 0x05 = XButton1, 0x06 = XButton2.
        // The activation click is ignored so clicking the bind box does not instantly
        // bind to LButton.
        if (!just_activated && !value_changed)
        {
            const int mouse_vk[] = { 0x01, 0x02, 0x04, 0x05, 0x06 };
            const int mouse_index[] = { 0, 1, 2, 3, 4 };
            for (int i = 0; i < IM_ARRAYSIZE(mouse_vk); ++i)
            {
                const int idx = mouse_index[i];
                if (idx < IM_ARRAYSIZE(io.MouseClicked) && io.MouseClicked[idx])
                {
                    key = mouse_vk[i];
                    value_changed = true;
                    ImGui::ClearActiveID();
                    break;
                }
            }
        }

        if (!value_changed)
        {
            for (auto i = 0x08; i <= 0xA5; i++)
            {
                if (io.KeysDown[i])
                {
                    key = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                    break;
                }
            }
        }

        *k = key;
    }

    /*
    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);

    const float border_size = g.Style.FrameBorderSize;
    if (border_size > 0.0f)
    {
        window->DrawList->AddRect(frame_bb.Min + ImVec2(1, 1), frame_bb.Max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), style.FrameRounding, 0, border_size);
        window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_Border), style.FrameRounding, 0, border_size);
    }
    */

    ImGui::RenderNavHighlight(total_bb, id);

    if (*k != 0 && g.ActiveId != id)
        strcpy_s(buf_display, sizeof buf_display, szKeyNames[*k]);
    else if (g.ActiveId == id)
        strcpy_s(buf_display, sizeof buf_display, "...");

    const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y);
    ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);

    return value_changed;
}