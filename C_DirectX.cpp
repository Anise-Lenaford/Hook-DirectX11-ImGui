#include "C_DirectX.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "kiero.h"
#include "C_Window.h"

namespace DirectX
{
    using Present_ = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT);
    static Present_ o_Present = nullptr;
    static HRESULT STDMETHODCALLTYPE Present_Func(IDXGISwapChain* this_, UINT SyncInterval, UINT Flags)
    {
        if (!C_DirectX::init)
        {
            if (SUCCEEDED(this_->GetDevice(IID_PPV_ARGS(&C_DirectX::iDevice))) && C_DirectX::iDevice)
            {
                ID3D11Texture2D* texture{};
                if (SUCCEEDED(this_->GetBuffer(0, IID_PPV_ARGS(&texture))) && texture)
                {
                    C_DirectX::iDevice->GetImmediateContext(&C_DirectX::iContext);
                    if (SUCCEEDED(C_DirectX::iDevice->CreateRenderTargetView(texture, nullptr, &C_DirectX::iView)))
                    {
                        DXGI_SWAP_CHAIN_DESC desc;
                        this_->GetDesc(&desc);

                        if (desc.OutputWindow)
                        {
                            if (!ImGui::GetCurrentContext()) ImGui::CreateContext();

                            ImGui_ImplWin32_Init(desc.OutputWindow);
                            if (ImGui_ImplDX11_Init(C_DirectX::iDevice, C_DirectX::iContext))
                            {
                                C_Window::SetWindow(desc.OutputWindow);

                                C_DirectX::init = true;
                            }

                        }
                    }

                    if (texture)
                    {
                        texture->Release();
                        texture = nullptr;
                    }
                }
            }

            if (!C_DirectX::init)
                return o_Present(this_, SyncInterval, Flags);

        }

        if (!C_DirectX::iContext || !C_DirectX::iView)
            return o_Present(this_, SyncInterval, Flags);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();



        ImGui::Render();

        C_DirectX::iContext->OMSetRenderTargets(1, &C_DirectX::iView, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        return o_Present(this_, SyncInterval, Flags);

    }


    using ResizeBuffers_ = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    static ResizeBuffers_ o_ResizeBuffers = nullptr;
    static HRESULT STDMETHODCALLTYPE ResizeBuffers_Func(IDXGISwapChain* this_, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
    {
        if (!C_DirectX::init || !C_DirectX::iContext)
            return o_ResizeBuffers(this_, BufferCount, Width, Height, NewFormat, SwapChainFlags);

        ImGui_ImplDX11_InvalidateDeviceObjects();

        C_DirectX::iContext->OMSetRenderTargets(0, nullptr, nullptr);

        if (C_DirectX::iView)
        {
            C_DirectX::iView->Release();
            C_DirectX::iView = nullptr;
        }

        auto result = o_ResizeBuffers(this_, BufferCount, Width, Height, NewFormat, SwapChainFlags);

        ID3D11Texture2D* texture{};
        if (FAILED(this_->GetBuffer(0, IID_PPV_ARGS(&texture))) || !texture) return result;

        auto hResultView = C_DirectX::iDevice->CreateRenderTargetView(texture, NULL, &C_DirectX::iView);
        texture->Release();

        if (FAILED(hResultView)) return result;

        C_DirectX::iContext->OMSetRenderTargets(1, &C_DirectX::iView, nullptr);

        D3D11_VIEWPORT viewPort =
        {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = (float)Width,
            .Height = (float)Height,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };

        C_DirectX::iContext->RSSetViewports(1, &viewPort);

        return result;

    }


}


void C_DirectX::Hook()
{
    //MH_Initialize();
    
    kiero::init(kiero::RenderType::D3D11);
    kiero::bind(8, reinterpret_cast<void**>(&DirectX::o_Present), DirectX::Present_Func);
    kiero::bind(13, reinterpret_cast<void**>(&DirectX::o_ResizeBuffers), DirectX::ResizeBuffers_Func);

}