#include "Common.hlsli"

struct Addition
{
    float4 offset;
    float4 color;
    float4 size;
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

float sdRoundedBox(float2 p, float2 b, float r)
{
    float2 q = abs(p) - b + r;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

float4 PSMain( PS_IN input ) : SV_Target
{
    float2 p = input.uv * 2.0 - 1.0;

    float aspect = AdditionData.size.x / AdditionData.size.y;
    float2 scaledP = p * float2(aspect, 1.0);
    float2 b = float2(aspect, 1.0) * 0.9;
    float radius = aspect * 0.9;

    float dist = sdRoundedBox(scaledP, b, radius * 1.3);

    float edgeSmoothing = fwidth(dist) / 2;
    float mask = smoothstep(edgeSmoothing, -edgeSmoothing, dist);

    float4 finalColor = AdditionData.color * mask;
    //finalColor.a *= mask;

    return finalColor;
}
