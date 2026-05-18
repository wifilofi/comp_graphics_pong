Texture2D    sceneTex : register(t0);
SamplerState sampler0 : register(s0);

struct PS_IN { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col    = sceneTex.Sample(sampler0, input.uv);
    float  levels = 8.0;
    col.rgb       = floor(col.rgb * levels) / levels;
    return col;
}
