#include "SkyBox.hlsli"


VS_OUT main(float4 position : POSITION,
	float3 normal : NORMAL,
	float2 texcoord : TEXCOORD, 
    float4 boneWeights : WEIGHTS,
    uint4 boneIndices : BONES)
{


    VS_OUT vout = (VS_OUT) 0;
    vout.position = position;
    vout.worldPosition = mul(position, mul(world, viewProjection)).xyww;
    vout.normal = normalize(mul(float4(normal.xyz, 0), world));
    vout.texcoord = texcoord;

    
    return vout;
}