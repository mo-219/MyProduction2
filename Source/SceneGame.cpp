#include "Graphics/Graphics.h"

#include "SceneGame.h"
#include "Camera.h"

#include "EnemyManager.h"
#include "EffectManager.h"
#include "StageManager.h"
#include "GameObjectManager.h" 
#include "LoadObj.h"

#include "EnemySlime.h"
#include "StageMain.h"
#include "StageMoveFloor.h"
#include "StageDoor.h"
#include "ItemObj.h"


#include "Input/Input.h"

#include "LightManager.h"
#include "Graphics/Light.h"

#include "LevelManager.h"

// シャドウマップのサイズ
static const UINT SHADOWMAP_SIZE = 2048;

// 初期化
void SceneGame::Initialize()
{

	timer = 0;



	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();


	//StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	//stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	//stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	//stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	//stageMoveFloor->setStageNum(StageNumber::Movefloor);
	//stageManager.Register(stageMoveFloor);



	// プレイヤー初期化
	player = new Player();

	// ゲージスプライト
	guage = new Sprite();


	// 画面遷移用
	fade = new RectFade();
	
	fade->SetAll(0, 0, 1280, 720, 1280, 720, 0, 0, 0, 1);

	HPBar = new RectBar();
	HPBar->SetAll(30, 10, 300, 20, 300, 20, 1, 0, 0, 1);
	HPBar->SetBaseAll(3, 3, 0, 0, 0, 1);

	APBar = new RectBar();
	APBar->SetAll(30, 50, 300, 20, 300, 20, 0, 1, 1, 1);
	APBar->SetBaseAll(3, 3, 0, 0, 0, 1);



	// レベルマネージャー初期化
	LevelManager::Instance().Initialize(all_script);


	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(DirectX::XMFLOAT3(0, 10, -10), DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0));
	camera.SetPerspectiveFov(DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f);
	{
#if 0
		// テクスチャを読み込む
		texture = std::make_unique<Texture>("Data/Texture/Title.png");

		// スプライト
		sprite = std::make_unique<Sprite>();
		sprite->SetShaderResourceView(texture->GetShaderResourceView(), texture->GetWidth(), texture->GetHeight());

		// マスクテクスチャの読み込み
		maskTexture = std::make_unique<Texture>("Data/Texture/dissolve_animation.png");
		dissolveThreshold = 0.0f;
		edgeThreshold = 0.2f; 			// 縁の閾値
		edgeColor = { 1, 0, 0, 1 };		// 縁の色




#endif
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


	// カメラコントローラー初期化
	cameraController = new CameraController();

	//Finalize();
	//exit(0);

	LoadObj load;

	load.Load("Data/Stage/SimpleNaturePack/GameObjectData.json");
	load.Create(stage1_mapData);


	EnemyManager& enemyManager = EnemyManager::Instance();
	meta = new Meta(player, &enemyManager);


}

// 終了化
void SceneGame::Finalize()
{
	if (meta != nullptr)
	{
		delete meta;
		meta = nullptr;
	}


	// エネミー終了化
	EnemyManager::Instance().Clear();

	ObjectManager::Instance().Clear();
	LevelManager::Instance().Clear();
	// カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
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
		// ステージの初期化
		levelManager.Initialize(currentStageNum);
		levelManager.Update(elapsedTime);	// リスポン位置取得
		player->Initialize(levelManager.GetRespawnPos());
		player->SetMaxPos(levelManager.GetMaxPos());
		player->SetMinPos(levelManager.GetMinPos());

		timer = 0;

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
		}

		stageManager.Update(elapsedTime);

		player->UpdateOnlyTransform(elapsedTime);

		// カメラコントローラー更新処理
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->UpdateOnlyTransform(elapsedTime);

		//
		objectManager.Update(elapsedTime);
		//

		enemyManager.UpdateOnlyTransform(elapsedTime);

		// エフェクト更新処理
		effctManager.Update(elapsedTime);
		break;

	case State::UPDATE:
		// 更新処理
		if (player->GetStageClearFlag())
		{
			state = State::END;
			stageManager.StageNumDelete(StageNumber::Door);

			break;
		}
		stageManager.Update(elapsedTime);
		levelManager.Update(elapsedTime);

		player->Update(elapsedTime);

		// カメラコントローラー更新処理
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->Update(elapsedTime);

		enemyManager.Update(elapsedTime);
		//
		objectManager.Update(elapsedTime);
		//

		// エフェクト更新処理
		effctManager.Update(elapsedTime);

		break;

	case State::FADEIN:
		// フェードイン
		if (fade->fadeIn(0.01f))
		{
			state = State::END;
		}

		stageManager.Update(elapsedTime);
		//
		objectManager.Update(elapsedTime);
		//
		player->UpdateOnlyTransform(elapsedTime);

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
	}

	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	HPBar->culcValue(player->GetHealth(), player->GetMaxHealth());
	APBar->culcValue(player->GetCurrentAP(), player->GetMaxAP());

	renderPosition.y += sinf(timer * elapsedTime * 3.0f) / 2;


	timer++;
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

		//RenderContext rc;
		//rc.deviceContext = dc;
		//rc.BlurCount 
		// ポストプロセス処理

		postprocessingRenderer->SetRenderPosition(renderPosition);
		postprocessingRenderer->Render(dc);
		//postprocessingRenderer->Render(&rc);


	}


	// 3Dデバッグ描画
	{

		if (ImGui::TreeNode("Mask"))
		{
			ImGui::SliderFloat("Dissolve Threshold", &dissolveThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("Edge Threshold", &edgeThreshold, 0.0f, 1.0f);
			ImGui::ColorEdit4("Edge Color", &edgeColor.x);

			ImGui::TreePop();
		}
		ImGui::Separator();

		if (ImGui::TreeNode("Shadowmap"))
		{
			ImGui::SliderFloat("DrawRect", &shadowDrawRect, 1.0f, 2048.0f);
			ImGui::ColorEdit3("Color", &shadowColor.x);
			ImGui::SliderFloat("Bias", &shadowBias, 0.0f, 0.001f);
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

	}

	// 2Dスプライト描画
	{
		RenderContext rc;
		rc.deviceContext = dc;

		// 画面遷移用
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);

		//Rect re;
		//re.position = { DirectX::XMFLOAT2(0,0) };
		//re.size  = { DirectX::XMFLOAT2(1280,720) };
		//re.color = { DirectX::XMFLOAT4(0, 0, 0, 1) };
		//re.imageSize = { DirectX::XMFLOAT2(1280,720) };
		HPBar->Render(rc, shader);
		APBar->Render(rc, shader);
		fade->Render(rc, shader);


	}

	// 2DデバッグGUI描画
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
		//	ObjectManager::Instance().findNear(player->GetPosition()));
		//ObjectManager::Instance().DrawDebugGUI();

	}
}

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

	rc.shadowBlurData.textureSize.x = static_cast<float>(shadowmapDepthStencil->GetWidth());
	rc.shadowBlurData.textureSize.y = static_cast<float>(shadowmapDepthStencil->GetHeight());
	rc.shadowBlurData.kernelSize =	  shadowKernelSize;
	rc.shadowBlurData.deviation =     shadowDeviation;


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
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();
		StageManager::Instance().DrawDebugPrimitive();
		//ObjectManager::Instance().DrawDebugPrimitive();



		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);
		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);

	}

}

void SceneGame::RenderShadowmap()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	{
		ID3D11RenderTargetView* rtv = nullptr;
		ID3D11DepthStencilView* dsv = shadowmapDepthStencil->GetDepthStencilView().Get();

		// 画面クリア
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		if (!mainDirectionalLight)	return;

		// レンダーターゲット設定
		dc->OMSetRenderTargets(0, &rtv, dsv);

		// ビューポートの設定
		D3D11_VIEWPORT vp = {};
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
			// 平行光源からカメラ位置を作成、原点の位置を見るように視線行列を生成
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
			//ObjectManager::Instance().Render(rc, shader);

			shader->End(rc);
		}
	}

}

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

#if 0

	// エネミー配置処理
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		return;

		// マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetPositionX());
		screenPosition.y = static_cast<float>(mouse.GetPositionY());

		// 自分のコード
#if 0
		//DirectX::XMVECTOR Near = 
		//	DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 0.0f));
		//DirectX::XMVECTOR Far = 
		//	DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 1.0f));
		//DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(Near, World);
		//DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(Far, World);
		//DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		//DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		//DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);
		//// レイの長さ
		//float neart;
		//DirectX::XMStoreFloat(&neart, Length);

		HitResult hit;

		DirectX::XMVECTOR Start = DirectX::XMVector3Unproject(
			DirectX::XMVECTOR(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 0.0f))),					// ワールド座標
			viewport.TopLeftX,		// ビューポート左上X位置
			viewport.TopLeftY,		// ビューポート左上Y位置
			viewport.Width,			// ビューポート幅
			viewport.Height,		// ビューポート高さ
			viewport.MinDepth,		// 深度値の範囲を表す最小値(0.0fでよい)
			viewport.MaxDepth,		// 深度値の範囲を表す最大値(1.0fでよい)
			Projection,				// プロジェクション行列
			View,					// ビュー行列
			World					// ワールド行列(単位行列でよい)
		);		DirectX::XMVECTOR Start = DirectX::XMVector3Unproject(
			DirectX::XMVECTOR(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 0.0f))),					// ワールド座標
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

		if (stage->RayCast(
			DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 0.0f),
			DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 1.0f),
			hit))
		{
			DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&hit.position);


			DirectX::XMFLOAT3 enemyPos;
			DirectX::XMStoreFloat3(&enemyPos, WorldPosition);

			EnemyManager& enemyManager = EnemyManager::Instance();	// 先を見据えるとこっちのほうが良い
			Enemy* enemy = new EnemySlime();
			enemy->setPosition(enemyPos);
			enemyManager.Register(enemy);

		}
#endif

		// 模範解答
		DirectX::XMVECTOR ScreenPosition, WorldPosition;
		DirectX::XMFLOAT3 rayStart, rayEnd;

		// 視点の算出
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		ScreenPosition = DirectX::XMVector3Unproject(
			ScreenPosition,			// ワールド座標
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

		DirectX::XMStoreFloat3(&rayStart, ScreenPosition);

		// 終点の算出
		screenPosition.z = 1.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		ScreenPosition = DirectX::XMVector3Unproject(
			ScreenPosition,			// ワールド座標
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

		DirectX::XMStoreFloat3(&rayEnd, ScreenPosition);

		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			// どこかに当たったら

			//EnemyManager& enemyManager = EnemyManager::Instance();	// 先を見据えるとこっちのほうが良い
			//Enemy* enemy = new EnemySlime();
			//enemy->SetPosition(hit.position);
			//enemyManager.Register(enemy);


		}
	}
#endif
}

