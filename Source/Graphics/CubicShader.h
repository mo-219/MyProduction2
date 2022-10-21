#pragma once

#include "Graphics/Graphics.h"

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class CubicShader : public ModelShader
{
public:
	CubicShader(ID3D11Device* device);
	~CubicShader() override {}

	void Begin(const RenderContext& rc) override;
	void Draw(const RenderContext& rc, const Model* model) override;
	void End(const RenderContext& rc) override;

	void DebugGUI() override;

private:

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;

	//----------------------------------------------------------
	// 
	//		�R���X�^���g�o�b�t�@�p
	// 
	//----------------------------------------------------------
	static const int MaxBones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4		viewPosition;
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		ambientLightColor;
		DirectionalLightData	directionalLightData;
		DirectX::XMFLOAT4X4		world;					// ���[���h�s��
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;

	struct CbSubset
	{
		DirectX::XMFLOAT4	materialColor;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer;

	// �V���h�E�}�b�v�p
	struct CbShadowMap
	{
		DirectX::XMFLOAT4X4 lightViewProjection;	// ���C�g�̈ʒu���猩���ˉe�s��
		DirectX::XMFLOAT3 shadowColor;				// �e�F
		float shadowBias;							// �[�x�o�C�A�X
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>				shadowmapConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			shadowmapSamplerState;			

	// �t�H�O�p
	struct CbFog
	{
		DirectX::XMFLOAT4 fogColor;
		DirectX::XMFLOAT4 fogRange;	// X: near�A�@Y: far�A�@zw: ��
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> fogConstantBuffer;
	DirectX::XMFLOAT4 fogColor{ 0.1f, 0.1f, 0.2f, 1.0f };			// �N���A�J���[�ɂ��Ă��܂�
	DirectX::XMFLOAT4 fogRange{ 0.01f,100.0f,0,0 };					// x : near, y : far

	DirectX::XMFLOAT4X4 lightViewProjection;
	float shadow_bias{ 0.008f };
	DirectX::XMFLOAT3 shadow_color{ 0.3f, 0.3f, 0.3f };


	// �L���[�r�b�N�J���[�p
	struct CbCubic
	{
		DirectX::XMFLOAT4 rightVec;
		DirectX::XMFLOAT4 topVec;
		DirectX::XMFLOAT4 frontVec;

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

		DirectX::XMFLOAT4 colorAlpha;	// a:  �{���ɂɎg���A���t�@�l		rgb: dummy
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> cubicConstantBuffer;
	DirectX::XMFLOAT4 rightVec = { 1, 0, 0 ,0};
	DirectX::XMFLOAT4 topVec   = { 0, 1, 0 ,0};
	DirectX::XMFLOAT4 frontVec = { 0, 0, -1 ,0};

	DirectX::XMFLOAT4 colorTop1		= { 1.0f, 1.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 colorBottom1	= { 1.0f, 0.8f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 colorRight1	= { 1.0f, 0.0f, 0.8f, 1.0f };
	DirectX::XMFLOAT4 colorLeft1	= { 0.0f, 1.0f, 0.3f, 1.0f };
	DirectX::XMFLOAT4 colorBack1	= { 0.2f, 0.1f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 colorFront1	= { 1.0f, 0.0f, 0.0f, 1.0f };
									  		  	    	  
	DirectX::XMFLOAT4 colorTop2		= { 0.5f, 0.5f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 colorBottom2	= { 1.0f, 0.8f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 colorRight2	= { 1.0f, 0.0f, 0.8f, 1.0f };
	DirectX::XMFLOAT4 colorLeft2	= { 0.0f, 1.0f, 0.3f, 1.0f };
	DirectX::XMFLOAT4 colorBack2	= { 0.2f, 0.1f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 colorFront2	= { 1.0f, 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT4 colorAlpha	= { 1,1,1,1 };

};
