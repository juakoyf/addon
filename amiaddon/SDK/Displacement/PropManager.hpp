#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "../Classes/IBaseClientDll.hpp"
#include "../Classes/Recv.hpp"


#ifdef GetProp
#undef GetProp
#endif

namespace Engine
{
    class PropManager
    {
    public:
        struct netvar_table {
            std::string                 name{};
            int                         offset{ 0 };
            RecvProp* prop{ nullptr };
            std::vector<netvar_table>  child_tables{};
            std::vector<RecvProp*>     child_props{};
        };

    public:
        PropManager() = default;
        ~PropManager();

        bool Create(IBaseClientDLL* client);
        void Destroy();

        int  GetOffset(const std::string& table, const std::string& prop);
        int  GetClientID(const std::string& network_name);

        RecvVarProxyFn Hook(RecvVarProxyFn hooked,
            const std::string& table,
            const std::string& prop);

    private:
        netvar_table LoadTable(RecvTable* table);

        int  GetProp(const std::string& table,
            const std::string& prop,
            RecvProp** out = nullptr);

        int  GetProp(RecvTable* table,
            const std::string& prop,
            RecvProp** out = nullptr);

        RecvTable* GetTable(const std::string& name);

    private:
        std::vector<RecvTable*>                     m_tables{};
        std::vector<netvar_table>                   m_database{};
        std::unordered_map<std::string, int>        m_client_ids{};
    };

    extern PropManager g_PropManager;
}
