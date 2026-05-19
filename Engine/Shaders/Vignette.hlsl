Texture2D    sceneTex : register(t0);
SamplerState sampler0 : register(s0);

struct PS_IN { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = sceneTex.Sample(sampler0, input.uv);
    float2 uv = input.uv * 2.0 - 1.0;
    float vignette = 1.0 - pow(length(uv), 1.5) * 0.5;
    col.rgb *= max(0.0, vignette);
    return col;
}
