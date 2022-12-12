#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#define CUBIC_DEFAULT 0
#define CUBIC_CUSTOMIZE  1

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
	float					  dissolveThreshold = 0.0f;
	float				      edgeThreshold = 0.2f;			// 縁の閾値
	DirectX::XMFLOAT4		  edgeColor = {1,0,0,1};		// 縁の色

	float					  maskFlag = 1.0f;
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
	float			  threshold = 0.95f;		// 閾値
	float			  intensity = 1.0f;		// ブルームの強度
	DirectX::XMFLOAT2 dummy2;

};

// ポストエフェクト用の最終パス用情報
struct FinalpassData
{
	// ブルームテクスチャ
	ID3D11ShaderResourceView* bloomTexture;
};


// シャドウマップ用情報
struct ShadowMapData
{
	ID3D11ShaderResourceView*	shadowMap;				// シャドウマップテクスチャ
	DirectX::XMFLOAT4X4			lightViewProjection;	// ライトビュープロジェクション行列
	DirectX::XMFLOAT3			shadowColor;			// 影の色
	float						shadowBias;				// 深度皮革用のオフセット値
};

// キュービックカラー用
struct CubicColorData
{
	int				  shaderFlag = CUBIC_DEFAULT;		// CUBIC_DEFAULT: default	CUBIC_CUSTOMIZE: 個々で色変え
	DirectX::XMFLOAT4 rightVec;							// 右ベクトル	w: dummy
	DirectX::XMFLOAT4 topVec;							// 上ベクトル	w: dummy
	DirectX::XMFLOAT4 frontVec;							// 前ベクトル	w: dummy

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

	DirectX::XMFLOAT4 colorAlpha;	// a:　ほんとに使うアルファ値		rgb: dummy
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

	// シャドウマップ情報
	ShadowMapData			shadowmapData;

	// キュービックカラー情報
	CubicColorData			cubicColorData;

	// シャドウぼかし
	GaussianFilterData		shadowBlurData;

	int BlurCount = 0;

	// heightFogするか否か
	int heightFogFlag = 1;
};

