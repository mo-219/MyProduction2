#pragma once

#include "Scene.h"
#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Light.h"

#include "Graphics/PostprocessingRenderer.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTarget.h"


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
	

	// オフスクリーンレンダリング用描画バッファ
	std::unique_ptr<RenderTarget> renderTarget;

	// ポストプロセス
	std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;
};
