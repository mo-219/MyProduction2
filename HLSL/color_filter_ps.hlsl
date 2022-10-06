#include "color_filter.hlsli"
Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);



float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;
	
	//　RGB >HSVに変換
    color.rgb = RGB2HSV(color.rgb);
	
	// 色相調整
    color.r += hueShift;
	
	// 彩度調整
    color.g *= saturation;
	
	// 明度調整
    color.b *= brightness;
	
	// HSV > RGBに変換
    color.rgb = HSV2RGB(color.rgb);
	
    return color;

}