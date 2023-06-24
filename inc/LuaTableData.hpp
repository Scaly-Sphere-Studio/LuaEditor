#pragma once

#include "includes.hpp"

class LuaTableData : public std::map<std::string, LuaTableData> {
private:
    LuaTableData(sol::type type_, std::string name_) : type(type_), name(name_) {};
public:
    LuaTableData(sol::state& lua, sol::table table, sol::environment env,
        std::string name = "");

private:
    inline void _append(LuaTableData const& data) { insert(data.cbegin(), data.cend()); }
    void _appendUserdata(sol::state& lua, sol::environment env, sol::userdata u);

    template <class T>
    static void _appendBaseClass(sol::state& lua, LuaTableData& data,
        sol::environment env, sol::userdata const& u);
    using _AppendBaseClassFunc = std::function <
        void(sol::state&, LuaTableData&, sol::environment, sol::userdata const&) >;
    static std::vector<_AppendBaseClassFunc> _append_fns;

public:
    template <class T>
    static inline void addBaseClass() { _append_fns.emplace_back(_appendBaseClass<T>); }

    static bool recursive;

    sol::type type;
    std::string name;
    char separator{ '.' };

    std::string to_string() const;
    inline operator std::string() const { return to_string(); }
};
