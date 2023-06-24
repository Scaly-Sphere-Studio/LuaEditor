#pragma once

#include "LuaTableData.hpp"

struct LuaEditor {

    std::string buffer;

    sol::environment env;

    std::unique_ptr<LuaTableData const> all_keys;
    size_t cursor{ 0 };
    bool reset_cursor{ true };
    std::string last_key;

    int callback(ImGuiInputTextCallbackData* data);
};
