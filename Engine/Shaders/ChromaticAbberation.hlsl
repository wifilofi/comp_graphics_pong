Texture2D    sceneTex : register(t0);
SamplerState sampler0 : register(s0);

struct PS_IN { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

float4 PSMain(PS_IN input) : SV_Target
{
    float2 dir = input.uv - 0.5;
    float dist = length(dir);
    float2 offset = dir * 0.02;
    float r = sceneTex.Sample(sampler0, input.uv - offset).r;
    float g = sceneTex.Sample(sampler0, input.uv).g;
    float b = sceneTex.Sample(sampler0, input.uv + offset).b;
    return float4(r, g, b, 1.0);
}
