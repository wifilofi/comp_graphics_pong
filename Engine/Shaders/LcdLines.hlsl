Texture2D    sceneTex : register(t0);
SamplerState sampler0 : register(s0);

struct PS_IN { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = sceneTex.Sample(sampler0, input.uv);
    float lineSpacing = 400.0;
    float lineThickness = 0.7;

    float pos = frac(input.uv.y * lineSpacing);
    float onLine = step(lineThickness, pos);
    float brightness = lerp(0.7, 1.0, onLine);

    col.rgb *= brightness;
    return col;
}
