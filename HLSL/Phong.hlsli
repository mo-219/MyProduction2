#include "Light.hlsli"
#include "ShadowmapFunctions.hlsli"
#include "ShadingFunctions.hlsli"

struct VS_OUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;
	float3 normal	: NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
    
    float3 shadowTexcoord : TEXCOORD1; // シャドウマップ用のパラメータ変数
};

cbuffer CbScene : register(b0)
{
	float4					viewPosition;
	row_major float4x4		viewProjection;
	float4					ambientLightColor;
	DirectionalLightData	directionalLightData;
    row_major float4x4      world;

};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
	float4				materialColor;
};

cbuffer SHADOWMAP_CONSTANT_BUFFER : register(b3)
{
    row_major float4x4 lightViewProjection;
    float3 shadowColor;
    float shadowBias;
};


cbuffer FOG_CONSTANT_BUFFER : register(b4)
{
    float4 fogColor;
    float4 fogRange;
    
};


