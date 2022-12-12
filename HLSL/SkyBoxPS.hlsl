#include "SkyBox.hlsli"

SamplerState sample0 : register(s0);
Texture2D tex0 : register(t0);
Texture2D skyMap : register(t1);


float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffeseColor = tex0.Sample(sample0, pin.texcoord);
    
    float3 E = normalize(pin.normal.xyz - viewPosition.xyz);
    float3 N = normalize(pin.normal.xyz);
    
    float4 color = diffeseColor;
    
    color.rgb = CalcSphereEnvironment(skyMap, sample0, color.rgb, N, E, skyValue);
	
    return color;
}