struct ObjectData
{
    float4x4 model;
    float4   color;
    float4   color2;
};

StructuredBuffer<ObjectData> instances : register(t0);

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
    float3 normalVS : NORMAL0;
    float4 color    : COLOR0;
};

PS_IN VSMain(VS_IN input, uint instanceID : SV_InstanceID)
{
    ObjectData obj = instances[instanceID];
    PS_IN output;
    float4 worldPos  = mul(float4(input.pos, 1.0), obj.model);
    float4 viewPos   = mul(worldPos, view);
    output.pos       = mul(viewPos, projection);
    output.normalVS  = normalize(mul(mul(float4(input.normal, 0.0), obj.model), view).xyz);
    output.color     = obj.color;
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float facing = abs(input.normalVS.z);          // 1 = center, 0 = edge
    float rim    = 1.0 - facing;

    float alpha  = smoothstep(0.0, 1.0, facing);   // fades to 0 at edges
    alpha        = pow(alpha, 0.5);                 // widen the soft core
    alpha       *= input.color.a;

    float bright = 1.0 + rim * 0.6;                // rim slightly brighter for halo feel
    float3 col   = input.color.rgb * bright;

    return float4(col, alpha);
}
