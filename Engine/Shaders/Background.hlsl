#include "Common.hlsli"

struct Addition
{
    float4 offset;
    float4 color;
};

cbuffer AdditionBuffer : register(b0)
{
    Addition AdditionData;
}

PS_IN VSMain( VS_IN input )
{
    PS_IN output;

    output.pos = float4(input.pos.xyz + AdditionData.offset.xyz, 1);
    output.uv  = input.col.xy;

    return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
    return float4(input.uv, time, 1);
}
