#include "LuaEditor.hpp"

extern sol::state lua{};

static int callback(ImGuiInputTextCallbackData* data)
{
    int ret = 0;
    if (data->EventFlag != ImGuiInputTextFlags_CallbackCompletion) {
        auto editor = reinterpret_cast<LuaEditor*>(data->UserData);
        ret = editor->callback(data);
    }
    else {
        //console->complete.all_keys.reset();
        //console->complete.last_key.clear();
    }

    //g->lua_editable_scene = std::make_unique<BasicScene>(data->Buf);
    //mylua_console_env = &g->lua_editable_scene->getEnv();

    return ret;
}

int main() try
{
    using namespace SSS;
    
    GL::Window& window = GL::Window::create({ 800, 800, "LuaEditor", 0, false, true });

    ImGuiH::setContext(window.getGLFWwindow());

    LuaEditor editor;
    static std::array<char, 0x5000000> buffer;
    
    constexpr ImGuiWindowFlags win_flags
        = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
    ;
    constexpr ImGuiInputTextFlags text_flags
        = ImGuiInputTextFlags_CallbackCompletion
        | ImGuiInputTextFlags_CallbackEdit
    ;

    while (!window.shouldClose()) {
        GL::pollEverything();

        if (!ImGuiH::newFrame())
            continue;

        int w, h;
        window.getDimensions(w, h);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(w), static_cast<float>(h)));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        if (ImGui::Begin("Editor", nullptr, win_flags)) {
            ImGui::InputTextMultiline("##editor", buffer.data(), buffer.size(),
                ImVec2(-1, -1), text_flags, callback, &editor);

            ImGui::End();
        }

        ImGuiH::render();
        window.printFrame();
    }

}
CATCH_AND_LOG_FUNC_EXC;