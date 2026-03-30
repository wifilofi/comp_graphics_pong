struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

cbuffer FrameBuffer : register(b1)
{
    float  time;
    float  _pad;
    float2 resolution;
}
