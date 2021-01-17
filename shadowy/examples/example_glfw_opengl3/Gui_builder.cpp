#include "Gui_builder.h"

#include "imgui_internal.h"

ImVec4 DarkColor[ImGuiCol_COUNT];
ImVec4 CustomColor[ImGuiCol_COUNT];

// TYPE INDENTIFICATIONS
std::string current_item;
int identf = 0, index = 0;
Indentification_btn btn;
Indentification_form frm;
Indentification_text txt;
Indentification_basic chk;
Indentification_basic tlg;
Indentification_slider slider_float;
Indentification_slider slider_integer;
Indentification_basic radio;
child_bar child;

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void PushAllColorsDark()
{
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        ImGui::PushStyleColor((ImGuiCol_)i, DarkColor[i]);
    }
}

void PopAllColorsDark()
{
    ImGui::PopStyleColor(ImGuiCol_COUNT);
}

void PushAllColorsCustom()
{
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        ImGui::PushStyleColor((ImGuiCol_)i, CustomColor[i]);
    }
}

void PopAllColorsCustom()
{
    ImGui::PopStyleColor(ImGuiCol_COUNT);
}

void ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
        *v = !*v;
    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = *v ? IM_COL32(145 + 20, 211, 68 + 20, 255) : IM_COL32(218 - 20, 218 - 20, 218 - 20, 255);
    else
        col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

void color_editor()
{
    ImGuiStyle& style = ImGui::GetStyle();

    static ImGuiStyle ref_saved_style;
    ImGuiStyle* ref;

    ref = &ref_saved_style;
    ImGui::Begin("Gui Builder color export/import ");
  
        static int output_dest = 0;
        static bool output_only_modified = false;

        if (ImGui::Button("Load"))
        {
            std::string line;
            std::ifstream fileimport("colors.col");

            if (fileimport.is_open())
            {
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    std::getline(fileimport, line);
                    auto colors = split(line, ',');
                    ImVec4 prot = ImVec4(std::stof(colors[0]), std::stof(colors[1]), std::stof(colors[2]), std::stof(colors[3]));
                    CustomColor[i] = prot;
                }

                fileimport.close();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Save"))
        {
            remove("colors.col");

            std::ofstream filewrite("colors.col");
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                char* buffer = new char[MAX_PATH];

                ImVec4 col = CustomColor[i];

                sprintf(buffer, "%.2ff, %.2ff, %.2ff, %.2ff\n", col.x, col.y, col.z, col.w);

                filewrite << buffer;

                delete[] buffer;
            }
            filewrite.close();
        }

        ImGui::SameLine();

        if (ImGui::Button("Export"))
        {
            if (output_dest == 0)
            {
                ImGui::LogToClipboard();

                ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;");
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const ImVec4& col = CustomColor[i];
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);",
                            name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }

                ImGui::LogFinish();
            }
        }

        ImGui::SameLine(); 

        ImGui::SetNextItemWidth(120); 
        ImGui::Combo("##output_type", &output_dest, "To Clipboard\0");

        // ImGui::SameLine(); 

        // ImGui::Checkbox("Only Modified Colors", &output_only_modified);

        static ImGuiTextFilter filter;
        filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

        static ImGuiColorEditFlags alpha_flags = 0;
        if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
        if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
        if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; }// ImGui::SameLine();


        ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
        ImGui::PushItemWidth(-160);
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName(i);
            if (!filter.PassFilter(name))
                continue;
            ImGui::PushID(i);
            ImGui::ColorEdit4("##color", (float*)&CustomColor[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
            if (memcmp(&CustomColor[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
            {
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = CustomColor[i]; }
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { CustomColor[i] = ref->Colors[i]; }
            }
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
            ImGui::TextUnformatted(name);
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();


    ImGui::End();
}

void Gui_builder::init_DarkColor()
{
        DarkColor[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        DarkColor[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        DarkColor[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
        DarkColor[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        DarkColor[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        DarkColor[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        DarkColor[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        DarkColor[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
        DarkColor[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        DarkColor[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        DarkColor[ImGuiCol_TitleBg]                = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
        DarkColor[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
        DarkColor[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        DarkColor[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        DarkColor[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        DarkColor[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        DarkColor[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        DarkColor[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        DarkColor[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        DarkColor[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        DarkColor[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        DarkColor[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        DarkColor[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        DarkColor[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        DarkColor[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        DarkColor[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        DarkColor[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        DarkColor[ImGuiCol_Separator]              = DarkColor[ImGuiCol_Border];
        DarkColor[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        DarkColor[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        DarkColor[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        DarkColor[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        DarkColor[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        DarkColor[ImGuiCol_Tab]                    = ImLerp(DarkColor[ImGuiCol_Header], DarkColor[ImGuiCol_TitleBgActive], 0.80f);
        DarkColor[ImGuiCol_TabHovered]             = DarkColor[ImGuiCol_HeaderHovered];
        DarkColor[ImGuiCol_TabActive]              = ImLerp(DarkColor[ImGuiCol_HeaderActive], DarkColor[ImGuiCol_TitleBgActive], 0.60f);
        DarkColor[ImGuiCol_TabUnfocused]           = ImLerp(DarkColor[ImGuiCol_Tab], DarkColor[ImGuiCol_TitleBg], 0.80f);
        DarkColor[ImGuiCol_TabUnfocusedActive]     = ImLerp(DarkColor[ImGuiCol_TabActive], DarkColor[ImGuiCol_TitleBg], 0.40f);
        DarkColor[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        DarkColor[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        DarkColor[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        DarkColor[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        DarkColor[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        DarkColor[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        DarkColor[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        DarkColor[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        DarkColor[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        DarkColor[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void Gui_builder::init_CustomColor()
{
        CustomColor[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        CustomColor[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        CustomColor[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
        CustomColor[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        CustomColor[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        CustomColor[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        CustomColor[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        CustomColor[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
        CustomColor[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        CustomColor[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        CustomColor[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
        CustomColor[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
        CustomColor[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        CustomColor[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        CustomColor[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        CustomColor[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        CustomColor[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        CustomColor[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        CustomColor[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        CustomColor[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        CustomColor[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        CustomColor[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        CustomColor[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        CustomColor[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        CustomColor[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        CustomColor[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        CustomColor[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        CustomColor[ImGuiCol_Separator]              = CustomColor[ImGuiCol_Border];
        CustomColor[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        CustomColor[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        CustomColor[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        CustomColor[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        CustomColor[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        CustomColor[ImGuiCol_Tab]                    = ImLerp(CustomColor[ImGuiCol_Header], CustomColor[ImGuiCol_TitleBgActive], 0.80f);
        CustomColor[ImGuiCol_TabHovered]             = CustomColor[ImGuiCol_HeaderHovered];
        CustomColor[ImGuiCol_TabActive]              = ImLerp(CustomColor[ImGuiCol_HeaderActive], CustomColor[ImGuiCol_TitleBgActive], 0.60f);
        CustomColor[ImGuiCol_TabUnfocused]           = ImLerp(CustomColor[ImGuiCol_Tab], CustomColor[ImGuiCol_TitleBg], 0.80f);
        CustomColor[ImGuiCol_TabUnfocusedActive]     = ImLerp(CustomColor[ImGuiCol_TabActive], CustomColor[ImGuiCol_TitleBg], 0.40f);
        CustomColor[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        CustomColor[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        CustomColor[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        CustomColor[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        CustomColor[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        CustomColor[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        CustomColor[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        CustomColor[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        CustomColor[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        CustomColor[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void Gui_builder::drawimgui(GLFWwindow* window, HWND hWnd)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    mainform_draw(hWnd);

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui_builder::mainform_draw(HWND wnd)
{
    PushAllColorsDark();

    ImGui::SetNextWindowSize({ 1280,150 });
    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::Begin("BUILDER", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("Save"))
            {
                remove("project.builder");

                std::ofstream filewrite("project.builder");



                filewrite.close();
            }

            if (ImGui::MenuItem("Open"))
            {
                std::ifstream fileread("project.builder");

                if (fileread.is_open())
                {


                    fileread.close();
                }
            }

            if (ImGui::MenuItem("Generate Code"))
            {
                create_builder();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (ImGui::Button("New Form"))
        create_from();
    ImGui::SameLine();
    if (ImGui::Button("New Button"))
        create_button();
    ImGui::SameLine();
    if (ImGui::Button("New Text"))
        create_text();
    ImGui::SameLine();
    if (ImGui::Button("New CheckBox"))
        create_chk();
    ImGui::SameLine();
    if (ImGui::Button("New Toggle Button"))
        create_toggle();
    ImGui::SameLine();
    if (ImGui::Button("New Slider Int"))
        create_sliderI();
    ImGui::SameLine();
    if (ImGui::Button("New Slider float"))
        create_sliderF();
    ImGui::SameLine();
    if (ImGui::Button("New RadioButton"))
        create_radio();
    ImGui::SameLine();
    if (ImGui::Button("New Child"))
        create_child();
    ImGui::SameLine();

    /* if (ImGui::Button("Generate Code"))
    {
        create_builder();
    } */

    ImGui::SameLine();

    static bool color;
    ImGui::Checkbox("Editor", &color);

    if (color)
    {
        color_editor();
    }

    show_propriets_geral();

    PopAllColorsDark();

    PushAllColorsCustom();

    show_form(wnd);

    PopAllColorsCustom();

    ImGui::End();
}

void Gui_builder::create_from()
{
    ++id;
    std::string form_name = "form id:";
    form_name.append(std::to_string(id));
    forms.push_back({ id,  {200,100}, false,form_name });
}

void Gui_builder::create_button()
{
    btn_id++;
    std::string name = "button";
    name.append(std::to_string(btn_id));
    buttons.push_back({ id, btn_id, {10,10}, {0,0} , false, name });
}

void Gui_builder::create_text()
{
    txt_id++;
    std::string name = "txt_edit";
    name.append(std::to_string(txt_id));
    texts.push_back({ id, txt_id, {10,20}, 0 , false, name, "Text here" });
}

void Gui_builder::create_toggle()
{
    tg_id++;
    std::string name = "toggle" + std::to_string(tg_id);
    toggle.push_back({id, tg_id, {10,20}, name });
}

void Gui_builder::create_chk()
{
    chk_id++;
    std::string name = "checkbox" + std::to_string(chk_id);
    checkbox.push_back({id, chk_id, {10,20}, name });
}

void Gui_builder::create_sliderI()
{
    si++;
    std::string name = "SliderInt" + std::to_string(si);
    SliderI.push_back({ id, si, {10,20},0, name });
}

void Gui_builder::create_sliderF()
{
    sf++;
    std::string name = "SliderFloat" + std::to_string(sf);
    SliderF.push_back({ id, sf, {10,20},0, name });
}

void Gui_builder::create_radio()
{
    rd++;
    std::string name = "Radio" + std::to_string(rd);
    Radio.push_back({id, rd, {10,20}, name});
}

void Gui_builder::create_child()
{
    chld++;
    std::string name = "Child:" + std::to_string(chld);

    childs.push_back({ {id, chld, {10,20}, name}, true, {30,30} });

}

void Gui_builder::show_form(HWND window)
{
    Gui_builder::window = window;

    for (size_t id_form = 0;  id_form < forms.size(); id_form++)
    {
        if(forms[id_form].edtion)
        {           
            continue;
        }

        ImGui::SetNextWindowSize(forms[id_form].size);

        ImGui::Begin(forms[id_form].name_form.c_str());

        forms[id_form].size = ImGui::GetWindowSize();

        //Buttons
        for (auto& button : buttons)
        {
            if (button.Form_id == forms[id_form].Form_id)
            {
                ImGui::SetCursorPos(button.Pos_item);
                ImGui::Button(button.name_item.c_str(), button.size_item);
            }
        }

        //Texts
        for (auto& text : texts)
        {
            if (text.Form_id == forms[id_form].Form_id)
            {
                ImGui::SetCursorPos(text.Pos_item);
                ImGui::PushItemWidth(text.wight);
                ImGui::InputText(text.name_text.c_str(), const_cast<char*>(text.same_buffer.c_str()), 25);
            }
        }

        //CheckBox
        for (auto& id_chk : checkbox)
        {
            if (id_chk.form_id == forms[id_form].Form_id)
            {
                ImGui::SetCursorPos(id_chk.Pos_item);
                static bool vb;
                ImGui::Checkbox(id_chk.name.c_str(), &vb);
            }
        }

        //Toggle
        for (auto& id_tg : toggle)
        {
            if (id_tg.form_id == forms[id_form].Form_id)
            {
                ImGui::SetCursorPos(id_tg.Pos_item);
                static bool vb;
                ToggleButton(id_tg.name.c_str(), &vb);
            }
        }
        //Radio box
        for(auto i = 0; i < Radio.size(); ++i)
        {
            if(Radio[i].form_id == forms[id_form].Form_id)
            {
                ImGui::SetCursorPos(Radio[i].Pos_item);
                static int vi;
                ImGui::RadioButton(Radio[i].name.c_str(), &vi,i);
            }
        }
        //Slider Integer
        for (auto& id_si : SliderI)
        {
            if (id_si.Form_id == forms[id_form].Form_id)
            {
                ImGui::SetCursorPos(id_si.Pos_item);
                static int vl;
                ImGui::PushItemWidth(id_si.wight);
                ImGui::SliderInt(id_si.name.c_str(), &vl, 0,100);
            }
        }

        //Slider Float
        for (auto& id_si : SliderF)
        {
            if (id_si.Form_id == forms[id_form].Form_id)
            {
                ImGui::SetCursorPos(id_si.Pos_item);
                static float vl;
                ImGui::PushItemWidth(id_si.wight);
                ImGui::SliderFloat(id_si.name.c_str(), &vl, 0, 100);
            }
        }

        //Child
        for (auto& id_child : childs)
        {
            if (id_child.a.form_id == forms[id_form].Form_id)
            {
                ImGui::SetCursorPos(id_child.a.Pos_item);
                ImGui::BeginChild(id_child.a.name.c_str(), id_child.size, id_child.borda);


                ImGui::EndChild();
            }
        }

        ImGui::End();
    }

}

//Move obj with cursor
POINT OLD_POS = { 0,0 };
int count_pos = 0;
ImVec2 Move_item(ImVec2 Obj_pos, HWND window)
{
    if (OLD_POS.x == 0 && count_pos == 0)
        GetCursorPos(&OLD_POS);

    else if (count_pos == 1)
        SetCursorPos(Obj_pos.x, Obj_pos.y);


    POINT new_pos;
    if (GetAsyncKeyState(VK_RBUTTON) && window == GetForegroundWindow())
    {
        if (count_pos >= 4)
        {
            GetCursorPos(&new_pos);
            Obj_pos.x = new_pos.x;
            Obj_pos.y = new_pos.y;
        }
        ++count_pos;
    }
    else
    {
        count_pos = 0;
        SetCursorPos(OLD_POS.x, OLD_POS.y);
        OLD_POS = { 0,0 };
    }
    return Obj_pos;
}

void Gui_builder::show_propriets_geral()
{

    ImGui::SetNextWindowPos({ 0,150 });
    ImGui::SetNextWindowSize({ 200,700 - 150 });
    ImGui::Begin("PROPRIEDADES", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (ImGui::BeginCombo("##itens", current_item.c_str()))
    {
        for (auto n = 0; n < forms.size(); n++)
        {
            if (forms[n].edtion)
            {
                forms[n].edtion = false;
                forms.erase(forms.begin() + n);

                break;
            }
              
            const bool is_selected = (current_item == forms[n].name_form);
            if (ImGui::Selectable(forms[n].name_form.c_str(), is_selected))
            {
                current_item = forms[n].name_form;
                frm = forms[n];
                index = n;
                identf = 1; // forms
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        for (auto n = 0; n < buttons.size(); n++)
        {
            auto is_selected = (current_item == buttons[n].name_item);
            if (ImGui::Selectable(buttons[n].name_item.c_str(), is_selected))
            {
                current_item = buttons[n].name_item;
                btn = buttons[n];
                index = n;
                identf = 2;// buttons
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        for (auto n = 0; n < texts.size(); n++)
        {
            const auto is_selected = (current_item == texts[n].name_text);
            if (ImGui::Selectable(texts[n].name_text.c_str(), is_selected))
            {
                current_item = texts[n].name_text;
                txt = texts[n];
                index = n;
                identf = 3;

            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        for (auto n = 0; n < checkbox.size(); n++)
        {
            const auto is_selected = (current_item == checkbox[n].name);
            if (ImGui::Selectable(checkbox[n].name.c_str(), is_selected))
            {
                current_item = checkbox[n].name;
                chk = checkbox[n];
                index = n;
                identf = 4;

            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        for (auto n = 0; n < toggle.size(); n++)
        {
            const auto is_selected = (current_item == toggle[n].name);
            if (ImGui::Selectable(toggle[n].name.c_str(), is_selected))
            {
                current_item = toggle[n].name;
                tlg = toggle[n];
                index = n;
                identf = 5;

            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();

        }
        for (auto n = 0; n < SliderI.size(); n++)
        {
            const bool is_selected = (current_item == SliderI[n].name);
            if (ImGui::Selectable(SliderI[n].name.c_str(), is_selected))
            {
                current_item = SliderI[n].name;
                slider_integer = SliderI[n];
                index = n;
                identf = 6;

            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();

        }
        for (auto n = 0; n < SliderF.size(); n++)
        {
            const bool is_selected = (current_item == SliderF[n].name);
            if (ImGui::Selectable(SliderF[n].name.c_str(), is_selected))
            {
                current_item = SliderF[n].name;
                slider_float = SliderF[n];
                index = n;
                identf = 7;

            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();

        }
        for (auto n = 0; n < Radio.size(); n++)
        {
            const bool is_selected = (current_item == Radio[n].name);
            if (ImGui::Selectable(Radio[n].name.c_str(), is_selected))
            {
                current_item = Radio[n].name;
                radio = Radio[n];
                index = n;
                identf = 8;

            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();

        }

        for (auto n = 0; n < childs.size(); n++)
        {
            const bool is_selected = (current_item == childs[n].a.name);
            if (ImGui::Selectable(childs[n].a.name.c_str(), is_selected))
            {
                current_item = childs[n].a.name;
                child = childs[n];
                index = n;
                identf = 20;

            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();

        }

        ImGui::EndCombo();
    }
    ImGui::End();

    switch (identf)
    {
    case 1:
        show_propriedades_form(frm);
        forms[index] = frm;
        break;

    case 2:
        show_propriedades_btn(btn);
        buttons[index] = btn;
        break;

    case 3:
        show_propriedades_txt(txt);
        texts[index] = txt;
        break;
    case 4:
        show_propriedades_chk_tlg(chk);
        checkbox[index] = chk;
        break;
    case 5:
        show_propriedades_chk_tlg(tlg);
        toggle[index] = tlg;
        break;

    case 6:
        show_propriedades_slider(slider_integer);
        SliderI[index] = slider_integer;
        break;
    case 7:
        show_propriedades_slider(slider_float);
        SliderF[index] = slider_float;
        break;
    case 8:
        show_propriedades_chk_tlg(radio);
        Radio[index] = radio;
        break;

    case 20:
        show_child_propriedade(child);
        childs[index] = child;
        break;

    default:
        break;
    }
}

void Gui_builder::show_propriedades_chk_tlg(Indentification_basic& checkbox_toggle)
{
    ImGui::SetNextWindowPos({ 0,150 });
    ImGui::SetNextWindowSize({ 200,700 - 150 });
    ImGui::Begin("PROPRIEDADES", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);
    float v[2] = { checkbox_toggle.Pos_item.x, checkbox_toggle.Pos_item.y };
    ImGui::SliderFloat("posX", &v[0], 0, 1000);
    ImGui::SliderFloat("posY", &v[1], 0, 1000);
    auto* name = const_cast<char*>(checkbox_toggle.name.c_str());
    ImGui::InputText("Name", name, 100);

    int father = checkbox_toggle.form_id;
    ImGui::InputInt("ID Form PAI", &father, 1, 10);
    checkbox_toggle.form_id = father;
    checkbox_toggle.Pos_item.x = v[0];
    checkbox_toggle.Pos_item.y = v[1];
    checkbox_toggle.Pos_item = Move_item(checkbox_toggle.Pos_item, window);

    checkbox_toggle.name = name;
    ImGui::End();
}

void Gui_builder::show_propriedades_slider(Indentification_slider& slider)
{
    ImGui::SetNextWindowPos({ 0,150 });
    ImGui::SetNextWindowSize({ 200,700 - 150 });
    ImGui::Begin("PROPRIEDADES", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);
    float v[2] = { slider.Pos_item.x, slider.Pos_item.y };
    ImGui::SliderFloat("posX", &v[0], 0, 1000);
    ImGui::SliderFloat("posY", &v[1], 0, 1000);
    auto* name = const_cast<char*>(slider.name.c_str());
    ImGui::InputText("Name", name, 100);
    float width = slider.wight;

    ImGui::InputFloat("width", &width, 1, 1);
    int father = slider.Form_id;
    ImGui::InputInt("ID Form PAI", &father, 1, 10);
    slider.Form_id = father;
    slider.Pos_item.x = v[0];
    slider.Pos_item.y = v[1];
    slider.Pos_item = Move_item(slider.Pos_item, Gui_builder::window);
    slider.wight = width;

    slider.name = name;
    ImGui::End();
}

void Gui_builder::show_propriedades_btn(Indentification_btn& item_button)
{
    ImGui::SetNextWindowPos({ 0,150 });
    ImGui::SetNextWindowSize({ 200,700-150 });
    ImGui::Begin("PROPRIEDADES", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);
    float v[2] = {item_button.Pos_item.x, item_button.Pos_item.y};
    ImGui::SliderFloat("posX", &v[0], 0,1000);
    ImGui::SliderFloat("posY", &v[1], 0, 1000);
    auto *name = const_cast<char*>( item_button.name_item.c_str());
    ImGui::InputText("Name", name, 100);
    float width = item_button.size_item.x;
    float height = item_button.size_item.y;
    ImGui::InputFloat("width", &width, 1, 1);
    ImGui::InputFloat("height", &height, 1, 1);
    int father = item_button.Form_id;
    ImGui::InputInt("ID Form PAI", &father, 1, 10);

    ImVec2 size(width, height);

    item_button.Form_id = father;
    item_button.Pos_item.x = v[0];
    item_button.Pos_item.y = v[1];
    item_button.Pos_item = Move_item(item_button.Pos_item, window);
    item_button.size_item = size;
    item_button.name_item = name;

    ImGui::End();
}

void Gui_builder::show_propriedades_txt(Indentification_text& text)
{
    ImGui::SetNextWindowPos({ 0,150 });
    ImGui::SetNextWindowSize({ 200,700 - 150 });
    ImGui::Begin("PROPRIEDADES", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);
    float v[2] = { text.Pos_item.x, text.Pos_item.y };
    ImGui::SliderFloat("posX", &v[0], 0, 1000);
    ImGui::SliderFloat("posY", &v[1], 0, 1000);
    auto* name = const_cast<char*>(text.name_text.c_str());
    ImGui::InputText("Name", name, 100);
    
    float width = text.wight;
  
    ImGui::InputFloat("width", &width, 1, 1);
    int father = text.Form_id;
    ImGui::InputInt("ID Form PAI", &father, 1, 10);



    text.Form_id = father;
    text.Pos_item.x = v[0];
    text.Pos_item.y = v[1];
    text.Pos_item = Move_item(text.Pos_item, window );
    text.wight = width;
    text.name_text = name;
    ImGui::End();
}

void Gui_builder::show_propriedades_form(Indentification_form& form)
{
    ImGui::SetNextWindowPos({ 0,150 });
    ImGui::SetNextWindowSize({ 200,700 - 150 });
    ImGui::Begin("PROPRIEDADES", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);

    auto* name = const_cast<char*>(form.name_form.c_str());
    int ids = form.Form_id;
    ImGui::InputInt("ID", &ids, 1, 1);
    ImGui::InputText("Name", name, 100);
    ImGui::InputFloat("width", &form.size.x, 1, 1);
    ImGui::InputFloat("height", &form.size.y, 1, 1);

    // ImVec2 size(width, height);

    form.Form_id = ids;
    // form.size = size;
    form.name_form = name;

    if (ImGui::Button("APAGAR"))
    {
        form.edtion = true;
        identf = 0;
        current_item = "";
    }

    ImGui::End();
}

void Gui_builder::show_child_propriedade(child_bar& child)
{
    ImGui::SetNextWindowPos({ 0,150 });
    ImGui::SetNextWindowSize({ 200,700 - 150 });
    ImGui::Begin("PROPRIEDADES", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);
    float v[2] = { child.a.Pos_item.x, child.a.Pos_item.y };
    ImGui::SliderFloat("posX", &v[0], 0, 1000);
    ImGui::SliderFloat("posY", &v[1], 0, 1000);
    auto* name = const_cast<char*>(child.a.name.c_str());
    ImGui::InputText("Name", name, 100);
    ImGui::Checkbox("Borda", &child.borda);
    float height = child.size.x;
    float width = child.size.y;
    ImGui::InputFloat("height", &height, 1, 1);
    ImGui::InputFloat("width", &width, 1, 1);

    child.size = { height,width };
    int father = child.a.form_id;
    ImGui::InputInt("ID Form PAI", &father, 1, 10);
    child.a.form_id = father;
    child.a.Pos_item.x = v[0];
    child.a.Pos_item.y = v[1];
    child.a.Pos_item = Move_item(child.a.Pos_item, window);

    child.a.name = name;
    ImGui::End();
}

void Gui_builder::create_builder()
{
    file_builder = "";
    for (size_t i = 0; i < forms.size(); ++i)
    {
        file_builder.append("\nvoid Gui_builder" + std::to_string(i) + "()\n{\n");
        file_builder.append("ImGui::SetNextWindowSize({" + std::to_string(static_cast<int>(forms[i].size.x)) + ".f," + std::to_string(static_cast<int>(forms[i].size.y)) + ".f});\n");
        file_builder.append("ImGui::Begin(\"" + forms[i].name_form + "\");");
        file_builder.append("\n");
        for (auto& button : buttons)
        {
            if (button.Form_id == forms[i].Form_id)
            {
                file_builder.append("ImGui::SetCursorPos({" + std::to_string(static_cast<int>(button.Pos_item.x)) + ".f," + std::to_string(static_cast<int>(
                    button.Pos_item.y)) + ".f});\n");
                file_builder.append("ImGui::Button(\"" + button.name_item + "\", {" + std::to_string(static_cast<int>(
                    button.size_item.x)) + ".f," + std::to_string(static_cast<int>(button.size_item.y)) + ".f});\n\n");
            }
        }
        for (auto& text : texts)
        {
            if (text.Form_id == forms[i].Form_id)
            {
                file_builder.append("ImGui::SetCursorPos(" + std::to_string(static_cast<int>(text.Pos_item.x)) + ".f," + std::to_string(static_cast<int>(text.Pos_item.y)) + ".f);\n");
                file_builder.append("ImGui::PushItemWidth(" + std::to_string(text.wight) + ".f);\n");
                file_builder.append("ImGui::InputText(\"" + text.name_text + "\",\"" + text.same_buffer + "\",25);\n\n");
            }
        }
        for (auto& chk : checkbox)
        {
            if (chk.form_id == forms[i].Form_id)
            {
                file_builder.append("ImGui::SetCursorPos(" + std::to_string(static_cast<int>(chk.Pos_item.x)) + ".f," + std::to_string(static_cast<int>(chk.Pos_item.y)) + ".f);\n");
                file_builder.append("static bool var_bool;\n");
                file_builder.append("ImGui::Checkbox(" + chk.name + ", var_bool);\n\n");
            }
        }

        for (auto& tgl : toggle)
        {
            if (tgl.form_id == forms[i].Form_id)
            {
                file_builder.append("ImGui::SetCursorPos(" + std::to_string(static_cast<int>(tgl.Pos_item.x)) + ".f," + std::to_string(static_cast<int>(tgl.Pos_item.y)) + ".f);\n");
                file_builder.append("static bool var_tgl;\n");
                file_builder.append("ToggleButton(" + tgl.name + ", var_tgl);\n\n");
            }
        }

        for (auto& radio : toggle)
        {
            if (radio.form_id == forms[i].Form_id)
            {
                file_builder.append("ImGui::SetCursorPos(" + std::to_string(static_cast<int>(radio.Pos_item.x)) + ".f," + std::to_string(static_cast<int>(radio.Pos_item.y)) + ".f);\n");
                file_builder.append("static bool var_radio;\n");
                file_builder.append(" ImGui::RadioButton(\"" + radio.name + "\", var_radio);\n\n");
            }
        }

        for (auto& si : SliderI)
        {
            if (si.Form_id == forms[i].Form_id)
            {
                file_builder.append("ImGui::SetCursorPos(" + std::to_string(static_cast<int>(si.Pos_item.x)) + ".f," + std::to_string(static_cast<int>(si.Pos_item.y)) + ".f);\n");
                file_builder.append("ImGui::PushItemWidth(" + std::to_string(si.wight) + ".f);\n");
                file_builder.append("Static int var_int;\n");
                file_builder.append(" ImGui::SliderInt(\"" + si.name + "\", &var_int, 0,100);\n\n");

            }
        }
        for (auto& sf : SliderF)
        {
            if (sf.Form_id == forms[i].Form_id)
            {
                file_builder.append("ImGui::SetCursorPos(" + std::to_string(static_cast<int>(sf.Pos_item.x)) + ".f," + std::to_string(static_cast<int>(sf.Pos_item.y)) + ".f);\n");
                file_builder.append("ImGui::PushItemWidth(" + std::to_string( static_cast<int>( sf.wight)) + ".f);\n");
                file_builder.append("Static float var_float;\n");
                file_builder.append("ImGui::SliderFloat(\"" + sf.name + "\", &var_float, 0,100);\n\n");
            }
        }

        for (auto& childer : childs)
        {
            if (childer.a.form_id == forms[i].Form_id)
            {
                file_builder.append("ImGui::SetCursorPos(" + std::to_string(static_cast<int>(childer.a.Pos_item.x)) + ".f," + std::to_string(static_cast<int>(childer.a.Pos_item.y)) + ".f);\n");
                file_builder.append("ImGui::BeginChild(\"" + childer.a.name+ "\",{"
                 + std::to_string(static_cast<int>(childer.size.x))+ ".f," + std::to_string(static_cast<int>(childer.size.y)) + ".f}, " + std::to_string(childer.borda) +");\n\n\n");
                file_builder.append("//Move code enter to your group child\n\n\n");
                file_builder.append("ImGui::EndChild();\n");
            }
        }



        file_builder.append("\n");
        file_builder.append("ImGui::End();");
        file_builder.append("\n}");
    }

    std::ofstream file_to_save;
    std::string name_file = "ImGuiBuilder_";
    name_file.append("0.cpp");
    file_to_save.open(name_file.c_str());
    for (auto i : file_builder)
        file_to_save << i;

    file_to_save.close();

    MessageBoxA(NULL, "Code been generated!", "ImGui Builder", MB_OK | MB_ICONINFORMATION);
}