#include "Mask.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

Texture2D maskTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color;

	// �}�X�N�摜����ԐF���擾
	float mask = maskTexture.Sample(sampler0, pin.texcoord).r;

	// step�֐���p����mask�̒l��dissolveThreshold�̒l���r���ē��ߒl��0 or 1�ɂ���
	float alpha = step(mask, dissolveThreshold);

	// color�̓��ߒl�ɏ�Z����
	color.a *= alpha; 

	// �A���t�@��0�ȉ��Ȃ炻�������`�悵�Ȃ��悤�ɂ���
	clip(color.a - 0.01f);

	return color;
}

