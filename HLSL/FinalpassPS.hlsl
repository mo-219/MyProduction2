#include "Finalpass.hlsli"

Texture2D sceneTexture : register(t0);
SamplerState sceneSampler : register(s0);

// �u���[���e�N�X�`��
Texture2D bloomTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(sceneSampler, pin.texcoord) * pin.color;
	
	// �u���[���e�N�X�`�����Z
    color.rgb += bloomTexture.Sample(sceneSampler, pin.texcoord).rgb;
    return color;
}