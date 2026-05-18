struct ObjectData
{
    float4x4 model;
    float4   color;
    float4   color2;
};

StructuredBuffer<ObjectData> instances : register(t0);

Texture2D              shadowMap     : register(t2);
SamplerComparisonState shadowSampler : register(s1);

cbuffer ShadowBuffer : register(b4)
{
    float4x4 lightViewProj;
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
};

struct PS_IN
{
    float4 pos      : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal   : NORMAL0;
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
    output.color    = obj.color;
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float3 norm    = normalize(input.normal);
    float3 viewDir = normalize(cameraPos - input.worldPos);

    float shadow = 1.0;
    {
        float4 lsPos = mul(float4(input.worldPos, 1.0), lightViewProj);
        float3 proj  = lsPos.xyz / lsPos.w;
        float2 uv    = float2(proj.x * 0.5 + 0.5, proj.y * -0.5 + 0.5);
        if (all(uv >= 0.0) && all(uv <= 1.0) && proj.z >= 0.0 && proj.z <= 1.0)
            shadow = shadowMap.SampleCmpLevelZero(shadowSampler, uv, proj.z);
    }

    float3 lighting = ambientStrength.xxx;

    for (int i = 0; i < numLights; ++i)
    {
        float3 ldir  = lightPos[i].xyz - input.worldPos;
        float  dist  = length(ldir);
        ldir        /= dist;
        float  atten = lightPos[i].w > 0.5
                     ? 1.0 / (1.0 + 0.005 * dist + 0.0005 * dist * dist)
                     : 1.0;

        float  diff    = max(dot(norm, ldir), 0.0);
        float3 halfway = normalize(ldir + viewDir);
        float  spec    = pow(max(dot(norm, halfway), 0.0), shininess);

        float lit = diff + specularStrength * spec;
        if (i == 0) lit *= shadow;
        lighting += lit * lightColor[i].xyz * atten;
    }

    return float4(lighting * input.color.rgb, input.color.a);
}
