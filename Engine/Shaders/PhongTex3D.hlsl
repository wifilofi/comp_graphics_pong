struct ObjectData
{
    float4x4 model;
    float4   color;
    float4   color2;
};

StructuredBuffer<ObjectData> instances : register(t0);
Texture2D    mainTex  : register(t1);
SamplerState sampler0 : register(s0);

cbuffer CameraBuffer : register(b2)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer LightBuffer : register(b3)
{
    float3 lightPos;
    float  _pad0;
    float3 lightColor;
    float  _pad1;
    float3 cameraPos;
    float  _pad2;
    float  ambientStrength;
    float  specularStrength;
    float  shininess;
    float  _pad3;
};

struct VS_IN
{
    float3 pos    : POSITION0;
    float3 normal : NORMAL0;
    float2 uv     : TEXCOORD0;
};

struct PS_IN
{
    float4 pos      : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal   : NORMAL0;
    float2 uv       : TEXCOORD1;
    float4 color    : COLOR0;
};

PS_IN VSMain(VS_IN input, uint instanceID : SV_InstanceID)
{
    ObjectData obj  = instances[instanceID];
    PS_IN output;
    float4 worldPos = mul(float4(input.pos, 1.0), obj.model);
    float4 viewPos  = mul(worldPos, view);
    output.pos      = mul(viewPos, projection);
    output.worldPos = worldPos.xyz;
    output.normal   = normalize(mul(float4(input.normal, 0.0), obj.model).xyz);
    output.uv       = input.uv * obj.color2.w;
    output.color    = obj.color;
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 texColor = mainTex.Sample(sampler0, input.uv) * input.color;

    float3 norm     = normalize(input.normal);
    float3 lightDir = normalize(lightPos - input.worldPos);
    float3 viewDir  = normalize(cameraPos - input.worldPos);

    float3 ambient  = ambientStrength * lightColor;

    float  diff     = max(dot(norm, lightDir), 0.0);
    float3 diffuse  = diff * lightColor;

    float3 halfway  = normalize(lightDir + viewDir);
    float  spec     = pow(max(dot(norm, halfway), 0.0), shininess);
    float3 specular = specularStrength * spec * lightColor;

    float3 result   = (ambient + diffuse + specular) * texColor.rgb;
    return float4(result, texColor.a);
}
