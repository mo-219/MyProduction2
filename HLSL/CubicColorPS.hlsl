#include "CubicColor.hlsli"

Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);
Texture2D dissolveTexture : register(t3);

SamplerState colorSamplerState : register(s0);
SamplerState shadowMapSamplerState : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = colorMap.Sample(colorSamplerState, pin.texcoord);
   
    float3 L = normalize(directionalLightData.direction.xyz);
    float3 E = normalize(viewPosition.xyz - pin.worldPosition.xyz);
    float3 N = normalize(pin.normal);
    
    float3x3 mat = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
   
   
    
	// マテリアル定数
    float3 ka = float3(1, 1, 1);
    float3 kd = float3(1, 1, 1);
    float3 ks = float3(0.3f, 0.3f, 0.3f);
    float shiness = 128;

	// 環境光の計算
    float3 ambient = ka * ambientLightColor.xyz;

	// 平行光源のライティング計算
    float3 directionalDiffuse = CalcHalfLambert(N, L, directionalLightData.color.rgb, kd);
    directionalDiffuse = max(directionalDiffuse, float3(0.7f, 0.7f, 0.7f));
    float3 directionalSpecular = CalcPhongSpeculer(N, L, -E, directionalLightData.color.rgb, kd.rgb);

    float3 shadow = CalcShadowColor(shadowMap, shadowMapSamplerState, pin.shadowTexcoord, shadowColor, shadowBias);


    
    
    // CubicColor
    // グラデーション用(color1とcolor2でグラデーション)
    HexColor color1;
    color1.colorTop = colorTop1;
    color1.colorBottom = colorBottom1;
    color1.colorRight = colorRight1;
    color1.colorLeft = colorLeft1;
    color1.colorBack = colorBack1;
    color1.colorFront = colorFront1;
    
    HexColor color2;
    color2.colorTop = colorTop2;
    color2.colorBottom = colorBottom2;
    color2.colorRight = colorRight2;
    color2.colorLeft = colorLeft2;
    color2.colorBack = colorBack2;
    color2.colorFront = colorFront2;

    float4 color = float4(ambient, diffuse_color.a);

    color.rgb += diffuse_color.rgb * directionalDiffuse * shadow;
    color.rgb += directionalSpecular * shadow;
    
    //color *= float4(shadow,1.0f);

    color = CubicColor(color, N, color1, color2, colorAlpha.a, rightVec,topVec,frontVec);
    
    color.rgb += CalcHemiSphereLight(N, float3(0, 1, 0), float3(1, 1, 1), float3(0, 0, 0), hemisphereWeight);
    
    color = CalcFog(color, fogColor, fogRange.xy, length(pin.worldPosition.xyz - viewPosition.xyz));
    
    color.rgb += CalcRimLight(N, E, L, directionalLightData.color.rgb);
    
    float4 dissolveValue = CulcDissolve(dissolveTexture, colorSamplerState, pin.texcoord, edgeColor, dissolveThreshold, edgeThreshold, maskFlag);

    
    color.a *= dissolveValue.a;
    clip(color.a - 0.01f);
    

    
    return color;
}