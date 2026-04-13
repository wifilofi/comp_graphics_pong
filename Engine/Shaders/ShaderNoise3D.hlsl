// ShaderNoise3D.hlsl
// Surface color is determined by fractal Brownian motion (value noise, 5 octaves)
// sampled at the local vertex position, then lerped between 'color' and 'color2'.

cbuffer ObjectBuffer : register(b0)
{
    float4x4 model;
    float4   color;
    float4   color2;
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
    float4 pos      : SV_POSITION;
    float3 normal   : NORMAL0;
    float3 localPos : TEXCOORD0;  // local vertex position → noise coordinate
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output;
    float4 worldPos  = mul(float4(input.pos, 1.0), model);
    float4 viewPos   = mul(worldPos, view);
    output.pos       = mul(viewPos, projection);
    output.normal    = normalize(mul(float4(input.normal, 0.0), model).xyz);
    output.localPos  = input.pos;
    return output;
}

// ---- Noise ---------------------------------------------------------------

// Hash: maps integer grid corners to pseudo-random floats in [0, 1].
float hash(float3 p)
{
    p = frac(p * float3(443.8975f, 397.2973f, 491.1871f));
    p += dot(p, p.yzx + 19.19f);
    return frac((p.x + p.y) * p.z);
}

// Smooth value noise (trilinear interpolation with smoothstep).
float noise(float3 p)
{
    float3 i = floor(p);
    float3 f = frac(p);
    float3 u = f * f * (3.0f - 2.0f * f);   // smoothstep

    return lerp(
        lerp(lerp(hash(i + float3(0,0,0)), hash(i + float3(1,0,0)), u.x),
             lerp(hash(i + float3(0,1,0)), hash(i + float3(1,1,0)), u.x), u.y),
        lerp(lerp(hash(i + float3(0,0,1)), hash(i + float3(1,0,1)), u.x),
             lerp(hash(i + float3(0,1,1)), hash(i + float3(1,1,1)), u.x), u.y), u.z);
}

// Fractal Brownian Motion — 5 octaves, each halving amplitude and doubling frequency.
float fbm(float3 p)
{
    float v = 0.0f;
    float a = 0.5f;
    [unroll] for (int i = 0; i < 5; ++i)
    {
        v += a * noise(p);
        p  = p * 2.01f + float3(1.7f, 9.2f, 3.3f);
        a *= 0.5f;
    }
    return v;
}

// ---- Pixel shader --------------------------------------------------------

float4 PSMain(PS_IN input) : SV_Target
{
    // Scale and slowly animate the noise coordinate.
    float n = fbm(input.localPos * 3.0f + time * 0.06f);

    // Remap [0, 1] noise into a sharper contrast band then lerp colors.
    float t = saturate(n * 2.2f - 0.4f);
    return lerp(color, color2, t);
}
