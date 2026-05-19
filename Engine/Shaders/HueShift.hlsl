Texture2D    sceneTex : register(t0);
SamplerState sampler0 : register(s0);

struct PS_IN { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

cbuffer FrameBuffer : register(b1)
{
    float  time;
    float  _pad;
    float2 resolution;
}

float3 RgbToHsv(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));
    float  d = q.x - min(q.w, q.y);
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + 1e-10)), d / (q.x + 1e-10), q.x);
}

float3 HsvToRgb(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float4 PSMain(PS_IN input) : SV_Target
{
    float2 uv = input.uv;
    uv.x += sin(uv.y * 20.0 + time * 0.5) * 0.01;
    uv.y += sin(uv.x * 20.0 + time * 0.67) * 0.01;

    float4 col = sceneTex.Sample(sampler0, uv);
    float3 hsv = RgbToHsv(col.rgb);
    hsv.x  = frac(hsv.x + time * 0.01);
    col.rgb = HsvToRgb(hsv);
    return col;
}
