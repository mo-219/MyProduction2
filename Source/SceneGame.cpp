
//--<インクルードファイル>---------------------------
#include "SceneGame.h"

// Graphicsファイル
#include "Graphics/Graphics.h"
#include "Graphics/Light.h"
#include "Graphics/font.h"

// カメラ、入力系
#include "Camera.h"
#include "Input/Input.h"

// データローディング用
#include "DataLoading.h"

// マネージャークラス
#include "EnemyManager.h"
#include "EffectManager.h"
#include "StageManager.h"
#include "GameObjectManager.h" 
#include "LevelManager.h"
#include "LightManager.h"

#include "StageDoor.h"


// シーン遷移用
#include "SceneManager.h"
#include "SceneTitle.h"

//----------------------------------------------


// シャドウマップのサイズ
static const UINT SHADOWMAP_SIZE = 2048;

// 初期化
void SceneGame::Initialize()
{

	timer = 0;

	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();

	// プレイヤー初期化
	player = new Player();

	// ゲージスプライト
	guage = new Sprite();


	// 画面遷移用
	fade = new RectFade();
	
	fade->SetAll(0, 0, 1280, 720, 1280, 720, 0, 0, 0, 1);

	HPBar = new RectBar();
	HPBar->SetAll(70, 20, 300, 20, 300, 20, 1, 0, 0, 1);
	HPBar->SetBaseAll(3, 3, 0, 0, 0, 1);

	APBar = new RectBar();
	APBar->SetAll(70, 70, 300, 20, 300, 20, 0, 1, 1, 1);
	APBar->SetBaseAll(3, 3, 0, 0, 0, 1);

	result = new Rect();
	result->SetAll(0, 0, 1280, 720, 1280, 720, 0, 0, 0, 0);


	// レベルマネージャー初期化
	LevelManager::Instance().Initialize(all_script);


	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(DirectX::XMFLOAT3(0, 10, -10), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	camera.SetPerspectiveFov(DirectX::XMConvertToRadians(45),
							 graphics.GetScreenWidth() / graphics.GetScreenHeight(),
							 0.1f, 1000.0f);

	{

		// 新しい描画ターゲットの生成
		{
			Graphics& graphics = Graphics::Instance();

			renderTarget = std::make_unique<RenderTarget>(static_cast<UINT>(graphics.GetScreenWidth() + graphics.GetScreenBlankWidth()),
														  static_cast<UINT>(graphics.GetScreenHeight() + graphics.GetScreenBlankHeight()),
														  DXGI_FORMAT_R8G8B8A8_UNORM);

			renderPosition = DirectX::XMFLOAT2(-graphics.GetScreenBlankWidth() / 2, -graphics.GetScreenBlankHeight() / 2);

		}

		mainDirectionalLight = new Light(LightType::Directional);
		mainDirectionalLight->SetDirection(DirectX::XMFLOAT3(1, -1, -1));
		LightManager::Instance().Register(mainDirectionalLight);

	}

	// シャドウマップ用に深度ステンシルの生成
	{
		Graphics& graphics = Graphics::Instance();
		shadowmapDepthStencil = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		depthStencil = std::make_unique<DepthStencil>(renderTarget->GetWidth(), renderTarget->GetHeight());
	}

	// ポストプロセス描画クラス生成
	{
		postprocessingRenderer = std::make_unique<PostprocessingRenderer>();

		// シーンテクスチャ設定
		ShaderResourceViewData srvData;

		srvData.srv = renderTarget->GetShaderResourceView();
		srvData.width = renderTarget->GetWidth();
		srvData.height = renderTarget->GetHeight();
		postprocessingRenderer->SetSceneData(srvData);
	}

	// UI取得
	waveUI.wave.text = "Wave ";
	waveUI.wave.position = { graphics.GetScreenWidth() / 2, graphics.GetScreenHeight() / 2 };
	waveUI.wave.scale = { 4,4 };
	waveUI.wave.color = { 1.0f, 0.1f, 0.4f, 0.0f};

	waveUI.enemy.text = "EnemyCount: ";
	waveUI.enemy.position = { waveUI.wave.position.x, waveUI.wave.position.y + 60 };
	waveUI.enemy.scale = { 2.5f, 2.5f };
	waveUI.enemy.color = { 0.0f, 0.0f, 0.0f, 0.0f };

	// UI用スプライト初期化
	manualTexture = std::make_unique<Texture>("Data/Sprite/Manual_tenzi.png");
	manualSprite = std::make_unique<Sprite>();
	manualSprite->SetShaderResourceView(manualTexture->GetShaderResourceView(), manualTexture->GetWidth(), manualTexture->GetHeight());

	HPIconTexture = std::make_unique<Texture>("Data/Sprite/HP.png");
	HPIconSprite = std::make_unique<Sprite>();
	HPIconSprite->SetShaderResourceView(HPIconTexture->GetShaderResourceView(), HPIconTexture->GetWidth(), HPIconTexture->GetHeight());

	APIconTexture = std::make_unique<Texture>("Data/Sprite/AP.png");
	APIconSprite = std::make_unique<Sprite>();
	APIconSprite->SetShaderResourceView(APIconTexture->GetShaderResourceView(), APIconTexture->GetWidth(), APIconTexture->GetHeight());



	// カメラコントローラー初期化
	cameraController = new CameraController();



	// ステージデータ読み込み
	LoadObj load;
	load.Load("Data/Stage/SimpleNaturePack/GameObjectData.json");
	load.Create(stage1_mapData);


	// メタAI取得
	EnemyManager& enemyManager = EnemyManager::Instance();
	meta = new Meta(player, &enemyManager);


	// Audio ローディング
	Audio& audio = Audio::Instance();
	bgm = audio.LoadAudioSource("Data/Audio/BGM/Game.wav");
	se = audio.LoadAudioSource("Data/Audio/SE/Ashioto.wav");

	bgm->Play(true);
}

// 終了化
void SceneGame::Finalize()
{
	// メタAI終了化
	if (meta != nullptr)
	{
		delete meta;
		meta = nullptr;
	}

	// エネミー終了化
	EnemyManager::Instance().Clear();

	// ステージオブジェクト終了化
	ObjectManager::Instance().Clear();

	// レベルマネージャー終了化
	LevelManager::Instance().Clear();

	// カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	if (result != nullptr)
	{
		delete result;
		result = nullptr;
	}
	if (fade != nullptr)
	{
		delete fade;
		fade = nullptr;
	}
		
	if (HPBar != nullptr)
	{
		delete HPBar;
		HPBar = nullptr;
	}
	if (APBar != nullptr)
	{
		delete APBar;
		APBar = nullptr;
	}
	// ゲージスプライト終了化
	if (guage != nullptr)
	{
		delete guage;
		guage = nullptr;
	}

	// プレイヤー終了化
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	LightManager::Instance().Clear();
	StageManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	LevelManager& levelManager = LevelManager::Instance();
	EnemyManager& enemyManager = EnemyManager::Instance();
	StageManager& stageManager = StageManager::Instance();
	EffectManager& effctManager = EffectManager::Instance();
	ObjectManager& objectManager = ObjectManager::Instance();

	DirectX::XMFLOAT3 target;
	StageDoor* stageDoor;
	switch (state)
	{
	case State::INITIALIZE:
		// 初期化

		// ステージの初期化
		levelManager.Initialize(currentStageNum);
		levelManager.Update(elapsedTime);	// リスポン位置取得


		// UIを初期位置に戻す
		waveUI.wave.position = { Graphics::Instance().GetScreenWidth() / 2, Graphics::Instance().GetScreenHeight() / 2 };
		waveUI.wave.color.w = 0;
		waveUI.waveCount++;

		waveUI.enemy.position = { waveUI.wave.position.x, waveUI.wave.position.y + 60 };
		waveUI.enemy.color.w = 0;
		waveUI.enemyCounts = levelManager.GetEnemyCounts();

		waveUI.state = 0;


		// プレイヤーの初期化
		player->Initialize(levelManager.GetRespawnPos());
		player->SetMaxPos(levelManager.GetMaxPos());
		player->SetMinPos(levelManager.GetMinPos());
		player->SetIsInputMove(false);

		timer = 0;


		// 倒した敵の数の初期化
		enemyManager.SetCurrentEnemyCount(0);


		// ドア生成
		stageDoor = new StageDoor();
		stageDoor->setPosition(0, 0, -4);
		stageDoor->setWidth(3.5);
		stageDoor->setHeight(4);
		stageDoor->setDepth(2);
		stageDoor->setStageNum(StageNumber::Door);
		stageManager.Register(stageDoor);


		state = State::FADEOUT;

		/*fallthrough*/
	case State::FADEOUT:
		// フェードアウト

		if (fade->fadeOut(0.01f))
		{
			state = State::UPDATE;
			player->SetIsInputMove(true);
		}

		stageManager.Update(elapsedTime);

		// カメラコントローラー更新処理
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->UpdateOnlyTransform(elapsedTime);

		
		objectManager.Update(elapsedTime);
		

		enemyManager.UpdateOnlyTransform(elapsedTime);

		// エフェクト更新処理
		effctManager.Update(elapsedTime);
		break;

	case State::UPDATE:
		UiUpdate(elapsedTime);

		// 更新処理
		if (player->GetStageClearFlag())
		{
			// クリアしたら
			state = State::END;
			stageManager.StageNumDelete(StageNumber::Door);
			player->SetIsInputMove(false);
			se->Play(false);
			break;
		}
		if (!player->IsLive())
		{
			// プレイヤーが死んだら
			state = State::RESULT;
			break;
		}
		stageManager.Update(elapsedTime);
		levelManager.Update(elapsedTime);


		// カメラコントローラー更新処理
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->Update(elapsedTime);


		// その他更新処理
		enemyManager.Update(elapsedTime);
		waveUI.currentEnemyCount = enemyManager.GetCurrentEnemyCount();
		objectManager.Update(elapsedTime);

		effctManager.Update(elapsedTime);

		break;
	case State::FADEIN:
		// フェードイン
		if (fade->fadeIn(0.01f))
		{
			if (sceneChange)
			{
				SceneManager::Instance().ChangeScene(new SceneTitle);
			}
			else
			{
				state = State::END;
			}
		}

		stageManager.Update(elapsedTime);
		//
		objectManager.Update(elapsedTime);
		//

		// カメラコントローラー更新処理
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->UpdateOnlyTransform(elapsedTime);


		enemyManager.UpdateOnlyTransform(elapsedTime);

		// エフェクト更新処理
		effctManager.Update(elapsedTime);
		break;

	case State::END:
		// ステージが終わった際
		enemyManager.Clear();	//一応全部消しとく
		

		++currentStageNum;
		if (currentStageNum > levelManager.GetStageMax())currentStageNum = 0;
		state = State::INITIALIZE;
		break;

	case State::RESULT:
		// 結果表示

		result->color.w += 0.01f;
		if (result->color.w >= 0.8f)	result->color.w = 0.8f;
		waveUI.enemy.color.w -= 0.03f;
		waveUI.wave.position.y += 2.0f;
		if (waveUI.wave.position.y >= 300)
		{
			// 何かしらボタンが押されたら
			waveUI.wave.position.y = 300;
			const GamePadButton anyButton = GamePad::BTN_UP
				| GamePad::BTN_RIGHT
				| GamePad::BTN_DOWN
				| GamePad::BTN_LEFT
				| GamePad::BTN_A
				| GamePad::BTN_B
				| GamePad::BTN_X
				| GamePad::BTN_Y
				| GamePad::BTN_START
				| GamePad::BTN_BACK
				| GamePad::BTN_LEFT_THUMB
				| GamePad::BTN_RIGHT_THUMB
				| GamePad::BTN_LEFT_SHOULDER
				| GamePad::BTN_RIGHT_SHOULDER
				| GamePad::BTN_LEFT_TRIGGER
				| GamePad::BTN_RIGHT_TRIGGER;

			if (Input::Instance().GetGamePad().GetButtonDown() & anyButton)
			{
				sceneChange = true;
				state = State::FADEIN;
			}

		}

		break;
	}

	player->Update(elapsedTime);

	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	HPBar->culcValue(player->GetHealth(), player->GetMaxHealth());
	APBar->culcValue(player->GetCurrentAP(), player->GetMaxAP());

	//renderPosition.y += sinf(timer * elapsedTime * 50.0f) / 0.3f;


	timer++;
}

// UI更新処理
void SceneGame::UiUpdate(float elapsedTime)
{
	switch (waveUI.state)
	{
	case 0:
		// フェード
		waveUI.wave.color.w += 0.1f;
		waveUI.enemy.color.w += 0.1f;
		if (waveUI.wave.color.w >= 1)
		{
			waveUI.state++;
		}
		break;

	case 1:
		// 停止
		waveUI.timer++;
		if (waveUI.timer >= 100)
		{
			waveUI.state++;
		}
		break;

	case 2:
		//上に上がっていく
		waveUI.wave.position.y -= 2.0f;
		waveUI.enemy.position.y -= 2.0f;
		if (waveUI.wave.position.y <= 60)
		{
			waveUI.wave.position.y = 60;
			waveUI.state++;
		}
		break;

	case 3:
		// 通常時何もしない

		break;

	}

}




// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// シャドウマップの描画
	RenderShadowmap();

	// 3D空間の描画を別バッファに対して行う
	Render3DScene();


	// 描き込み先をバックバッファに変えてオフスクリーンレンダリングの結果を描画する
	{
		ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
		ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

		// 画面クリア＆レンダーターゲット設定
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0～1.0)
		dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		dc->OMSetRenderTargets(1, &rtv, dsv);

		// ビューポートの設定
		D3D11_VIEWPORT	vp = {};
		vp.Width = renderTarget->GetWidth();
		vp.Height = renderTarget->GetHeight();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		dc->RSSetViewports(1, &vp);

		// ポストプロセス処理
		postprocessingRenderer->SetRenderPosition(renderPosition);
		postprocessingRenderer->Render(dc);
	}



	// 3Dデバッグ描画
	{
		//ImGui

		
		if (ImGui::TreeNode("Shadowmap"))
		{
			ImGui::SliderFloat("DrawRect", &shadowDrawRect, 1.0f, 2048.0f);
			ImGui::ColorEdit3("Color", &shadowColor.x);
			ImGui::SliderFloat("Bias", &shadowBias, 0.0f, 0.1f);
			ImGui::Text("texture");
			ImGui::Image(shadowmapDepthStencil->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 },
				{ 1, 1, 1, 1 });
			ImGui::TreePop();
		}
		ImGui::Separator();

		if (ImGui::Button("Phong"))
		{
			id = ModelShaderId::Phong;
		}
		if (ImGui::Button("Cubic"))
		{
			id = ModelShaderId::Cubic;
		}
		ImGui::Separator();
		LightManager::Instance().DrawDebugGUI();
		ImGui::Separator();

		ImGui::SliderFloat("renderPositionX", &renderPosition.x, -500.0f, 500.0f);
		ImGui::SliderFloat("renderPositionY", &renderPosition.y, -500.0f, 500.0f);

		{
			// プレイヤーデバッグ描画
			player->DrawDebugGUI();
			StageManager::Instance().DrawDebugGUI();
			cameraController->DrawDebugGUI();
			postprocessingRenderer->DrawDebugGUI();
			ModelShader* shader = graphics.GetShader(ModelShaderId::Cubic);
			EnemyManager::Instance().DrawDebugGUI();
			shader->DebugGUI();

			//ObjectManager::Instance().setNearNum(
			//ObjectManager::Instance().findNear(player->GetPosition()));
			//ObjectManager::Instance().DrawDebugGUI();
		}
	}

	// 2Dスプライト描画
	{
		RenderContext rc;
		rc.deviceContext = dc;

		// 画面遷移用
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);

		manualSprite->SetShaderResourceView(manualTexture->GetShaderResourceView(), manualTexture->GetWidth(), manualTexture->GetHeight());
		manualSprite->Update(0.0f, 0.0f,
			Graphics::Instance().GetScreenWidth(), Graphics::Instance().GetScreenHeight(),
			0.0f, 0.0f,
			static_cast<float>(manualTexture->GetWidth()), static_cast<float>(manualTexture->GetHeight()),
			0.0f,
			1.0f, 1.0f, 1.0f, 1.0f);

		HPBar->Render(rc, shader);
		HPIconSprite->Update(HPBar->position.x - HPIconTexture->GetWidth()+5, HPBar->position.y - 15,
			static_cast<float>(HPIconTexture->GetWidth()), static_cast<float>(HPIconTexture->GetHeight()),
			0.0f, 0.0f,
			static_cast<float>(HPIconTexture->GetWidth()), static_cast<float>(HPIconTexture->GetHeight()),
			0.0f,
			1.0f, 1.0f, 1.0f, 1.0f);


		APBar->Render(rc, shader);
		APIconSprite->Update(APBar->position.x - APIconTexture->GetWidth()+5, APBar->position.y -15,
			static_cast<float>(APIconTexture->GetWidth()), static_cast<float>(APIconTexture->GetHeight()),
			0.0f, 0.0f,
			static_cast<float>(APIconTexture->GetWidth()), static_cast<float>(APIconTexture->GetHeight()),
			0.0f,
			1.0f, 1.0f, 1.0f, 1.0f);



		shader->Begin(rc);
		shader->Draw(rc, manualSprite.get());
		shader->Draw(rc, HPIconSprite.get());
		shader->Draw(rc, APIconSprite.get());
		shader->End(rc);

		// 結果発表描画
		result->Render(rc, shader);
		if (state == State::RESULT)
		{
			if (waveUI.wave.position.y >= 300)
			{
				font::textOut(3, "Your Score: ",
					DirectX::XMFLOAT2(waveUI.wave.position.x - 200, waveUI.wave.position.y - 80),
					waveUI.enemy.scale,
					DirectX::XMFLOAT4(1, 1, 1, 1),
					TEXT_ALIGN::MIDDLE);
			}
		}


		font::textOut(3, waveUI.wave.text + std::to_string(waveUI.waveCount),
						 waveUI.wave.position,
						 waveUI.wave.scale,
						 waveUI.wave.color,
						 TEXT_ALIGN::MIDDLE);
		font::textOut(3, waveUI.enemy.text + std::to_string(waveUI.currentEnemyCount) + " / " + std::to_string(waveUI.enemyCounts),
						 waveUI.enemy.position,
						 waveUI.enemy.scale,
						 waveUI.enemy.color,
						 TEXT_ALIGN::MIDDLE);

		// フェード描画用
		fade->Render(rc, shader);

	}


}

// 3DモデルをrenderTargetに描画
void SceneGame::Render3DScene()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = renderTarget->GetRenderTargetView().Get();
	ID3D11DepthStencilView* dsv = depthStencil->GetDepthStencilView().Get();


	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);


	// ビューポート設定
	D3D11_VIEWPORT vp = {};
	vp.Width = renderTarget->GetWidth();
	vp.Height = renderTarget->GetHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);


	// 描画処理
	RenderContext rc;
	rc.deviceContext = dc;

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);


	// シャドウマップの設定
	rc.shadowmapData.shadowMap = shadowmapDepthStencil->GetShaderResourceView().Get();
	rc.shadowmapData.lightViewProjection = lightViewProjection;
	rc.shadowmapData.shadowColor = shadowColor;
	rc.shadowmapData.shadowBias = shadowBias;



	// カメラパラメータ
	Camera& camera = Camera::Instance();
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();



	// 3Dモデル描画
	{
		ModelShader* shader = graphics.GetShader(id);

		shader->Begin(rc);

		// ステージ描画
		StageManager::Instance().Render(rc,shader);
		EnemyManager::Instance().Render(rc, shader);
		ObjectManager::Instance().Render(rc, shader);
		player->Render(rc, shader);
		shader->End(rc);
	}



	//3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	//3D2DUI
	{
		RenderEnemyGauge(rc, rc.view, rc.projection);

	}


	// デバッグプリミティブの表示
	{
		// グリッド描画
		//player->DrawDebugPrimitive();
		//player->sword->DrawDebugPrimitive();
		//EnemyManager::Instance().DrawDebugPrimitive();
		//StageManager::Instance().DrawDebugPrimitive();
		//ObjectManager::Instance().DrawDebugPrimitive();

		//// ラインレンダラ描画実行
		//graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);
		//// デバッグレンダラ描画実行
		//graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);

	}

}

// 影データ取得
void SceneGame::RenderShadowmap()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11DepthStencilView* dsv = shadowmapDepthStencil->GetDepthStencilView().Get();

	// 画面クリア
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (!mainDirectionalLight)
		return;

	// レンダーターゲット設定
	dc->OMSetRenderTargets(0, &rtv, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(shadowmapDepthStencil->GetWidth());
	vp.Height = static_cast<float>(shadowmapDepthStencil->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	// 描画処理
	RenderContext rc;
	rc.deviceContext = dc;

	// カメラパラメータ設定
	{
		// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&mainDirectionalLight->GetDirection());
		LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// シャドウマップに描画したい範囲の射影行列を生成
		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowDrawRect, shadowDrawRect, 0.1f, 1000.0f);
		DirectX::XMStoreFloat4x4(&rc.view, V);
		DirectX::XMStoreFloat4x4(&rc.projection, P);
		DirectX::XMStoreFloat4x4(&lightViewProjection, V * P);
	}
	// 3Dモデル描画
	{
		ModelShader* shader = graphics.GetShader(ModelShaderId::ShadowmapCaster);

		shader->Begin(rc);

		StageManager::Instance().Render(rc, shader);
		player->Render(rc, shader);
		EnemyManager::Instance().Render(rc, shader);
		ObjectManager::Instance().Render(rc, shader);

		shader->End(rc);
	}
}

// エネミー体力ゲージ描画
void SceneGame::RenderEnemyGauge(
	const RenderContext& rc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;

	rc.deviceContext->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();					// 単位行列
	// もしXMVector3Unprojectを使うのならば逆行列にする必要とかがある

	// 全ての敵の頭上にHPゲージを表示
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();


	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		DirectX::XMFLOAT3 overHead = enemy->GetPosition();
		overHead = { overHead.x, overHead.y + enemy->GetHeight() + 0.2f, overHead.z };

		// エネミーの頭上のワールド座標
		DirectX::XMVECTOR worldEnemyPos = DirectX::XMLoadFloat3(&overHead);

		// ワールド座標からスクリーン座標への変換
		DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
			worldEnemyPos,			// ワールド座標
			viewport.TopLeftX,		// ビューポート左上X位置
			viewport.TopLeftY,		// ビューポート左上Y位置
			viewport.Width,			// ビューポート幅
			viewport.Height,		// ビューポート高さ
			viewport.MinDepth,		// 深度値の範囲を表す最小値(0.0fでよい)
			viewport.MaxDepth,		// 深度値の範囲を表す最大値(1.0fでよい)
			Projection,				// プロジェクション行列
			View,					// ビュー行列
			World					// ワールド行列(単位行列でよい)
		);


		// スクリーン座標(XMFLOAT2)
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		// ゲージの長さ
		const float guageWidth = 30.0f;
		const float guageHeight = 5.0f;

		float healthRate = (float)enemy->GetHealth() / enemy->GetMaxHealth();


		Graphics& graphics = Graphics::Instance();
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);

		shader->Begin(rc);

		// ゲージ描画
		guage->Update(screenPosition.x - 1 - guageWidth / 2, screenPosition.y - 1 - guageHeight,
					  guageWidth + 2, guageHeight + 2,
					  0, 0,
					  static_cast<float>(guage->GetTextureWidth()),
					  static_cast<float>(guage->GetTextureHeight()),
					  0,
					  0, 0, 0, 1);

		shader->Draw(rc, guage);


		guage->Update(screenPosition.x - guageWidth / 2,
					  screenPosition.y - guageHeight,
					  guageWidth * healthRate, guageHeight,
					  0, 0,
					  static_cast<float>(guage->GetTextureWidth()),
					  static_cast<float>(guage->GetTextureHeight()),
					  0,
					  1, 0, 0, 1);
		shader->Draw(rc, guage);
		shader->End(rc);

	}

}

