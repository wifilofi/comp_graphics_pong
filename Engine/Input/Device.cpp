#include "Device.h"

using namespace Engine::Input;

void Device::Compose(PHandlerWindow pHandlerWindow)
{
    pHandlerWindow_ = pHandlerWindow;
}

bool Device::IsKeyDown(const Keyboard::Keys key) const
{
    return pKeys_.count(key) != 0;
}

void Device::UpdateMouseMovement(const Point &translation, uint16 wheelDelta)
{
    Point position;
    GetCursorPos(&position);
    ScreenToClient(pHandlerWindow_, &position);
    const Mouse::Event event = {position, translation, wheelDelta};
    MouseEvent.Broadcast(event);
}

void Device::AddPressedKey(Keyboard::Keys key)
{
    pKeys_.insert(key);
    const Keyboard::Event event = {key, Keyboard::Buttons::Down};
    KeyboardEvent.Broadcast(event);
}

void Device::RemovePressedKey(Keyboard::Keys key)
{
    pKeys_.erase(key);
    const Keyboard::Event event = {key, Keyboard::Buttons::Up};
    KeyboardEvent.Broadcast(event);
}
