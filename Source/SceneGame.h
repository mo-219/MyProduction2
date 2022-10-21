#pragma once

#include "Scene.h"
#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Graphics/Light.h"

#include "Graphics/PostprocessingRenderer.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"



// ゲームシーン
class SceneGame : public  Scene
{
public:
	//SceneGame() {}
	~SceneGame() override {}

	// 初期化
	void Initialize()override;

	// 終了化
	void Finalize()override;

	// 更新処理
	void Update(float elapsedTime)override;

	// 描画処理
	void Render()override;

	// 3D空間の描画
	void Render3DScene();

	// シャドウ用情報
	void RenderShadowmap();

private:
	Stage* stage = nullptr;
	Player* player = nullptr;

	CameraController* cameraController = nullptr;

	// 平行光源データ
	DirectX::XMFLOAT4			ambientLightColor;
	std::unique_ptr<Light>		directional_light;


	// 2Dデータ用変数
	// UVスクロールデータ
	UVScrollData			uvScrollData;

	// マスク画像
	std::unique_ptr<Texture>	maskTexture;
	float						dissolveThreshold;
	float						edgeThreshold; 	// 縁の閾値
	DirectX::XMFLOAT4			edgeColor;		// 縁の色

	std::unique_ptr<Sprite>		sprite;
	std::unique_ptr<Texture>	texture;

	//// ガウスフィルターデータ
	//GaussianFilterData			gaussianFilterData;
	//std::unique_ptr<Texture>	gaussianBlurTexture;
	//std::unique_ptr<Sprite>		gaussianBlurSprite;

	//// ブルームデータ
	//LuminanceExtractionData luminanceExtractionData;

	
	// シャドウ用情報
	Light* mainDirectionalLight = nullptr;
	float shadowDrawRect = 500.0f;

	DirectX::XMFLOAT4X4 lightViewProjection;			  // ライトビュープロジェクション行列
	DirectX::XMFLOAT3 shadowColor = { 0.1f, 0.1f, 0.1f}; // 影の色
	float shadowBias = 0.0003f;

	std::unique_ptr<DepthStencil>	shadowmapDepthStencil;	// シャドウ用深度ステンシル

	// オフスクリーンレンダリング用描画バッファ
	std::unique_ptr<RenderTarget> renderTarget;


	// ポストプロセス
	std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;

	ModelShaderId id = ModelShaderId::Cubic;
};
