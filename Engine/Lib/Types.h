#pragma once

#include <cstdint>
#include <d3d11.h>
#include <windows.h>
#include <SimpleMath.h>
#include <DirectXCollision.h>

typedef char int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef unsigned char uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef WCHAR char16;

typedef UINT_PTR PUint64;
typedef LONG_PTR PInt64;

typedef DirectX::SimpleMath::Vector2 float2;
typedef DirectX::SimpleMath::Vector3 float3;
typedef DirectX::SimpleMath::Vector4 float4;

typedef POINT Point;
typedef RECT Rect;

typedef DirectX::SimpleMath::Rectangle DXRect;
typedef DirectX::BoundingBox DXBox;

typedef HINSTANCE PHandlerInstance;
typedef HWND PHandlerWindow;
typedef WNDCLASSEXW WindowClassEx;
typedef RAWINPUT RawInput;
typedef HRAWINPUT PHandlerRawInput;

typedef ID3D11Device DXDevice;
typedef ID3D11DeviceContext DXDeviceContext;
typedef IDXGISwapChain DXSwapChain;
typedef ID3D11RenderTargetView DXRenderTargetView;
typedef D3D11_VIEWPORT DXViewport;
typedef ID3DBlob DXBlob;
typedef D3D_SHADER_MACRO DXShaderMacros;
typedef ID3DInclude DXIncludes;
typedef ID3D11VertexShader DXVertexShader;
typedef ID3D11PixelShader DXPixelShader;
typedef ID3D11InputLayout DXInputLayout;
typedef ID3D11Buffer DXBuffer;
typedef ID3D11RasterizerState DXRasterizerState;
