#pragma once

#include "Device.h"

namespace Engine::Input
{
    class PC final : public Device
    {
    public:
        void Compose(PHandlerWindow pHandlerWindow) override;

        PInt64 CALLBACK HandleInput(
            PHandlerWindow pHandlerWindow, uint32 message, PUint64 pUintParam, PInt64 pIntParam) override;

        void Destroy() override;

    private:
        RawInput *ReadRawInput(uint32 *pSize, PInt64 pIntParam);

        void ProcessKeyboard(const RawInput *pRawInput);

        void ProcessMouse(const RawInput *pRawInput);

        uint32 inputBuffer_[32]{};
    };
}
