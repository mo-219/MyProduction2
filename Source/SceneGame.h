#pragma once

//--<インクルードファイル>---------------------------
#include "Scene.h"

// グラフィックス
#include "Graphics/Light.h"
#include "Graphics/PostprocessingRenderer.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"

// カメラ、入力用
#include "CameraController.h"

#include "Stage.h"
#include "Player.h"
#include "LevelScript.h"
#include "Rect.h"
#include "MetaAI.h"

// 音用
#include "Audio/Audio.h"

//--------------------------------------------------




// ゲームシーン
class SceneGame : public  Scene
{
public:
	~SceneGame() override {}

	// 初期化
	void Initialize()override;

	// 終了化
	void Finalize()override;

	// 更新処理
	void Update(float elapsedTime)override;

	void UiUpdate(float elapsedTime);

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

	void ScreenShake();


	struct Font
	{
		std::string text = {};

		DirectX::XMFLOAT2 position = { 0,0 };
		DirectX::XMFLOAT2 scale = { 4,4 };
		DirectX::XMFLOAT4 color = { 1,0.1f,0.4f,0 };
	};

	struct WaveUI
	{
		Font wave = {};
		Font enemy = {};

		int waveCount = 0;
		int currentEnemyCount = 0;
		int enemyCounts = 0;
		int timer = 0;
		int state = 0;
	};


private:

	enum class State
	{
		Error = -1,
		INITIALIZE = 0,
		FADEOUT,
		UPDATE,
		FADEIN,
		END,
		RESULT,

		MAX
	};

	CameraController*	cameraController = nullptr;	// カメラコントローラー
	Player*				player = nullptr;			// プレイヤー変数
	Meta*				meta = nullptr;				// メタAI保存用


//----<UI用変数>------------------------------------

	RectFade*	fade;			// 画面遷移用
	Rect*		result;			// リザルト描画用
	RectBar*	HPBar;
	RectBar*	APBar;
	Sprite*		guage = nullptr;

	std::unique_ptr<Sprite>		sprite;
	std::unique_ptr<Texture>	texture;

	std::unique_ptr<Sprite>		manualSprite;
	std::unique_ptr<Texture>	manualTexture;

	std::unique_ptr<Sprite>		HPIconSprite;
	std::unique_ptr<Texture>	HPIconTexture;
	std::unique_ptr<Sprite>		APIconSprite;
	std::unique_ptr<Texture>	APIconTexture;


//---------------------------------------------------

//----<Update用変数>---------------------------------------------------

	State	state = State::INITIALIZE;	// Update用
	int		currentStageNum = 0;		// 現在のステージ
	bool	sceneChange = false;		// シーンチェンジ用フラグ


//--------------------------------------------------------------------


//----<グラフィックス用変数>------------------------------------

	// 平行光源データ
	DirectX::XMFLOAT4			ambientLightColor;
	std::unique_ptr<Light>		directional_light;


//------------------------------------------------------------

	
//----<シャドウ用変数>--------------------------------------------------------------------------
	Light* mainDirectionalLight = nullptr;
	float shadowDrawRect = 80.0f;
	DirectX::XMFLOAT4X4 lightViewProjection;			  // ライトビュープロジェクション行列
	DirectX::XMFLOAT3 shadowColor = { 0.01f, 0.0f, 1.0f}; // 影の色
	float shadowBias = 0.00015f;
	std::unique_ptr<DepthStencil>	shadowmapDepthStencil;	// シャドウ用深度ステンシル
//---------------------------------------------------------------------------------------------


	// オフスクリーンレンダリング用描画バッファ
	std::unique_ptr<RenderTarget> renderTarget;
	std::unique_ptr<DepthStencil> depthStencil;	// シャドウ用深度ステンシル


	// ポストプロセス
	std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;
	
	ModelShaderId id = ModelShaderId::Cubic;
	DirectX::XMFLOAT2 renderPosition = { 0,0 };

	WaveUI waveUI = {};


	std::unique_ptr<AudioSource> bgm;
	std::unique_ptr<AudioSource> se;

};



