#include	"Light.hlsli"

struct VS_OUT
{
	float4 position : SV_POSITION;
	float3 world_position : POSITION;
	float3 normal	: NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
    
    float3 shadow_texcoord : TEXCOORD1; // シャドウマップ用のパラメータ変数
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


cbuffer HEMISPHERE_LIGHT_CONSTANT_BUFFER : register(b4)
{
    float4 sky_color;
    float4 ground_color;
    float4 hemisphere_weight;
};

cbuffer FOG_CONSTANT_BUFFER : register(b5)
{
    float4 fog_color;
    float4 fog_range;
    
};

cbuffer SHADOWMAP_CONSTANT_BUFFER : register(b6)
{
    row_major float4x4 light_view_projection;
    float3 shadow_color;
    float shadow_bias;
};

cbuffer CUBIC_COLOR_BUFFER : register(b7)
{
    float4 colorTop1;
    float4 colorBottom1;
    float4 colorRight1;
    float4 colorLeft1;
    float4 colorBack1;
    float4 colorFront1;

    float4 colorTop2;
    float4 colorBottom2;
    float4 colorRight2;
    float4 colorLeft2;
    float4 colorBack2;
    float4 colorFront2;
    
    float4 colorAlpha;
};


#include "shading_functions.hlsli"
