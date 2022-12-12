#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#define CUBIC_DEFAULT 0
#define CUBIC_CUSTOMIZE  1

//	UV�X�N���[�����
struct UVScrollData
{
	DirectX::XMFLOAT2	uvScrollValue;	// UV�X�N���[���l
};

//	���s�������
struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// ����
	DirectX::XMFLOAT4	color;		// �F
};

// �}�X�N�f�[�^
struct MaskData
{
	ID3D11ShaderResourceView* maskTexture;
	float					  dissolveThreshold = 0.0f;
	float				      edgeThreshold = 0.2f;			// ����臒l
	DirectX::XMFLOAT4		  edgeColor = {1,0,0,1};		// ���̐F

	float					  maskFlag = 1.0f;
};

// �K�E�X�t�B���^�[�v�Z���
struct GaussianFilterData
{
	int				  kernelSize = 8;		// �J�[�l���T�C�Y
	float			  deviation = 10.0f;	// �W���΍�
	DirectX::XMFLOAT2 textureSize;			// �ڂ����e�N�X�`���̃T�C�Y 
};
static const int MaxKernelSize = 16;

// ���P�x�o�͗p���
struct LuminanceExtractionData
{
	float			  threshold = 0.95f;		// 臒l
	float			  intensity = 1.0f;		// �u���[���̋��x
	DirectX::XMFLOAT2 dummy2;

};

// �|�X�g�G�t�F�N�g�p�̍ŏI�p�X�p���
struct FinalpassData
{
	// �u���[���e�N�X�`��
	ID3D11ShaderResourceView* bloomTexture;
};


// �V���h�E�}�b�v�p���
struct ShadowMapData
{
	ID3D11ShaderResourceView*	shadowMap;				// �V���h�E�}�b�v�e�N�X�`��
	DirectX::XMFLOAT4X4			lightViewProjection;	// ���C�g�r���[�v���W�F�N�V�����s��
	DirectX::XMFLOAT3			shadowColor;			// �e�̐F
	float						shadowBias;				// �[�x��v�p�̃I�t�Z�b�g�l
};

// �L���[�r�b�N�J���[�p
struct CubicColorData
{
	int				  shaderFlag = CUBIC_DEFAULT;		// CUBIC_DEFAULT: default	CUBIC_CUSTOMIZE: �X�ŐF�ς�
	DirectX::XMFLOAT4 rightVec;							// �E�x�N�g��	w: dummy
	DirectX::XMFLOAT4 topVec;							// ��x�N�g��	w: dummy
	DirectX::XMFLOAT4 frontVec;							// �O�x�N�g��	w: dummy

	DirectX::XMFLOAT4 colorTop1;
	DirectX::XMFLOAT4 colorBottom1;
	DirectX::XMFLOAT4 colorRight1;
	DirectX::XMFLOAT4 colorLeft1;
	DirectX::XMFLOAT4 colorBack1;
	DirectX::XMFLOAT4 colorFront1;

	DirectX::XMFLOAT4 colorTop2;
	DirectX::XMFLOAT4 colorBottom2;
	DirectX::XMFLOAT4 colorRight2;
	DirectX::XMFLOAT4 colorLeft2;
	DirectX::XMFLOAT4 colorBack2;
	DirectX::XMFLOAT4 colorFront2;

	DirectX::XMFLOAT4 colorAlpha;	// a:�@�ق�ƂɎg���A���t�@�l		rgb: dummy
};


// �����_�[�R���e�L�X�g
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;

	//	�J�������
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;

	//	�X�N���[���f�[�^
	UVScrollData			uvScrollData;

	// �}�X�N�f�[�^
	MaskData				maskData;

	//	���C�g���
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;

	// �K�E�X�t�B���^�[���
	GaussianFilterData		gaussianFilterData;

	// ���P�x���o�p���
	LuminanceExtractionData luminanceExtractionData;

	// �ŏI�p�X���
	FinalpassData			finalpassData;

	// �V���h�E�}�b�v���
	ShadowMapData			shadowmapData;

	// �L���[�r�b�N�J���[���
	CubicColorData			cubicColorData;

	// �V���h�E�ڂ���
	GaussianFilterData		shadowBlurData;

	int BlurCount = 0;

	// heightFog���邩�ۂ�
	int heightFogFlag = 1;
};

