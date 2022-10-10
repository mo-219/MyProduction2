#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"

#include "EnemyManager.h"
#include "EffectManager.h"
#include "EnemySlime.h"

// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化
	stage = new Stage();

	// プレイヤー初期化
	player = new Player();

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
			renderTarget = std::make_unique<RenderTarget>(static_cast<UINT>(graphics.GetScreenWidth()),
														  static_cast<UINT>(graphics.GetScreenHeight()),
														  DXGI_FORMAT_R8G8B8A8_UNORM);

			//// spriteで描画するものをシーンの描画結果に変える
			//sprite = std::make_unique<Sprite>();
			//sprite->SetShaderResourceView(renderTarget->GetShaderResourceView(),
			//							  renderTarget->GetWidth(), 
			//							  renderTarget->GetHeight());
		}


		//// ガウスブラー用スプライト生成
		//{
		//	// テクスチャを読み込む
		//	gaussianBlurTexture = std::make_unique<Texture>("Data/Texture/1920px-Equirectangular-projection.jpg");
		//	gaussianBlurSprite = std::make_unique<Sprite>();
		//	gaussianBlurSprite->SetShaderResourceView(gaussianBlurTexture->GetShaderResourceView(), gaussianBlurTexture->GetWidth(),
		//		gaussianBlurTexture->GetHeight());
		//}
		

		// 平行光源を追加
		directional_light = std::make_unique<Light>(LightType::Directional);
		ambientLightColor = { 0.2f, 0.2f, 0.2f, 0.2f };
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

	// エネミー初期化
	//EnemyManager::Instance().Register(new EnemySlime());
	EnemyManager& enemyManager = EnemyManager::Instance();	// 先を見据えるとこっちのほうが良い
	Enemy* enemy = new EnemySlime();
	enemy->setPosition(DirectX::XMFLOAT3{ 5, 0, 5 });
	enemyManager.Register(enemy);

	enemy = new EnemySlime();
	enemy->setPosition(DirectX::XMFLOAT3{ 7, 0, 5 });
	enemyManager.Register(enemy);


}

// 終了化
void SceneGame::Finalize()
{
	// エネミー終了化
	EnemyManager::Instance().Clear();

	// カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}


	// プレイヤー終了化
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// ステージ終了化
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	stage->Update(elapsedTime);

	player->Update(elapsedTime);

	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	//gaussianBlurSprite->Update(0.0f, 0.0f,
	//	Graphics::Instance().GetScreenWidth(), Graphics::Instance().GetScreenHeight(),
	//	0.0f, 0.0f,
	//	static_cast<float>(gaussianBlurTexture->GetWidth()), static_cast<float>(gaussianBlurTexture->GetHeight()),
	//	0.0f,
	//	1.0f, 1.0f, 1.0f, 1.0f);

	//sprite->Update(0.0f, 0.0f,
	//	Graphics::Instance().GetScreenWidth(), Graphics::Instance().GetScreenHeight(),
	//	0.0f, 0.0f,
	//	static_cast<float>(renderTarget->GetWidth()), static_cast<float>(renderTarget->GetHeight()),
	//	0.0f,
	//	1.0f, 1.0f, 1.0f, 1.0f);


}

#if 0
// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.deviceContext = dc;
	rc.ambientLightColor = ambientLightColor;
	rc.directionalLightData.direction.x = directional_light->GetDirection().x;
	rc.directionalLightData.direction.y = directional_light->GetDirection().y;
	rc.directionalLightData.direction.z = directional_light->GetDirection().z;
	rc.directionalLightData.direction.w = 0;
	rc.directionalLightData.color = directional_light->GetColor();

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	//rc.view = camera.GetView();
	//rc.projection = camera.GetProjection();

	// 01までの処理													
#if 0
	// ビュー行列
	{
		DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// カメラの視点（位置）
		DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// カメラの注視点（ターゲット）
		DirectX::XMFLOAT3 up = { 0, 1, 0 };		// カメラの上方向

		DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
		DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
		DirectX::XMStoreFloat4x4(&rc.view, View);
	}
	// プロジェクション行列
	{
		float fovY = DirectX::XMConvertToRadians(45);	// 視野角
		float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// 画面縦横比率
		float nearZ = 0.1f;	// カメラが映し出すの最近距離
		float farZ = 1000.0f;	// カメラが映し出すの最遠距離
		DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
		DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	}	 
#endif	

	// 3Dモデル描画
	{
		ModelShader* shader = graphics.GetShader(ModelShaderId::Phong);
		shader->Begin(rc);

		// ステージ描画
		stage->Render(rc, shader);
		player->Render(rc,shader);
		EnemyManager::Instance().Render(rc, shader);

		shader->End(rc);
	}

	//3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		// プレイヤーデバッグプリミティブ描画
		//graphics.GetDebugRenderer()->DrawSphere(player->GetPosition(), player->GetRadius(), { 1,0,0,1 });
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);

		if (ImGui::TreeNode("Mask"))
		{
			ImGui::SliderFloat("Dissolve Threshold", &dissolveThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("Edge Threshold", &edgeThreshold, 0.0f, 1.0f);
			ImGui::ColorEdit4("Edge Color", &edgeColor.x);

			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNode("GaussianFilter"))
		{
			ImGui::SliderInt("kernelSize", &gaussianFilterData.kernelSize, 1, MaxKernelSize - 1);
			ImGui::SliderFloat("deviation", &gaussianFilterData.deviation, 1.0f, 10.0f);
			ImGui::TreePop();
		}

	}

	// 2Dスプライト描画
	{
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);
		RenderContext rc;

		rc.deviceContext = dc;
		rc.gaussianFilterData = gaussianFilterData;
		rc.gaussianFilterData.textureSize.x = texture->GetWidth();
		rc.gaussianFilterData.textureSize.y = texture->GetHeight();
		rc.gaussianFilterData.kernelSize = gaussianFilterData.kernelSize;

		//shader->Begin(rc);
		//shader->Draw(rc, gaussianBlurSprite.get());
		//shader->End(rc);

	}

	// 2DデバッグGUI描画
	{
		// プレイヤーデバッグ描画
		player->DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}
#endif
// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// 3D空間の描画を別バッファに対して行う
	Render3DScene();


	// 描き込み先をバックバッファに変えてオフスクリーンレンダリングの結果を描画する
	{
#if 0
		ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
		ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

		// 画面クリア＆レンダーターゲット設定
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0～1.0)
		dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, dsv);


		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);
		RenderContext rc;

		rc.deviceContext = dc;
		//rc.gaussianFilterData = gaussianFilterData;
		//rc.gaussianFilterData.textureSize.x = gaussianBlurTexture->GetWidth();
		//rc.gaussianFilterData.textureSize.y = gaussianBlurTexture->GetHeight();
		//rc.gaussianFilterData.kernelSize = gaussianFilterData.kernelSize;

		shader->Begin(rc);

		shader->Draw(rc, sprite.get());

		shader->End(rc);
#endif
		ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
		ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

		// 画面クリア＆レンダーターゲット設定
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0～1.0)
		dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, dsv);

		// ポストプロセス処理
		postprocessingRenderer->Render(dc);
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


	}

	// 2Dスプライト描画
	{
#if 0
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::LuminanceExtraction);
		RenderContext rc;

		rc.deviceContext = dc;
		rc.gaussianFilterData = gaussianFilterData;
		rc.gaussianFilterData.textureSize.x = gaussianBlurTexture->GetWidth();
		rc.gaussianFilterData.textureSize.y = gaussianBlurTexture->GetHeight();
		rc.gaussianFilterData.kernelSize = gaussianFilterData.kernelSize;
		rc.luminanceExtractionData = luminanceExtractionData;

		shader->Begin(rc);
		shader->Draw(rc, gaussianBlurSprite.get());
		shader->End(rc);
#endif

	}

	// 2DデバッグGUI描画
	{
		// プレイヤーデバッグ描画
		player->DrawDebugGUI();
		cameraController->DrawDebugGUI();
		postprocessingRenderer->DrawDebugGUI();
	}
}

void SceneGame::Render3DScene()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = renderTarget->GetRenderTargetView().Get();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();


	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);


	// ビューポート設定
	D3D11_VIEWPORT vp = {};
	vp.Width = graphics.GetScreenWidth();
	vp.Height = graphics.GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);


	// 描画処理
	RenderContext rc;
	rc.deviceContext = dc;

	// ライトの情報を詰め込む
	//LightManager::Instance().PushRenderContext(rc);
	rc.ambientLightColor = ambientLightColor;
	rc.directionalLightData.direction.x = directional_light->GetDirection().x;
	rc.directionalLightData.direction.y = directional_light->GetDirection().y;
	rc.directionalLightData.direction.z = directional_light->GetDirection().z;
	rc.directionalLightData.direction.w = 0;
	rc.directionalLightData.color = directional_light->GetColor();

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
		ModelShader* shader = graphics.GetShader(ModelShaderId::Phong);

		shader->Begin(rc);

		// ステージ描画
		stage->Render(rc, shader);
		player->Render(rc, shader);
		EnemyManager::Instance().Render(rc, shader);

		shader->End(rc);
	}


	//3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}


	// デバッグプリミティブの表示
	{
		// グリッド描画
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);
		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

}
