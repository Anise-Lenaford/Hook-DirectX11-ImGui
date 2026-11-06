#include "C_Window.h"
#include <Windows.h>
#include "imgui_impl_win32.h"

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

namespace Window
{
    static WNDPROC o_WndProc = nullptr;
    static LRESULT WINAPI WndProc_Func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return TRUE;

        switch (uMsg)
        {
        case WM_SIZE:
        {

        }
        break;
        case WM_KEYDOWN:
        {

        }
        break;
        }

        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse)
        {
            switch (uMsg)
            {
                //case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
                return TRUE;
            }
        }

        return CallWindowProcA(o_WndProc, hWnd, uMsg, wParam, lParam);

    }


}


void C_Window::SetWindow(void* hWnd)
{
    if (Window::o_WndProc == nullptr)
    Window::o_WndProc = (WNDPROC)SetWindowLongPtrA(static_cast<HWND>(hWnd), GWLP_WNDPROC, (LONG_PTR)Window::WndProc_Func);
}
