#include "color_filter.hlsli"
Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);



float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;
	
	//�@RGB >HSV�ɕϊ�
    color.rgb = RGB2HSV(color.rgb);
	
	// �F������
    color.r += hueShift;
	
	// �ʓx����
    color.g *= saturation;
	
	// ���x����
    color.b *= brightness;
	
	// HSV > RGB�ɕϊ�
    color.rgb = HSV2RGB(color.rgb);
	
    return color;

}