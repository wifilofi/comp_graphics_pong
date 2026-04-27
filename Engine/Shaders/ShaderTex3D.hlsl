struct ObjectData
{
    float4x4 model;
    float4   color;
    float4   color2;
};

StructuredBuffer<ObjectData> instances : register(t0);
Texture2D    mainTex  : register(t1);
SamplerState sampler0 : register(s0);

cbuffer CameraBuffer : register(b2)
{
    float4x4 view;
    float4x4 projection;
};

struct VS_IN
{
    float3 pos    : POSITION0;
    float3 normal : NORMAL0;
    float2 uv     : TEXCOORD0;
};

struct PS_IN
{
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

PS_IN VSMain(VS_IN input, uint instanceID : SV_InstanceID)
{
    ObjectData obj  = instances[instanceID];
    PS_IN output;
    float4 worldPos = mul(float4(input.pos, 1.0), obj.model);
    float4 viewPos  = mul(worldPos, view);
    output.pos      = mul(viewPos, projection);
    output.uv       = input.uv;
    output.color    = obj.color;
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    return mainTex.Sample(sampler0, input.uv) * input.color;
}
