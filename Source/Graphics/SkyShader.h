#pragma once

#include "Graphics/Graphics.h"

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

class SkyShader : public ModelShader
{
public:
	SkyShader(ID3D11Device* device);
	~SkyShader() override {}

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

	struct CbSky
	{
		float skyValue;
		DirectX::XMFLOAT3 dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			skyConstantBuffer;

	float skyValue = 1.0f;


	// �X�J�C�摜
	std::unique_ptr<Texture>	skyTexture;	

};


