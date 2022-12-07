#include "Light.hlsli"
#include "ShadowmapFunctions.hlsli"
#include "ShadingFunctions.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 shadowTexcoord : TEXCOORD1; // シャドウマップ用のパラメータ変数
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

cbuffer CbSubset : register(b2)
{
    float4 materialColor;
};

cbuffer CbShadowmap : register(b3)
{
    row_major float4x4 lightViewProjection;
    float3 shadowColor;
    float shadowBias;
};


cbuffer CbFog : register(b4)
{
    float4 fogColor;
    float4 fogRange;
    
};


cbuffer CbCubicColor : register(b5)
{
    float4 rightVec;
    float4 topVec;
    float4 frontVec;
    
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

#define KERNEL_MAX 16
cbuffer CBFilter : register(b6)
{
    float4 weights[KERNEL_MAX * KERNEL_MAX];
    float kernelSize;
    float2 texcel;
    float dummy;
};

cbuffer CbMask : register(b7)
{
    float dissolveThreshold; // ディゾルブ量
    float edgeThreshold; // 縁の閾値
    float maskFlag;
    float dummy2;

    float4 edgeColor; // 縁の色
};