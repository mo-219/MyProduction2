// ���s�������
struct DirectionalLightData
{
	float4	direction;	// ����
	float4	color;		// �F
};

//--------------------------------------------
//	�����o�[�g�g�U���ˌv�Z�֐�
//--------------------------------------------
// normal		: �@��(���K���ς�)
// lightVector	: ���˃x�N�g��(���K���ς�)
// lightColor	: ���ˌ��F
// kd			: ���˗�(���˂̋���)
// �Ԃ��l		: �g�U���ːF
float3	CalcLambertDiffuse( float3 normal, float3 lightVector, float3 lightColor, float3 kd )
{
	// �@���ƃ��C�g�����ւ̃x�N�g���ł̓��� 
	// ���ς������ʁA���C�g���猩�ė������̖ʂ��ƃ}�C�i�X�̌��ʂ��o�Ă��܂�����
	// ��������̂܂܏�Z����ƐF�����������Ȃ�܂��B
	// 0�ȉ��ł����0�ɂȂ�悤�ɐ��������܂��傤�B
	float d = saturate(dot(normal, -lightVector));

	// ���ˌ��F�Ɠ��ς̌��ʁA�y�є��˗������ׂď�Z���ĕԋp���܂��傤�B
	return lightColor * d * kd;
}

//--------------------------------------------
//	�t�H���̋��ʔ��ˌv�Z�֐�
//--------------------------------------------
// normal		: �@��(���K���ς�)
// lightVector	: ���˃x�N�g��(���K���ς�)
// lightColor	: ���ˌ��F
// eyeVector		: �����x�N�g��(���K���ς�)
// shininess		: ����x(���������l)
// ks			: ���˗�(���˂̋���)
// �Ԃ��l		: ���ʔ��ːF
float3 CalcPhongSpecular(float3 normal, float3 lightVector, float3 lightColor, float3 eyeVector, float shininess, float3 ks)
{
	// ���˃x�N�g����@�������ɑ΂��Ĕ��˂��������˃x�N�g�����Z�o���܂��傤
	float3 R = reflect(lightVector, normal); 

	// ���˃x�N�g���Ǝ����x�N�g���Ƃœ���
	// ���ς������ʂ�0�ȉ��ł����0�ɂȂ�悤�ɐ��������܂��傤�B
	float d = max( dot(eyeVector, R), 0);

	// ���򏈗�
	// ���˂͕\�ʂ��c���c���Ȃقǃn�C���C�g�͋����Ȃ�A�͈͂��������Ȃ�܂��B
	// �t�ɕ\�ʂ��U���U���ȂقǁA�n�C���C�g�͎キ�Ȃ�A�͈͍͂L����܂��B
	// ���̕ω��͎w���I�ɕω����邽�ߌ���x���搔�Ƃ��Ďg�p���܂��B
	d = pow(d, shininess);

	// ���ˌ��F�Ɠ��ς̌��ʁA�y�є��˗������ׂď�Z���ĕԋp���܂��傤�B
	return	lightColor * d * ks;
}
