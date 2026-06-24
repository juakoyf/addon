#include "PropManager.hpp"
#include <cctype>
#include <cstring>

#ifdef GetProp
#undef GetProp
#endif

namespace Engine
{
    PropManager g_PropManager;

    PropManager::~PropManager()
    {
        Destroy();
    }

    bool PropManager::Create(IBaseClientDLL* client)
    {
        if (!client) {
            return false;
        }

        auto cc = client->GetAllClasses();
        if (!cc) {
            return false;
        }

        for (auto cur = cc; cur; cur = cur->m_pNext) {
            if (cur->m_pRecvTable)
                m_tables.emplace_back(cur->m_pRecvTable);

            m_client_ids[cur->m_pNetworkName] = cur->m_ClassID;
        }

        if (m_tables.empty()) {
            return false;
        }

        for (auto cur = cc; cur; cur = cur->m_pNext) {
            if (cur->m_pRecvTable)
                m_database.emplace_back(LoadTable(cur->m_pRecvTable));
        }

        return true;
    }

    void PropManager::Destroy()
    {
        m_tables.clear();
        m_database.clear();
        m_client_ids.clear();
    }

    PropManager::netvar_table PropManager::LoadTable(RecvTable* table)
    {
        netvar_table out{};
        out.name = table->m_pNetTableName;
        out.offset = 0;

        for (int i = 0; i < table->m_nProps; ++i) {
            auto* prop = &table->m_pProps[i];
            if (!prop || !prop->m_pVarName)
                continue;

            if (!std::strcmp(prop->m_pVarName, "baseclass"))
                continue;

            const bool starts_with_digit = std::isdigit(prop->m_pVarName[0]);
            if (starts_with_digit && prop->m_RecvType != DPT_DataTable)
                continue;

            if (prop->m_RecvType == DPT_DataTable && prop->m_pDataTable) {
                auto child = LoadTable(prop->m_pDataTable);
                child.offset = prop->m_Offset;
                child.prop = prop;
                out.child_tables.emplace_back(std::move(child));
            }
            else {
                out.child_props.emplace_back(prop);
            }
        }

        return out;
    }

    int PropManager::GetClientID(const std::string& network_name)
    {
        auto it = m_client_ids.find(network_name);
        return it != m_client_ids.end() ? it->second : -1;
    }

    int PropManager::GetOffset(const std::string& table, const std::string& prop)
    {
        return GetProp(table, prop);
    }

    RecvVarProxyFn PropManager::Hook(RecvVarProxyFn hooked,
        const std::string& table,
        const std::string& prop)
    {
        RecvProp* out = nullptr;
        if (!GetProp(table, prop, &out) || !out)
            return nullptr;

        auto original = out->m_ProxyFn;
        out->m_ProxyFn = hooked;
        return original;
    }

    int PropManager::GetProp(const std::string& table,
        const std::string& prop,
        RecvProp** out)
    {
        auto* tbl = GetTable(table);
        if (!tbl)
            return 0;

        return GetProp(tbl, prop, out);
    }

    int PropManager::GetProp(RecvTable* table,
        const std::string& prop,
        RecvProp** out)
    {
        int extra = 0;

        for (int i = 0; i < table->m_nProps; ++i) {
            auto* p = &table->m_pProps[i];
            if (!p)
                continue;

            if (p->m_pDataTable && p->m_pDataTable->m_nProps) {
                int add = GetProp(p->m_pDataTable, prop, out);
                if (add)
                    extra += p->m_Offset + add;
            }

            if (!std::strcmp(p->m_pVarName, prop.c_str())) {
                if (out)
                    *out = p;
                return p->m_Offset + extra;
            }
        }

        return extra;
    }

    RecvTable* PropManager::GetTable(const std::string& name)
    {
        for (auto* tbl : m_tables) {
            if (!std::strcmp(tbl->m_pNetTableName, name.c_str()))
                return tbl;
        }
        return nullptr;
    }
}
