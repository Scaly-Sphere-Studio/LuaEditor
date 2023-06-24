#include "LuaEditor.hpp"
#include <regex>

int LuaEditor::callback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag != ImGuiInputTextFlags_CallbackCompletion || reset_cursor) {
        if (all_keys) {
            if (!last_key.empty()) {
                buffer = data->Buf;
                last_key.clear();
            }
            all_keys.reset();
        }
        cursor = data->CursorPos;
        reset_cursor = false;
    }

    switch (data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion:
        break;
    case ImGuiInputTextFlags_CallbackEdit:
        buffer = data->Buf;
    default:
        return 0;
    }

    std::string const buf = buffer.substr(0, cursor);

    auto const [table, key, separator] = [buf]() {
        std::regex const r("[.:]");

        std::string key = [](std::string buf) {
            std::regex const r("[ (;,\n\t]");
            std::smatch sm;
            while (std::regex_search(buf, sm, r)) {
                buf = sm.suffix();
            }
            return buf;
        }(buf);

        std::string table;
        char separator = '.';
        std::smatch sm;
        while (std::regex_search(key, sm, r)) {
            if (!table.empty()) {
                table += separator;
            }
            separator = sm[0].str().at(0);
            table += sm.prefix().str();
            key = sm.suffix().str();
        }
        return std::make_tuple(table, key, separator);
    }();

    if (!all_keys) {
        sol::table t = env;
        if (!table.empty()) {
            auto res = lua.safe_script("return " + table, env, sol::script_pass_on_error);
            if (!res.valid() || (res.get_type() != sol::type::table &&
                res.get_type() != sol::type::userdata))
                return 0;
            t = res;
        }
        all_keys = std::make_unique<LuaTableData const>(lua, t, env, table);
    }

    if (!last_key.empty()) {
        data->DeleteChars(buf.size(), last_key.size() - key.size());
        auto it = all_keys->find(last_key);
        if (it != all_keys->cend()) {
            ++it;
            while (it != all_keys->cend() && it->second.separator != separator)
                ++it;
            if (it != all_keys->cend()) {
                std::string const& new_key = it->first;
                if (new_key.find(key) == 0) {
                    data->InsertChars(buf.size(), new_key.c_str() + key.size());
                    last_key = new_key;
                    return 0;
                }
            }
        }
    }

    for (auto const& [k, v] : *all_keys) {
        if (v.separator == separator && k.find(key) == 0 && (!last_key.empty() || k != key)) {
            data->InsertChars(buf.size(), k.c_str() + key.size());
            last_key = k;
            return 0;
        }
    }
}
