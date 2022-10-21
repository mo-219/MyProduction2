#pragma once


#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class PhongShader : public ModelShader
{
public:
	PhongShader(ID3D11Device* device);
	~PhongShader() override {}

	void Begin(const RenderContext& rc) override;
	void Draw(const RenderContext& rc, const Model* model) override;
	void End(const RenderContext& rc) override;

private:
	static const int MaxBones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4		viewPosition;
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		ambientLightColor;
		DirectionalLightData	directionalLightData;
		DirectX::XMFLOAT4X4		world;					// ワールド行列
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	materialColor;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer;


	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;


	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;


	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;

	//----------------------------------------------------------
	// 
	//		コンスタントバッファ用
	// 
	//----------------------------------------------------------

	// シャドウマップ用
	struct CbShadowMap
	{
		DirectX::XMFLOAT4X4 lightViewProjection;	// ライトの位置から見た射影行列
		DirectX::XMFLOAT3 shadowColor;				// 影色
		float shadowBias;							// 深度バイアス
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>				shadowmapConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			shadowmapSamplerState;					//
	//Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		shadowmapDepthStencilView;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shadowmapShaderResourceView;	//
	//Microsoft::WRL::ComPtr<ID3D11VertexShader>			shadowmapCasterVertexShader;			//
	//Microsoft::WRL::ComPtr<ID3D11InputLayout>			shadowmapCasterInputLayout;			//

	DirectX::XMFLOAT4X4 lightViewProjection;
	float shadow_bias{ 0.008f };
	DirectX::XMFLOAT3 shadow_color{ 0.3f, 0.3f, 0.3f };



};
