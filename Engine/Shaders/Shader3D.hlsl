cbuffer ObjectBuffer : register(b0)
{
    float4x4 model;
    float4   color;
};

cbuffer FrameBuffer : register(b1)
{
    float  time;
    float  _pad;
    float2 resolution;
};

cbuffer CameraBuffer : register(b2)
{
    float4x4 view;
    float4x4 projection;
};

struct VS_IN
{
    float3 pos    : POSITION0;
    float3 normal : NORMAL0;
};

struct PS_IN
{
    float4 pos    : SV_POSITION;
    float3 normal : NORMAL0;
    float4 color  : COLOR0;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output;
    float4 worldPos  = mul(float4(input.pos, 1.0), model);
    float4 viewPos   = mul(worldPos, view);
    output.pos       = mul(viewPos, projection);
    output.normal    = normalize(mul(float4(input.normal, 0.0), model).xyz);
    output.color     = color;
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    return float4(input.color.rgb, input.color.a);
}
