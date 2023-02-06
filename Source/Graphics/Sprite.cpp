#include <stdio.h> 
#include "WICTextureLoader.h"
#include "Sprite.h"
#include "Misc.h"
#include "Graphics/Graphics.h"

#include "util.h"
#include "ResourceManager.h"


// コンストラクタ
Sprite::Sprite()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	HRESULT hr = S_OK;

	// 頂点データの定義
	// 0           1
	// +-----------+
	// |           |
	// |           |
	// +-----------+
	// 2           3
	Vertex vertices[] = {
		{ DirectX::XMFLOAT3(-0.5, +0.5, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(+0.5, +0.5, 0), DirectX::XMFLOAT4(1, 0, 0, 1) },
		{ DirectX::XMFLOAT3(-0.5, -0.5, 0), DirectX::XMFLOAT4(0, 1, 0, 1) },
		{ DirectX::XMFLOAT3(+0.5, -0.5, 0), DirectX::XMFLOAT4(0, 0, 1, 1) },
	};

	// ポリゴンを描画するにはGPUに頂点データやシェーダーなどのデータを渡す必要がある。
	// GPUにデータを渡すにはID3D11***のオブジェクトを介してデータを渡します。

	// 頂点バッファの生成
	{
		// 頂点バッファを作成するための設定オプション
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(vertices);	// バッファ（データを格納する入れ物）のサイズ
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;	// UNIT.03
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとしてバッファを作成する。
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// UNIT.03
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		// 頂点バッファに頂点データを入れるための設定
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = vertices;	// ここに格納したい頂点データのアドレスを渡すことでCreateBuffer()時にデータを入れることができる。
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
		// 頂点バッファオブジェクトの生成
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertexBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ダミーテクスチャの生成
	{
		const int width = 8;
		const int height = 8;
		UINT pixels[width * height];
		::memset(pixels, 0xFF, sizeof(pixels));

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));
		data.pSysMem = pixels;
		data.SysMemPitch = width;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
		HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		textureWidth = desc.Width;
		textureHeight = desc.Height;
	}
}

// 描画実行
void Sprite::Update(
	float dx, float dy,
	float dw, float dh,
	float sx, float sy,
	float sw, float sh,
	float angle,
	float r, float g, float b, float a) const
{
	ID3D11DeviceContext* immediate_context = Graphics::Instance().GetDeviceContext();

	{
		// 現在設定されているビューポートからスクリーンサイズを取得する。
		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		immediate_context->RSGetViewports(&numViewports, &viewport);
		float screen_width = viewport.Width;
		float screen_height = viewport.Height;

		// スプライトを構成する４頂点のスクリーン座標を計算する
		DirectX::XMFLOAT2 positions[] = {
			DirectX::XMFLOAT2(dx,      dy),			// 左上
			DirectX::XMFLOAT2(dx + dw, dy),			// 右上
			DirectX::XMFLOAT2(dx,      dy + dh),	// 左下
			DirectX::XMFLOAT2(dx + dw, dy + dh),	// 右下
		};

		// スプライトを構成する４頂点のテクスチャ座標を計算する
		DirectX::XMFLOAT2 texcoords[] = {
			DirectX::XMFLOAT2(sx,      sy),			// 左上
			DirectX::XMFLOAT2(sx + sw, sy),			// 右上
			DirectX::XMFLOAT2(sx,      sy + sh),	// 左下
			DirectX::XMFLOAT2(sx + sw, sy + sh),	// 右下
		};

		// スプライトの中心で回転させるために４頂点の中心位置が
		// 原点(0, 0)になるように一旦頂点を移動させる。
		float mx = dx + dw * 0.5f;
		float my = dy + dh * 0.5f;
		for (auto& p : positions)
		{
			p.x -= mx;
			p.y -= my;
		}

		// 頂点を回転させる
		const float PI = 3.141592653589793f;
		float theta = angle * (PI / 180.0f);	// 角度をラジアン(θ)に変換
		float c = cosf(theta);
		float s = sinf(theta);
		for (auto& p : positions)
		{
			DirectX::XMFLOAT2 r = p;
			p.x = c * r.x + -s * r.y;
			p.y = s * r.x + c * r.y;
		}

		// 回転のために移動させた頂点を元の位置に戻す
		for (auto& p : positions)
		{
			p.x += mx;
			p.y += my;
		}

		// スクリーン座標系からNDC座標系へ変換する。
		for (auto& p : positions)
		{
			p.x = 2.0f * p.x / screen_width - 1.0f;
			p.y = 1.0f - 2.0f * p.y / screen_height;
		}

		// 頂点バッファの内容の編集を開始する。
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		HRESULT hr = immediate_context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// pDataを編集することで頂点データの内容を書き換えることができる。
		Vertex* v = static_cast<Vertex*>(mappedBuffer.pData);
		for (int i = 0; i < 4; ++i)
		{
			v[i].position.x = positions[i].x;
			v[i].position.y = positions[i].y;
			v[i].position.z = 0.0f;

			v[i].color.x = r;
			v[i].color.y = g;
			v[i].color.z = b;
			v[i].color.w = a;

			v[i].texcoord.x = texcoords[i].x / textureWidth;
			v[i].texcoord.y = texcoords[i].y / textureHeight;
		}

		// 頂点バッファの内容の編集を終了する。
		immediate_context->Unmap(vertexBuffer.Get(), 0);
	}
}

// シェーダーリソースビューの設定
void Sprite::SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight)
{
	shaderResourceView = srv;
	textureWidth = texWidth;
	textureHeight = texHeight;
}


//******************************************************************************

DirectX::XMFLOAT2 adjustTextOrigin(TEXT_ALIGN align, const DirectX::XMFLOAT2& v, float w, float h)
{
	DirectX::XMFLOAT2 pos = v;
	switch (align)
	{
	default:
	case TEXT_ALIGN::UPPER_LEFT:                                    break;
	case TEXT_ALIGN::UPPER_MIDDLE:  pos.x -= w / 2;                 break;
	case TEXT_ALIGN::UPPER_RIGHT:   pos.x -= w;                     break;
	case TEXT_ALIGN::MIDDLE_LEFT:   pos.y -= h / 2;                 break;
	case TEXT_ALIGN::MIDDLE:        pos.y -= h / 2; pos.x -= w / 2; break;
	case TEXT_ALIGN::MIDDLE_RIGHT:  pos.y -= h / 2; pos.x -= w;     break;
	case TEXT_ALIGN::LOWER_LEFT:    pos.y -= h;                     break;
	case TEXT_ALIGN::LOWER_MIDDLE:  pos.y -= h;     pos.x -= w / 2; break;
	case TEXT_ALIGN::LOWER_RIGHT:   pos.y -= h;     pos.x -= w;     break;
	}

	return pos;
}

static const float UV_ADJUST = 1.0f;// 0.99994f;

//--------------------------------------------------------------
//  コンストラクタ
//--------------------------------------------------------------
SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* fileName, size_t maxInstance)
{
	MAX_INSTANCES = maxInstance;

	//VertexBufferの作成
	vertex vertices[4] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT2(1, 0) },
		{ DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT2(0, 1) },
		{ DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 1) },
	};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0; //Not use for vertex buffers.
	subresourceData.SysMemSlicePitch = 0; //Not use for vertex buffers.
	if (FAILED(device->CreateBuffer(&bufferDesc, &subresourceData, &buffer)))
	{
		assert(!"頂点バッファの作成に失敗(SpriteBatch)");
		return;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT numElements = ARRAYSIZE(layout);
	if (!ResourceManager::loadVertexShader(device, "Shader\\sprite_batch_vs.cso", layout, numElements, &vertexShader, &inputLayout))
	{
		assert(!"頂点シェーダーの読み込みに失敗(SpriteBatch)");
		return;
	}
	if (!ResourceManager::loadPixelShader(device, "Shader\\sprite_batch_ps.cso", &pixelShader))
	{
		assert(!"ピクセルシェーダーの作成に失敗(SpriteBatch)");
		return;
	}

	instance* inst = new instance[MAX_INSTANCES];
	{
		D3D11_BUFFER_DESC bd = {};
		D3D11_SUBRESOURCE_DATA sd = {};

		bd.ByteWidth = sizeof(instance) * MAX_INSTANCES;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		sd.pSysMem = inst;
		sd.SysMemPitch = 0; //Not use for vertex buffers.mm 
		sd.SysMemSlicePitch = 0; //Not use for vertex buffers.
		if (FAILED(device->CreateBuffer(&bd, &sd, &instanceBuffer)))
		{
			assert(!"バッファの生成に失敗(SpriteBatch)");
			return;
		}
	}
	delete[] inst;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	if (FAILED(device->CreateRasterizerState(&rasterizerDesc, &rasterizerState)))
	{
		assert(!"ラスタライザステートの作成に失敗(SpriteBatch)");
		return;
	}

	if (!ResourceManager::loadShaderResourceView(device, fileName, &shaderResourceView, &tex2dDesc))
	{
		assert(!"テクスチャ画像読み込み失敗(SpriteBatch)");
		return;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	for (int i = 0; i < 4; i++) samplerDesc.BorderColor[i] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(device->CreateSamplerState(&samplerDesc, &samplerState)))
	{
		assert(!"サンプラーステートの生成に失敗(SpriteBatch)");
		return;
	}

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	if (FAILED(device->CreateDepthStencilState(&dsDesc, &depthStencilState)))
	{
		assert(!"デプスステンシルステートの作成に失敗(SpriteBatch)");
		return;
	}
}

//--------------------------------------------------------------
//  デストラクタ
//--------------------------------------------------------------
SpriteBatch::~SpriteBatch()
{
	safe_release(instanceBuffer);
	safe_release(depthStencilState);
	safe_release(samplerState);
	ResourceManager::releaseShaderResourceView(shaderResourceView);
	safe_release(rasterizerState);
	safe_release(buffer);
	ResourceManager::releasePixelShader(pixelShader);
	ResourceManager::releaseVertexShader(vertexShader, inputLayout);
}

//--------------------------------------------------------------
//  前処理（描画前に1度呼ぶ）
//--------------------------------------------------------------
void SpriteBatch::begin(ID3D11DeviceContext* context)
{
	HRESULT hr = S_OK;

	UINT strides[2] = { sizeof(vertex), sizeof(instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* vbs[2] = { buffer, instanceBuffer };
	context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(inputLayout);
	//context->OMSetDepthStencilState(depthStencilState, 1);
	context->RSSetState(rasterizerState);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
	context->PSSetShaderResources(0, 1, &shaderResourceView);
	context->PSSetSamplers(0, 1, &samplerState);

	UINT numViewports = 1;
	context->RSGetViewports(&numViewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	hr = context->Map(instanceBuffer, 0, map, 0, &mappedBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	instances = static_cast<instance*>(mappedBuffer.pData);

	instanceCount = 0;
}

//--------------------------------------------------------------
//  描画準備
//--------------------------------------------------------------
void SpriteBatch::render(
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
	const DirectX::XMFLOAT2& center, float angle/*radian*/,
	const DirectX::XMFLOAT4& color)
{
	if (instanceCount >= MAX_INSTANCES)
	{
		assert(!"Number of instances must be less than MAX_INSTANCES.(SpriteBatch)");
		return;
	}

	if (scale.x == 0.0f || scale.y == 0.0f) return;
	float tw = texSize.x;
	float th = texSize.y;
	if (texSize.x == FLT_MIN && texSize.y == FLT_MIN)
	{
		tw = (float)tex2dDesc.Width;
		th = (float)tex2dDesc.Height;
	}

	//	float cx = dw*0.5f, cy = dh*0.5f; /*Center of Rotation*/
	float cx = center.x;
	float cy = center.y;
	float sx = scale.x;
	float sy = scale.y;

#ifdef GAMELIB_PLUS_UP
	sy = -sy; // Y軸上が正ならスケール反転
	cy = texSize.y - cy;
#endif

	cx *= sx;
	cy *= sy;

#if 0
	DirectX::XMVECTOR scaling = DirectX::XMVectorSet(dw, dh, 1.0f, 0.0f);
	DirectX::XMVECTOR origin = DirectX::XMVectorSet(cx, cy, 0.0f, 0.0f);
	DirectX::XMVECTOR translation = DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f);
	DirectX::XMMATRIX M = DirectX::XMMatrixAffineTransformation2D(scaling, origin, angle * 0.01745f, translation);
	DirectX::XMMATRIX N(
		2.0f / viewport.Width, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / viewport.Height, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&instances[count_instance].ndc_transform, DirectX::XMMatrixTranspose(M * N)); //column_major
#else
	FLOAT c = cosf(angle);
	FLOAT s = sinf(angle);
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;

	instances[instanceCount].ndcTransform._11 = w * sx * tw * c;
	instances[instanceCount].ndcTransform._21 = h * sx * tw * s;
	instances[instanceCount].ndcTransform._31 = 0.0f;
	instances[instanceCount].ndcTransform._41 = 0.0f;
	instances[instanceCount].ndcTransform._12 = w * sy * th * -s;
	instances[instanceCount].ndcTransform._22 = h * sy * th * c;
	instances[instanceCount].ndcTransform._32 = 0.0f;
	instances[instanceCount].ndcTransform._42 = 0.0f;
	instances[instanceCount].ndcTransform._13 = 0.0f;
	instances[instanceCount].ndcTransform._23 = 0.0f;
	instances[instanceCount].ndcTransform._33 = 1.0f;
	instances[instanceCount].ndcTransform._43 = 0.0f;
	instances[instanceCount].ndcTransform._14 = w * (-cx * c + -cy * -s + cx * 0 + position.x) - 1.0f;
	instances[instanceCount].ndcTransform._24 = h * (-cx * s + -cy * c + cy * 0 + position.y) + 1.0f;
	instances[instanceCount].ndcTransform._34 = 0.0f;
	instances[instanceCount].ndcTransform._44 = 1.0f;
#ifdef GAMELIB_PLUS_UP
	instances[instanceCount].ndcTransform._21 *= -1;
	instances[instanceCount].ndcTransform._22 *= -1;
	instances[instanceCount].ndcTransform._24 *= -1;
#endif
#endif
	float tex_width = static_cast<float>(tex2dDesc.Width);
	float tex_height = static_cast<float>(tex2dDesc.Height);

	// UV座標の調整
	float u = tw * UV_ADJUST / tex_width;
	float v = th * UV_ADJUST / tex_height;

	instances[instanceCount].texcoordTransform = DirectX::XMFLOAT4(texPos.x / tex_width, texPos.y / tex_height, u, v);
	instances[instanceCount].color = color;

	instanceCount++;
}

//--------------------------------------------------------------
//  テキスト描画準備
//--------------------------------------------------------------
float SpriteBatch::textout(std::string str,
	const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT4& color,
	TEXT_ALIGN align, bool world)
{
	float tw = static_cast<float>(tex2dDesc.Width / 16.0);
	float th = static_cast<float>(tex2dDesc.Height / 16.0);
	float cursor = 0.0f;

	// alignを元に左上の座標を計算
	float textWidth = str.length() * tw * scale.x;
	float textHeight = th * scale.y;
	DirectX::XMFLOAT2 position = adjustTextOrigin(align, pos, textWidth, textHeight);

	if (world)
	{
		RECT rc;
		//GetWindowRect(MouseController::Instance().GetHWnd(), &rc);
		//GetWindowRect(Graphics::Instance().GetDevice(), &rc);

		//DirectX::XMFLOAT2 v = view::worldToScreen(position);
		//DirectX::XMFLOAT2 s = scale * view::getScale();

		rc = Graphics::Instance().GetRect();

		DirectX::XMFLOAT2 v =
		{ position.x - rc.left * 1.0f,
		  position.y - rc.top * 1.0f };
		DirectX::XMFLOAT2 s =
		{ scale.x * 1.0f, scale.y * 1.0f };

		for (const auto& c : str)
		{
			render(DirectX::XMFLOAT2(v.x + cursor, v.y), s, DirectX::XMFLOAT2(tw * (c & 0x0F), th * (c >> 4)),
				DirectX::XMFLOAT2(tw, th), DirectX::XMFLOAT2(0, 0), 0, color);
			cursor += tw * s.x;
		}
		return th * s.y;
	}
	else
	{
		float y;
#ifdef GAMELIB_PLUS_UP
		y = (window::getHeight() - position.y) - th * scale.y;
#else
		y = position.y;
#endif

		for (const auto& c : str)
		{
			render(DirectX::XMFLOAT2(position.x + cursor, y), scale, DirectX::XMFLOAT2(tw * (c & 0x0F), th * (c >> 4)),
				DirectX::XMFLOAT2(tw, th), DirectX::XMFLOAT2(0, 0), 0, color);
			cursor += tw * scale.x;
		}
		return th * scale.y;
	}
}

//--------------------------------------------------------------
//  後処理（描画準備後に1度呼ぶ）
//--------------------------------------------------------------
void SpriteBatch::end(ID3D11DeviceContext* context)
{
	context->Unmap(instanceBuffer, 0);
	context->DrawInstanced(4, instanceCount, 0, 0);
}
//******************************************************************************

