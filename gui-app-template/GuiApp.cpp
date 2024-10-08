/******************************************************************************************
*                                                                                         *
*    GUI Application Base                                                                 *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "GuiApp.hpp"

GuiApp::GuiApp(std::string_view name) : appName(name)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        std::exit(1);

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    window = glfwCreateWindow(1, 1, appName.c_str(), nullptr, nullptr);
    if (window == nullptr)
        std::exit(1);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Close callback
    glfwSetWindowCloseCallback(window, window_close_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigDockingWithShift = false;                          // Enable docking with holding shift key
    io.ConfigWindowsResizeFromEdges = true;                     // Allow resizing windows from their edge.
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup ImPlot style
    ImPlot::StyleColorsDark();
    //ImPlot::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);


    // ==== Fonts and Icons  ====

    // Add font RobotoMedium
    float sizePixels = 16.0f;
    fontRobotoMedium = io.Fonts->AddFontFromMemoryCompressedTTF(&RobotoMedium_compressed_data, RobotoMedium_compressed_size, sizePixels);
    IM_ASSERT(fontRobotoMedium != nullptr);

    // Merge icons to font RobotoMedium
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    config.GlyphOffset = ImVec2(0.0f, 3.0f); // Offset
    static const ImWchar ranges[] = { 0xe000, 0xf3ff, 0 };
    fontMaterialIcons = io.Fonts->AddFontFromMemoryCompressedTTF(&MaterialIcons_compressed_data, MaterialIcons_compressed_size, sizePixels, &config, ranges);
    IM_ASSERT(fontMaterialIcons != nullptr);

    // Add font RobotoRegular
    fontRobotoRegular = io.Fonts->AddFontFromMemoryCompressedTTF(&RobotoRegular_compressed_data, RobotoRegular_compressed_size, sizePixels);
    IM_ASSERT(fontRobotoRegular != nullptr);

    // Add large font
    sizePixels = 20.0f;
    fontLarge = io.Fonts->AddFontFromMemoryCompressedTTF(&RobotoMedium_compressed_data, RobotoMedium_compressed_size, sizePixels);
    IM_ASSERT(fontLarge != nullptr);

    // Merge large icons to large font
    ImFontConfig configLarge;
    configLarge.MergeMode = true;
    configLarge.PixelSnapH = true;
    configLarge.GlyphOffset = ImVec2(0.0f, 4.0f); // Offset
    static const ImWchar rangesLarge[] = { 0xe000, 0xf3ff, 0 };
    iconLarge = io.Fonts->AddFontFromMemoryCompressedTTF(&MaterialIcons_compressed_data, MaterialIcons_compressed_size, sizePixels, &configLarge, rangesLarge);
    IM_ASSERT(iconLarge != nullptr);

    // Path to ini file
    io.IniFilename = "layout.ini";
}

GuiApp::~GuiApp()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void GuiApp::glfw_error_callback(int error, const char * description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void GuiApp::window_close_callback(GLFWwindow* window)
{
    glfwSetWindowShouldClose(window, false);
    attemptToClose = true;
}

void GuiApp::Run()
{
    const ImGuiIO& io = ImGui::GetIO();
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Pure virtual function
        Update();

        // Exit
        if (timeToClose == true)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // Rendering
        ImGui::Render();
        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
}

void GuiApp::SetTitle(std::string_view name)
{
    title = name;
    UpdateTitle();
}

void GuiApp::SetAsterisk(bool flag)
{
    titleAsterisk = flag;
    UpdateTitle();
}

void GuiApp::UpdateTitle()
{
    glfwSetWindowTitle(window, (title + (titleAsterisk ? "*" : "") + " - " + appName).c_str());
}
