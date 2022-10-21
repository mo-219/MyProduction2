#include "Misc.h"
#include "Graphics/CubicShader.h"

#include <imgui.h>
CubicShader::CubicShader(ID3D11Device* device)
{
	// 頂点シェーダー
	{
		//// ファイルを開く
		//FILE* fp = nullptr;
		//fopen_s(&fp, "Shader\\PhongVS.cso", "rb");
		//_ASSERT_EXPR_A(fp, "CSO File not found");

		//// ファイルのサイズを求める
		//fseek(fp, 0, SEEK_END);
		//long csoSize = ftell(fp);
		//fseek(fp, 0, SEEK_SET);

		//// メモリ上に頂点シェーダーデータを格納する領域を用意する
		//std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		//fread(csoData.get(), csoSize, 1, fp);
		//fclose(fp);

		//// 頂点シェーダー生成
		//HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		//hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 頂点シェーダー生成
		create_vs_from_cso(device, "Shader\\CubicColorVS.cso",
			vertexShader.GetAddressOf(),
			inputLayout.GetAddressOf(),
			inputElementDesc, ARRAYSIZE(inputElementDesc));

	}

	// ピクセルシェーダー生成
	create_ps_from_cso(device, "Shader\\CubicColorPS.cso", pixelShader.GetAddressOf());


	// 定数バッファ
	{
		// シーン用バッファ
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbScene);
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, sceneConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));


		// メッシュ用バッファ
		desc.ByteWidth = sizeof(CbMesh);
		hr = device->CreateBuffer(&desc, 0, meshConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));


		// サブセット用バッファ
		desc.ByteWidth = sizeof(CbSubset);
		hr = device->CreateBuffer(&desc, 0, subsetConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));


		// シャドウマップ用バッファ
		desc.ByteWidth = sizeof(CbShadowMap);
		hr = device->CreateBuffer(&desc, 0, shadowmapConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// フォグ用
		desc.ByteWidth = sizeof(CbFog);
		hr = device->CreateBuffer(&desc, 0, fogConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		desc.ByteWidth = sizeof(CbCubic);
		hr = device->CreateBuffer(&desc, 0, cubicConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));


	}

	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// サンプラステート
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;


		HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));


		// シャドウマップ用
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;	// テクスチャに指定されている影の範囲外に影を落とさないように、パラメータを指定できるようにするため
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.BorderColor[0] = FLT_MAX;
		desc.BorderColor[1] = FLT_MAX;
		desc.BorderColor[2] = FLT_MAX;
		desc.BorderColor[3] = FLT_MAX;

		hr = device->CreateSamplerState(&desc, shadowmapSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	}
}

// 描画開始
void CubicShader::Begin(const RenderContext& rc)
{
	rc.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	rc.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	rc.deviceContext->IASetInputLayout(inputLayout.Get());

	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		meshConstantBuffer.Get(),
		subsetConstantBuffer.Get(),
		shadowmapConstantBuffer.Get(),
		fogConstantBuffer.Get(),
		cubicConstantBuffer.Get(),
	};
	rc.deviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	rc.deviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	rc.deviceContext->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	rc.deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	rc.deviceContext->RSSetState(rasterizerState.Get());

	ID3D11SamplerState* samplerStates[]
	{
		samplerState.Get(),
		shadowmapSamplerState.Get()
	};
	rc.deviceContext->PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

	// シーン用定数バッファ更新
	CbScene cbScene;

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);
	cbScene.ambientLightColor = rc.ambientLightColor;
	cbScene.directionalLightData = rc.directionalLightData;
	cbScene.viewPosition = rc.viewPosition;

	rc.deviceContext->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);

	// シャドウマップ用定数バッファ更新
	CbShadowMap cbShadowmap;
	cbShadowmap.shadowColor = rc.shadowmapData.shadowColor;
	cbShadowmap.shadowBias = rc.shadowmapData.shadowBias;
	cbShadowmap.lightViewProjection = rc.shadowmapData.lightViewProjection;
	rc.deviceContext->UpdateSubresource(shadowmapConstantBuffer.Get(), 0, 0, &cbShadowmap, 0, 0);

	// シャドウマップ設定
	rc.deviceContext->PSSetShaderResources(2, 1, &rc.shadowmapData.shadowMap);
}

// 描画
void CubicShader::Draw(const RenderContext& rc, const Model* model)
{
	const ModelResource* resource = model->GetResource();
	const std::vector<Model::Node>& nodes = model->GetNodes();

	// フォグコンスタントバッファ更新
	CbFog cbFog;
	cbFog.fogColor = fogColor;
	cbFog.fogRange = fogRange;
	rc.deviceContext->UpdateSubresource(fogConstantBuffer.Get(), 0, 0, &cbFog, 0, 0);

	// キュービックカラーコンスタントバッファ更新
	CbCubic cbCubic;
	switch (rc.cubicColorData.shaderFlag)
	{
	case CUBIC_DEFAULT:
		// デフォルト設定の場合
		cbCubic.rightVec		= rightVec;
		cbCubic.topVec			= topVec;
		cbCubic.frontVec		= frontVec;

		cbCubic.colorTop1		= colorTop1;
		cbCubic.colorBottom1	= colorBottom1;
		cbCubic.colorRight1		= colorRight1;
		cbCubic.colorLeft1		= colorLeft1;
		cbCubic.colorBack1		= colorBack1;
		cbCubic.colorFront1		= colorFront1;

		cbCubic.colorTop2		= colorTop2;
		cbCubic.colorBottom2	= colorBottom2;
		cbCubic.colorRight2		= colorRight2;
		cbCubic.colorLeft2		= colorLeft2;
		cbCubic.colorBack2		= colorBack2;
		cbCubic.colorFront2		= colorFront2;

		cbCubic.colorAlpha = colorAlpha;
		break;

	case CUBIC_CUSTOMIZE:
		// カスタマイズしている場合
		cbCubic.rightVec		= rc.cubicColorData.rightVec;
		cbCubic.topVec			= rc.cubicColorData.topVec;
		cbCubic.frontVec		= rc.cubicColorData.frontVec;

		cbCubic.colorTop1		= rc.cubicColorData.colorTop1;
		cbCubic.colorBottom1	= rc.cubicColorData.colorBottom1;
		cbCubic.colorRight1		= rc.cubicColorData.colorRight1;
		cbCubic.colorLeft1		= rc.cubicColorData.colorLeft1;
		cbCubic.colorBack1		= rc.cubicColorData.colorBack1;
		cbCubic.colorFront1		= rc.cubicColorData.colorFront1;

		cbCubic.colorTop2		= rc.cubicColorData.colorTop2;
		cbCubic.colorBottom2	= rc.cubicColorData.colorBottom2;
		cbCubic.colorRight2		= rc.cubicColorData.colorRight2;
		cbCubic.colorLeft2		= rc.cubicColorData.colorLeft2;
		cbCubic.colorBack2		= rc.cubicColorData.colorBack2;
		cbCubic.colorFront2		= rc.cubicColorData.colorFront2;

		cbCubic.colorAlpha		= rc.cubicColorData.colorAlpha;
		break;
	};

	rc.deviceContext->UpdateSubresource(cubicConstantBuffer.Get(), 0, 0, &cbCubic, 0, 0);


	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// メッシュ用定数バッファ更新
		CbMesh cbMesh;
		::memset(&cbMesh, 0, sizeof(cbMesh));
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
				DirectX::XMMATRIX boneTransform = offsetTransform * worldTransform;
				DirectX::XMStoreFloat4x4(&cbMesh.boneTransforms[i], boneTransform);
			}
		}
		else
		{
			cbMesh.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
		}
		rc.deviceContext->UpdateSubresource(meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		rc.deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		rc.deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		rc.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			CbSubset cbSubset;
			cbSubset.materialColor = subset.material->color;
			rc.deviceContext->UpdateSubresource(subsetConstantBuffer.Get(), 0, 0, &cbSubset, 0, 0);
			rc.deviceContext->PSSetShaderResources(0, 1, subset.material->diffuse_map.GetAddressOf());
			rc.deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
			rc.deviceContext->DrawIndexed(subset.indexCount, subset.startIndex, 0);
		}
	}
}

// 描画終了
void CubicShader::End(const RenderContext& rc)
{
	rc.deviceContext->VSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->PSSetShader(nullptr, nullptr, 0);
	rc.deviceContext->IASetInputLayout(nullptr);

	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr };
	rc.deviceContext->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
}

void CubicShader::DebugGUI()
{


	if (ImGui::Begin("Cubic", nullptr, ImGuiWindowFlags_None))
	{

		//ImGui::Text("Color1");
		//{
		//	ImGui::ColorEdit3("colorTop", &colorTop1.x);
		//	ImGui::ColorEdit3("colorBottom", &colorBottom1.x);
		//	ImGui::ColorEdit3("colorRight", &colorRight1.x);
		//	ImGui::ColorEdit3("colorLeft", &colorLeft1.x);
		//	ImGui::ColorEdit3("colorBack", &colorBack1.x);
		//	ImGui::ColorEdit3("colorFront", &colorFront1.x);
		//	ImGui::SliderFloat("Alpha", &colorAlpha.w, 0.0f, +1.0f);
		//	//if (ImGui::TreeNode("ColorValue"))
		//	//{
		//	//	ImGui::SliderFloat3("colorTop", &colorTop1.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorBottom", &colorBottom1.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorRight", &colorRight1.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorLeft", &colorLeft1.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorBack", &colorBack1.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorFront", &colorFront1.x, 0, 1);
		//	//	ImGui::TreePop();
		//	//}
		//	//ImGui::TreePop();
		//}


		//ImGui::Text("Color2");
		//{
		//	ImGui::ColorEdit3("colorTop", &colorTop2.x);
		//	ImGui::ColorEdit3("colorBottom", &colorBottom2.x);
		//	ImGui::ColorEdit3("colorRight", &colorRight2.x);
		//	ImGui::ColorEdit3("colorLeft", &colorLeft2.x);
		//	ImGui::ColorEdit3("colorBack", &colorBack2.x);
		//	ImGui::ColorEdit3("colorFront", &colorFront2.x);
		//	ImGui::SliderFloat("Alpha", &colorAlpha.w, 0.0f, +1.0f);
		//	//if (ImGui::TreeNode("ColorValue"))
		//	//{
		//	//	ImGui::SliderFloat3("colorTop", &colorTop2.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorBottom", &colorBottom2.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorRight", &colorRight2.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorLeft", &colorLeft2.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorBack", &colorBack2.x, 0, 1);
		//	//	ImGui::SliderFloat3("colorFront", &colorFront2.x, 0, 1);
		//	//	ImGui::TreePop();
		//	//}
		//	//ImGui::TreePop();
		//}
		ImGui::Separator();
		ImGui::ColorEdit3("fog_color", &fogColor.x);
		ImGui::SliderFloat("fog_near", &fogRange.x, 0.1f, +100.0f);
		ImGui::SliderFloat("fog_far", &fogRange.y, 0.1f, +100.0f);


	}

	ImGui::End();
}

