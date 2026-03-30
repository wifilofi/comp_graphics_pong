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
    float timeSlowed = time * 0.05 + 100;
    float2 uv = input.uv * resolution - resolution / 2.0;
    uv = 2.0 * uv / resolution.y;

    uv = uv / (1.0 + length(uv));

    float pixel;
    uv.x = uv.x + (0.1 * sin(uv.y * 10.0)) * sin(timeSlowed);
    pixel = sign(fmod(uv.y + timeSlowed *  0.04, 0.2) - 0.1)
              * sign(fmod(uv.x + timeSlowed *  0.12, 0.2) - 0.1);

    float4 col = float4(0.027, 0.016, 0.161, 1);

    if (pixel == 1.0)
    {
        col = float4(0.094, 0.086, 0.2, 1);
    }

    return col;
}
