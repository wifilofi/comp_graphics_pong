#pragma once

#include <windows.h>
#include <WinUser.h>

#include "../Lib/Types.h"

namespace Engine
{
    namespace Render
    {
        class Window final
        {
        public:
            void Construct(const char16* pName, const Point& size);
            void Show() const;
            void Hide() const;
            void Destroy() const;

            PHandlerWindow GetHandlerWindow() const { return pHandlerWindow_; }

        private:
            void ConstructWindowClass();
            void ConstructHandlerWindow();

            static PInt64 CALLBACK WindowProcedure(
                PHandlerWindow pHandlerWindow, uint32 message, PUint64 pUintParam, PInt64 pIntParam);

            const char16* pName_ = nullptr;
            Point size_{};
            PHandlerInstance pHandlerInstance_ = nullptr;
            WindowClassEx windowClassEx_{};
            PHandlerWindow pHandlerWindow_ = nullptr;
        };
    }
}
