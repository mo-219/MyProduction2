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

// 高輝度出力用情報
struct LuminanceExtractionData
{
	float			  threshold = 0.5f;		// 閾値
	float			  intensity = 1.0f;		// ブルームの強度
	DirectX::XMFLOAT2 dummy2;

};

// ポストエフェクト用の最終パス用情報
struct FinalpassData
{
	// ブルームテクスチャ
	ID3D11ShaderResourceView* bloomTexture;
};



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

	// 高輝度抽出用情報
	LuminanceExtractionData luminanceExtractionData;

	// 最終パス情報
	FinalpassData			finalpassData;
};

//// レンダーコンテキスト
//struct RenderContext
//{
//	DirectX::XMFLOAT4X4		view;
//	DirectX::XMFLOAT4X4		projection;
//	DirectX::XMFLOAT4		lightDirection;
//};
