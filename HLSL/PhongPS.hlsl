#include "Phong.hlsli"

SamplerState diffuseMapSamplerState : register(s0);
SamplerState shadowMapSamplerState : register(s1);

Texture2D diffuseMap : register(t0);
Texture2D normal_map : register(t1);
Texture2D shadowMap : register(t2);


float4 main(VS_OUT pin) : SV_TARGET
{
	float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;

	float3 N = normalize(pin.normal);
	float3 L = normalize(directionalLightData.direction.xyz);
	float3 E = normalize(viewPosition.xyz - pin.worldPosition.xyz);

	// マテリアル定数
	float3 ka = float3(1, 1, 1); 
	float3 kd = float3(1, 1, 1);
	float3 ks = float3(1, 1, 1);
	float shiness = 128;

	// 環境光の計算
	float3 ambient = ka * ambientLightColor.xyz;

	// 平行光源のライティング計算
    float3 directionalDiffuse = CalcLambert(N, L, directionalLightData.color.rgb, kd);
    float3 directionalSpecular = CalcPhongSpeculer(N, L, -E, directionalLightData.color.rgb, kd.rgb);

	// 平行光源の影なので、平行光源に対して影を適応
    float3 shadow = CalcShadowColor(shadowMap, shadowMapSamplerState, pin.shadowTexcoord, shadowColor, shadowBias);

    directionalDiffuse *= shadow;
    directionalSpecular *= shadow;
	
	float4 color = float4(ambient, diffuseColor.a);
	color.rgb += diffuseColor.rgb * directionalDiffuse;
	color.rgb += directionalSpecular;


	
	return color;
}

#if false
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;

    float3 N = normalize(pin.normal);
    float3 L = normalize(directionalLightData.direction.xyz);
    float3 E = normalize(viewPosition.xyz - pin.world_position.xyz);

	// マテリアル定数
    float3 ka = float3(1, 1, 1);
    float3 kd = float3(1, 1, 1);
    float3 ks = float3(1, 1, 1);
    float shiness = 128;

	// 環境光の計算
    float3 ambient = ka * ambientLightColor;

	// 平行光源のライティング計算
    float3 directionalDiffuse = CalcLambertDiffuse(N, L, directionalLightData.color.rgb, kd);
    float3 directionalSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, E, shiness, ks);

    float4 color = float4(ambient, diffuseColor.a);
    color.rgb += diffuseColor.rgb * directionalDiffuse;
    color.rgb += directionalSpecular;

    return color;
}
#endif
