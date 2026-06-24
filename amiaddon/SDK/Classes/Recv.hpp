#pragma once

#include <cstdint>

enum SendPropType {
    DPT_Float = 0,
    DPT_Vector,
    DPT_VectorXY,
    DPT_Int,
    DPT_String,
    DPT_Array,
    DPT_DataTable,
    DPT_Int64,
};


class DVariant {
public:
    union {
        float m_Float;
        int m_Int;
        const char* m_pString;
        void* m_pData;
        float m_Vector[3];
    };
    SendPropType m_Type;
};

class RecvProp;
class RecvTable;

using RecvVarProxyFn = void(*)(const class CRecvProxyData*, void*, void*);

class CRecvProxyData {
public:
    const RecvProp* m_pRecvProp;
private:
    std::uint8_t __pad[4];
public:
    DVariant m_Value;
    int m_iElement;
    int m_ObjectID;
};

class RecvProp {
public:
    const char* m_pVarName;
    SendPropType m_RecvType;
    int m_Flags;
    int m_StringBufferSize;
    bool m_bInsideArray;
    const void* m_pExtraData;
    RecvProp* m_pArrayProp;
    void* m_ArrayLengthProxy;
    RecvVarProxyFn m_ProxyFn;
    void* m_DataTableProxyFn;
    RecvTable* m_pDataTable;
    int m_Offset;
    int m_ElementStride;
    int m_nElements;
    const char* m_pParentArrayPropName;
};

class RecvTable {
public:
    RecvProp* m_pProps;
    int m_nProps;
    void* m_pDecoder;
    const char* m_pNetTableName;
    bool m_bInitialized;
    bool m_bInMainList;
};

class ClientClass {
public:
    void* m_pCreateFn;
    void* m_pCreateEventFn;
    char* m_pNetworkName;
    RecvTable* m_pRecvTable;
    ClientClass* m_pNext;
    int m_ClassID;
};