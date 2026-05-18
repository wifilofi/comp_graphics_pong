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

static const int kMaxLights = 16;

cbuffer LightBuffer : register(b3)
{
    float3 cameraPos;
    int    numLights;
    float  ambientStrength;
    float  specularStrength;
    float  shininess;
    float  _pad0;
    float4 lightPos[kMaxLights];
    float4 lightColor[kMaxLights];
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

    float3 norm    = normalize(input.normal);
    float3 viewDir = normalize(cameraPos - input.worldPos);

    float3 lighting = ambientStrength.xxx;

    for (int i = 0; i < numLights; ++i)
    {
        float3 ldir  = lightPos[i].xyz - input.worldPos;
        float  dist  = length(ldir);
        ldir        /= dist;
        float  atten = lightPos[i].w > 0.5
                     ? 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist)
                     : 1.0;

        float  diff    = max(dot(norm, ldir), 0.0);
        float3 halfway = normalize(ldir + viewDir);
        float  spec    = pow(max(dot(norm, halfway), 0.0), shininess);

        lighting += (diff + specularStrength * spec) * lightColor[i].xyz * atten;
    }

    return float4(lighting * texColor.rgb, texColor.a);
}
