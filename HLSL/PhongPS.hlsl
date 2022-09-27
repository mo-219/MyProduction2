#include "Phong.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

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
