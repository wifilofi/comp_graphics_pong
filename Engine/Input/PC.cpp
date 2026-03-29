#include "PC.h"

using namespace Engine::Input;

void PC::Compose(PHandlerWindow pHandlerWindow)
{
    Device::Compose(pHandlerWindow);

    RAWINPUTDEVICE devices[2];

    devices[0].usUsagePage = 1; // Keyboard
    devices[0].usUsage = 2;
    devices[0].dwFlags = 0;
    devices[0].hwndTarget = pHandlerWindow;

    devices[1].usUsagePage = 1; // Mouse
    devices[1].usUsage = 6;
    devices[1].dwFlags = 0;
    devices[1].hwndTarget = pHandlerWindow;

    RegisterRawInputDevices(devices, 2, sizeof(devices[0]));
}

PInt64 CALLBACK PC::HandleInput(
    PHandlerWindow pHandlerWindow, uint32 message, PUint64 pUintParam, PInt64 pIntParam)
{
    if (message != WM_INPUT) return DefWindowProc(pHandlerWindow, message, pUintParam, pIntParam);

    uint32 size;
    auto* pRawInput = ReadRawInput(&size, pIntParam);
    if (size == 0) return DefWindowProc(pHandlerWindow, message, pUintParam, pIntParam);

    if (pRawInput->header.dwType == RIM_TYPEKEYBOARD) ProcessKeyboard(pRawInput);
    else if (pRawInput->header.dwType == RIM_TYPEMOUSE) ProcessMouse(pRawInput);

    return DefWindowProc(pHandlerWindow, message, pUintParam, pIntParam);
}

void PC::Destroy()
{
    RAWINPUTDEVICE devices[2];

    devices[0].usUsagePage = 1; // Keyboard
    devices[0].usUsage = 2;
    devices[0].dwFlags = RIDEV_REMOVE;
    devices[0].hwndTarget = nullptr;

    devices[1].usUsagePage = 1; // Mouse
    devices[1].usUsage = 6;
    devices[1].dwFlags = RIDEV_REMOVE;
    devices[1].hwndTarget = nullptr;

    RegisterRawInputDevices(devices, 2, sizeof(devices[0]));
}

RAWINPUT* PC::ReadRawInput(uint32* pSize, PInt64 pIntParam)
{
    GetRawInputData(reinterpret_cast<PHandlerRawInput>(pIntParam), RID_INPUT,
                    inputBuffer_, pSize, sizeof(RAWINPUTHEADER));
    return reinterpret_cast<RawInput*>(inputBuffer_);
}

void PC::ProcessKeyboard(const RawInput* pRawInput)
{
    auto key = static_cast<Keyboard::Keys>(pRawInput->data.keyboard.VKey);

    if (pRawInput->data.keyboard.MakeCode == 42) key = Keyboard::Keys::LeftShift;
    else if (pRawInput->data.keyboard.MakeCode == 54) key = Keyboard::Keys::RightShift;

    if (pRawInput->data.keyboard.Flags & RI_KEY_BREAK)
    {
        if (pKeys_.count(key) != 0) RemovePressedKey(key);
    }
    else if (pKeys_.count(key) == 0) AddPressedKey(key);
}

void PC::ProcessMouse(const RawInput* pRawInput)
{
    const auto flags = pRawInput->data.mouse.usButtonFlags;
    if (flags & static_cast<uint16>(Mouse::Buttons::LeftButtonDown)) AddPressedKey(Keyboard::Keys::MouseLeftButton);
    if (flags & static_cast<uint16>(Mouse::Buttons::LeftButtonUp)) RemovePressedKey(Keyboard::Keys::MouseLeftButton);
    if (flags & static_cast<uint16>(Mouse::Buttons::RightButtonDown)) AddPressedKey(Keyboard::Keys::MouseRightButton);
    if (flags & static_cast<uint16>(Mouse::Buttons::RightButtonUp)) RemovePressedKey(Keyboard::Keys::MouseRightButton);
    if (flags & static_cast<uint16>(Mouse::Buttons::WheelButtonDown)) AddPressedKey(Keyboard::Keys::MouseWheelButton);
    if (flags & static_cast<uint16>(Mouse::Buttons::WheelButtonUp)) RemovePressedKey(Keyboard::Keys::MouseWheelButton);
    const Point translation = {pRawInput->data.mouse.lLastX, pRawInput->data.mouse.lLastY};
    UpdateMouseMovement(translation, pRawInput->data.mouse.usButtonData);
}
