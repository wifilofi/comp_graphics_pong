struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0; // Assuming UVs are passed here as (u, v, 0, 0)
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

    // Apply offset from constant buffer
    output.pos = float4(input.pos.xyz + AdditionData.offset.xyz, 1);
    // Pass through the UV coordinates
    output.uv  = input.col.xy;

    return output;
}

// Simple Rounded Rectangle SDF function
// p: current position (centered at 0,0)
// b: half-extents (width/2, height/2)
// r: radius of the corners
float sdRoundedBox(float2 p, float2 b, float r)
{
    float2 q = abs(p) - b + r;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

float4 PSMain( PS_IN input ) : SV_Target
{
    // 1. Remap UV from [0, 1] to [-1, 1] to center the coordinate system
    float2 p = input.uv * 2.0 - 1.0;

    // 2. Define dimensions
    // size.x/y should be slightly less than 1.0 to stay within the quad
    float2 size = float2(0.8, 0.8);
    float radius = 0.8;

    // 3. Calculate distance
    float dist = sdRoundedBox(p, size, radius);

    // 4. Smoothstep for basic anti-aliasing
    // fwidth(dist) helps keep the edge sharpness consistent regardless of zoom/resolution
    float edgeSmoothing = fwidth(dist);
    float mask = smoothstep(edgeSmoothing, -edgeSmoothing, dist);

    // 5. Apply the color from our constant buffer
    float4 finalColor = float4(1, 1, 1, 1) * mask;
    //finalColor.a 1;*= mask;

    return finalColor;
}