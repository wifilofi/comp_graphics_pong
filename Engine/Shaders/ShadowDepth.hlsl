struct ObjectData
{
    float4x4 model;
    float4   color;
    float4   color2;
};

StructuredBuffer<ObjectData> instances : register(t0);

cbuffer ShadowBuffer : register(b4)
{
    float4x4 lightViewProj;
};

struct VS_IN
{
    float3 pos : POSITION0;
};

float4 VSMain(VS_IN input, uint instanceID : SV_InstanceID) : SV_Position
{
    ObjectData obj  = instances[instanceID];
    float4 worldPos = mul(float4(input.pos, 1.0), obj.model);
    return mul(worldPos, lightViewProj);
}
