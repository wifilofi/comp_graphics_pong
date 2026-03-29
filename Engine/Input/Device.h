#pragma once

#include <unordered_set>

#include "../Lib/Types.h"
#include "../Lib/Delegates.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace Engine::Input
{
    class Device
    {
    public:
        virtual ~Device() = default;

        virtual void Compose(PHandlerWindow pHandlerWindow);

        virtual PInt64 CALLBACK HandleInput(
            PHandlerWindow pHandlerWindow, uint32 message, PUint64 pUintParam, PInt64 pIntParam) = 0;

        virtual void Destroy() = 0;

        bool IsKeyDown(Keyboard::Keys key) const;

        Lib::MulticastDelegate<const Mouse::Event &> MouseEvent;
        Lib::MulticastDelegate<const Keyboard::Event &> KeyboardEvent;

    protected:
        void UpdateMouseMovement(const Point &translation, uint16 wheelDelta);

        void AddPressedKey(Keyboard::Keys key);

        void RemovePressedKey(Keyboard::Keys key);

        std::unordered_set<Keyboard::Keys> pKeys_{};
        PHandlerWindow pHandlerWindow_ = nullptr;
    };
}
