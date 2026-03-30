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
    return float4(input.uv, 0, 1);
}