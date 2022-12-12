#include "Light.hlsli"
#include "ShadingFunctions.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;

    
};

cbuffer CbScene : register(b0)
{
    float4 viewPosition;
    row_major float4x4 viewProjection;
    float4 ambientLightColor;
    DirectionalLightData directionalLightData;
    row_major float4x4 world;

};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};

cbuffer CbSky : register(b2)
{
    float skyValue;
    float3 dummy;
};
