#include "ImageBtnText.h"

bool ImageButtonWithText(ImTextureID texId, const std::string& label, unsigned int* _size,
    bool* hovered,
    const ImVec2& imageSize,
    const ImVec2& uv0,
    const ImVec2& uv1,
    int frame_padding,
    const ImVec4& bg_col,
    const ImVec4& tint_col,
    unsigned int index) {

    std::string l_label = label;
    while (l_label.size() > 16) {
        l_label.pop_back();
    }

    l_label += "##" + std::to_string(index);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImVec2 size = imageSize;
    if (size.x <= 0 && size.y <= 0) { size.x = size.y = ImGui::GetTextLineHeightWithSpacing(); }
    else {
        if (size.x <= 0)          size.x = size.y;
        else if (size.y <= 0)     size.y = size.x;
        size *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
    }

    ImGuiContext& g = *ImGui::GetCurrentContext();
    const ImGuiStyle& style = g.Style;

    ImGuiID id = window->GetID(l_label.c_str());
    const ImVec2 textSize = ImGui::CalcTextSize(l_label.c_str(), NULL, true);
    const bool hasText = textSize.x > 0;

    const float innerSpacing = hasText ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
    ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;


    int text_end = (int)(padding.x + textSize.x);
    int addv = text_end > (int)size.x + (int)innerSpacing ? text_end - (int)size.x - (int)innerSpacing : 0;

    const ImVec2 totalSizeWithoutPadding(size.x + innerSpacing + addv, size.y + textSize.y);
    if(_size != nullptr)
        *_size = (int)totalSizeWithoutPadding.x;

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + totalSizeWithoutPadding + padding * 2);
    ImVec2 start(0, 0);
    start = window->DC.CursorPos + padding; 
    if (size.y < textSize.y) 
        start.y += (textSize.y - size.y) * .5f;
    const ImRect image_bb(start, start + size);
    start = window->DC.CursorPos + padding;
    start.y += size.y;
    //start.x += size.x + innerSpacing; 
    //if (size.y > textSize.y) start.y += (size.y - textSize.y) * .5f;
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool _hovered = false, held = false;
    bool pressed = ImGui::ButtonBehavior(bb, id, &_hovered, &held);
    if (hovered != nullptr)
        *hovered = _hovered;
    // Render
    const ImU32 col = ImGui::GetColorU32((_hovered && held) ? ImGuiCol_ButtonActive : _hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    ImGui::RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
    if (bg_col.w > 0.0f)
        window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, ImGui::GetColorU32(bg_col));

    window->DrawList->AddImage(texId, image_bb.Min, image_bb.Max, uv0, uv1, ImGui::GetColorU32(tint_col));

    if (textSize.x > 0) ImGui::RenderText(start, l_label.c_str());

    return pressed;
}