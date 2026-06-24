#include "../Hooks.hpp"
#include "../Renderer/Render.hpp"

HRESULT __stdcall Hooked::hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    Render::Engine::Invalidate();
    auto ret = oReset(pDevice, pPresentationParameters);

    if (ret == D3D_OK)
    {
        Render::Engine::Initialize();
    }

    return ret;
}
