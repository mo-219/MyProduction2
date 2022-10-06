#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

//	UVスクロール情報
struct UVScrollData
{
	DirectX::XMFLOAT2	uvScrollValue;	// UVスクロール値
};

//	平行光源情報
struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
};

// マスクデータ
struct MaskData
{
	ID3D11ShaderResourceView* maskTexture;
	float					dissolveThreshold;
	float					edgeThreshold; 	// 縁の閾値
	DirectX::XMFLOAT4			edgeColor;		// 縁の色
};


// ガウスフィルター計算情報
struct GaussianFilterData
{
	int				  kernelSize = 8;		// カーネルサイズ
	float			  deviation = 10.0f;	// 標準偏差
	DirectX::XMFLOAT2 textureSize;			// ぼかすテクスチャのサイズ 
};
static const int MaxKernelSize = 16;


// レンダーコンテキスト
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;

	//	カメラ情報
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;

	//	スクロールデータ
	UVScrollData			uvScrollData;

	// マスクデータ
	MaskData				maskData;

	//	ライト情報
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;

	// ガウスフィルター情報
	GaussianFilterData		gaussianFilterData;
};

//// レンダーコンテキスト
//struct RenderContext
//{
//	DirectX::XMFLOAT4X4		view;
//	DirectX::XMFLOAT4X4		projection;
//	DirectX::XMFLOAT4		lightDirection;
//};
