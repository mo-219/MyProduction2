#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

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
	float					dissolveThreshold;
	float					edgeThreshold; 	// ����臒l
	DirectX::XMFLOAT4			edgeColor;		// ���̐F
};


// �K�E�X�t�B���^�[�v�Z���
struct GaussianFilterData
{
	int				  kernelSize = 8;		// �J�[�l���T�C�Y
	float			  deviation = 10.0f;	// �W���΍�
	DirectX::XMFLOAT2 textureSize;			// �ڂ����e�N�X�`���̃T�C�Y 
};
static const int MaxKernelSize = 16;


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
};

//// �����_�[�R���e�L�X�g
//struct RenderContext
//{
//	DirectX::XMFLOAT4X4		view;
//	DirectX::XMFLOAT4X4		projection;
//	DirectX::XMFLOAT4		lightDirection;
//};
