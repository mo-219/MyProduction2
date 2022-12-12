#pragma once

#include "Scene.h"
#include "Stage.h"
#include "CameraController.h"
#include "Graphics/Light.h"
#include "Graphics/SkyBox.h"
#include "Player.h"

#include "Graphics/PostprocessingRenderer.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"
#include "Rect.h"

#include "LevelScript.h"



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

	// エネミーHPゲージの描画
	void RenderEnemyGauge(
		const RenderContext& rc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);


private:

	enum class State
	{
		Error = -1,
		INITIALIZE = 0,
		FADEOUT,
		UPDATE,
		FADEIN,
		END,

		MAX
	};

	Rect fade;		// 画面遷移用
	Player* player = nullptr;
	SkyBox* sky = nullptr;
	Sprite* guage = nullptr;
	State state = State::INITIALIZE;
	int currentStageNum = 0;

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


	
	// シャドウ用情報
	Light* mainDirectionalLight = nullptr;
	float shadowDrawRect = 80.0f;

	DirectX::XMFLOAT4X4 lightViewProjection;			  // ライトビュープロジェクション行列
	DirectX::XMFLOAT3 shadowColor = { 0.01f, 0.0f, 1.0f}; // 影の色
	float shadowBias = 0.00045f;

	//std::unique_ptr<RenderTarget>	shadowmapRenderTarget;	// シャドウ用深度ステンシル
	std::unique_ptr<Sprite>         shadowSprite;
	std::unique_ptr<RenderTarget>   shadowBlurTarget;
	std::unique_ptr<DepthStencil>	shadowmapDepthStencil;	// シャドウ用深度ステンシル
	int				  shadowKernelSize = 8;		// カーネルサイズ
	float			  shadowDeviation = 10.0f;	// 標準偏差



	// オフスクリーンレンダリング用描画バッファ
	std::unique_ptr<RenderTarget> renderTarget;


	// ポストプロセス
	std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;

	ModelShaderId id = ModelShaderId::Cubic;
};
