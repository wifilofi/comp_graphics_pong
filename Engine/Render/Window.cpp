#include "Window.h"

#include "../Game/Game.h"

using namespace Engine::Render;

void Window::Compose(const char16* pName, const Point& size)
{
    pName_ = pName;
    size_ = size;
    pHandlerInstance_ = GetModuleHandle(nullptr);
    ComposeWindowClass();
    ComposeHandlerWindow();
    Show();
}

void Window::Show() const
{
    ShowWindow(pHandlerWindow_, SW_SHOW);
    SetForegroundWindow(pHandlerWindow_);
    SetFocus(pHandlerWindow_);
    ShowCursor(true);
}

void Window::Hide() const
{
    ShowWindow(pHandlerWindow_, SW_HIDE);
}

void Window::Destroy() const
{
    DestroyWindow(pHandlerWindow_);
    UnregisterClass(pName_, pHandlerInstance_);
}

void Window::ComposeWindowClass()
{
    windowClassEx_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClassEx_.lpfnWndProc = WindowProcedure;
    windowClassEx_.cbClsExtra = 0;
    windowClassEx_.cbWndExtra = 0;
    windowClassEx_.hInstance = pHandlerInstance_;
    windowClassEx_.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    windowClassEx_.hIconSm = windowClassEx_.hIcon;
    windowClassEx_.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClassEx_.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    windowClassEx_.lpszMenuName = nullptr;
    windowClassEx_.lpszClassName = pName_;
    windowClassEx_.cbSize = sizeof(WindowClassEx);
    RegisterClassEx(&windowClassEx_);
}

void Window::ComposeHandlerWindow()
{
    Rect windowRectangle = {0, 0, size_.x, size_.y};
    AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
    constexpr auto drawingStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;
    const auto positionX = (GetSystemMetrics(SM_CXSCREEN) - size_.x) / 2;
    const auto positionY = (GetSystemMetrics(SM_CYSCREEN) - size_.y) / 2;
    pHandlerWindow_ = CreateWindowEx(
        WS_EX_APPWINDOW, pName_, pName_, drawingStyle, positionX, positionY,
        windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top,
        nullptr, nullptr, pHandlerInstance_, nullptr);
}

PInt64 Window::WindowProcedure(PHandlerWindow pHandlerWindow, uint32 message, PUint64 pUintParam, PInt64 pIntParam)
{
    if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    if (message == WM_SIZE)
    {
        const int w = LOWORD(pIntParam);
        const int h = HIWORD(pIntParam);
        if (w > 0 && h > 0)
            Game::Instance()->GetRenderPipeline()->Resize(w, h);
        return 0;
    }
    return Game::Instance()->GetInputDevice()->HandleInput(pHandlerWindow, message, pUintParam, pIntParam);
}
