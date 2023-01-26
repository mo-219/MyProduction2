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

// �V���h�E�}�b�v�̃T�C�Y
static const UINT SHADOWMAP_SIZE = 2048;

// ������
void SceneGame::Initialize()
{

	timer = 0;



	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();


	//StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	//stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	//stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	//stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	//stageMoveFloor->setStageNum(StageNumber::Movefloor);
	//stageManager.Register(stageMoveFloor);



	// �v���C���[������
	player = new Player();

	// �Q�[�W�X�v���C�g
	guage = new Sprite();


	// ��ʑJ�ڗp
	fade = new RectFade();
	
	fade->SetAll(0, 0, 1280, 720, 1280, 720, 0, 0, 0, 1);

	HPBar = new RectBar();
	HPBar->SetAll(30, 10, 300, 20, 300, 20, 1, 0, 0, 1);
	HPBar->SetBaseAll(3, 3, 0, 0, 0, 1);

	APBar = new RectBar();
	APBar->SetAll(30, 50, 300, 20, 300, 20, 0, 1, 1, 1);
	APBar->SetBaseAll(3, 3, 0, 0, 0, 1);



	// ���x���}�l�[�W���[������
	LevelManager::Instance().Initialize(all_script);


	// �J���������ݒ�
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
		// �e�N�X�`����ǂݍ���
		texture = std::make_unique<Texture>("Data/Texture/Title.png");

		// �X�v���C�g
		sprite = std::make_unique<Sprite>();
		sprite->SetShaderResourceView(texture->GetShaderResourceView(), texture->GetWidth(), texture->GetHeight());

		// �}�X�N�e�N�X�`���̓ǂݍ���
		maskTexture = std::make_unique<Texture>("Data/Texture/dissolve_animation.png");
		dissolveThreshold = 0.0f;
		edgeThreshold = 0.2f; 			// ����臒l
		edgeColor = { 1, 0, 0, 1 };		// ���̐F




#endif
		// �V�����`��^�[�Q�b�g�̐���
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
	// �V���h�E�}�b�v�p�ɐ[�x�X�e���V���̐���
	{
		Graphics& graphics = Graphics::Instance();
		shadowmapDepthStencil = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		depthStencil = std::make_unique<DepthStencil>(renderTarget->GetWidth(), renderTarget->GetHeight());
	}

	// �|�X�g�v���Z�X�`��N���X����
	{
		postprocessingRenderer = std::make_unique<PostprocessingRenderer>();

		// �V�[���e�N�X�`���ݒ�
		ShaderResourceViewData srvData;

		srvData.srv = renderTarget->GetShaderResourceView();
		srvData.width = renderTarget->GetWidth();
		srvData.height = renderTarget->GetHeight();
		postprocessingRenderer->SetSceneData(srvData);
	}


	// �J�����R���g���[���[������
	cameraController = new CameraController();

	//Finalize();
	//exit(0);

	LoadObj load;

	load.Load("Data/Stage/SimpleNaturePack/GameObjectData.json");
	load.Create(stage1_mapData);


	EnemyManager& enemyManager = EnemyManager::Instance();
	meta = new Meta(player, &enemyManager);


}

// �I����
void SceneGame::Finalize()
{
	if (meta != nullptr)
	{
		delete meta;
		meta = nullptr;
	}


	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();

	ObjectManager::Instance().Clear();
	LevelManager::Instance().Clear();
	// �J�����R���g���[���[�I����
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
	// �Q�[�W�X�v���C�g�I����
	if (guage != nullptr)
	{
		delete guage;
		guage = nullptr;
	}

	// �v���C���[�I����
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	LightManager::Instance().Clear();
	StageManager::Instance().Clear();
}

// �X�V����
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
		// �X�e�[�W�̏�����
		levelManager.Initialize(currentStageNum);
		levelManager.Update(elapsedTime);	// ���X�|���ʒu�擾
		player->Initialize(levelManager.GetRespawnPos());
		player->SetMaxPos(levelManager.GetMaxPos());
		player->SetMinPos(levelManager.GetMinPos());

		timer = 0;

		// �h�A����
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
		// �t�F�[�h�A�E�g

		if (fade->fadeOut(0.01f))
		{
			state = State::UPDATE;
		}

		stageManager.Update(elapsedTime);

		player->UpdateOnlyTransform(elapsedTime);

		// �J�����R���g���[���[�X�V����
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->UpdateOnlyTransform(elapsedTime);

		//
		objectManager.Update(elapsedTime);
		//

		enemyManager.UpdateOnlyTransform(elapsedTime);

		// �G�t�F�N�g�X�V����
		effctManager.Update(elapsedTime);
		break;

	case State::UPDATE:
		// �X�V����
		if (player->GetStageClearFlag())
		{
			state = State::END;
			stageManager.StageNumDelete(StageNumber::Door);

			break;
		}
		stageManager.Update(elapsedTime);
		levelManager.Update(elapsedTime);

		player->Update(elapsedTime);

		// �J�����R���g���[���[�X�V����
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->Update(elapsedTime);

		enemyManager.Update(elapsedTime);
		//
		objectManager.Update(elapsedTime);
		//

		// �G�t�F�N�g�X�V����
		effctManager.Update(elapsedTime);

		break;

	case State::FADEIN:
		// �t�F�[�h�C��
		if (fade->fadeIn(0.01f))
		{
			state = State::END;
		}

		stageManager.Update(elapsedTime);
		//
		objectManager.Update(elapsedTime);
		//
		player->UpdateOnlyTransform(elapsedTime);

		// �J�����R���g���[���[�X�V����
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->UpdateOnlyTransform(elapsedTime);


		enemyManager.UpdateOnlyTransform(elapsedTime);

		// �G�t�F�N�g�X�V����
		effctManager.Update(elapsedTime);
		break;

	case State::END:
		// �X�e�[�W���I�������
		enemyManager.Clear();	//�ꉞ�S�������Ƃ�
		

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




// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// �V���h�E�}�b�v�̕`��
	RenderShadowmap();

	// 3D��Ԃ̕`���ʃo�b�t�@�ɑ΂��čs��
	Render3DScene();


	// �`�����ݐ���o�b�N�o�b�t�@�ɕς��ăI�t�X�N���[�������_�����O�̌��ʂ�`�悷��
	{
		ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
		ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

		// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0�`1.0)
		dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		dc->OMSetRenderTargets(1, &rtv, dsv);

		// �r���[�|�[�g�̐ݒ�
		D3D11_VIEWPORT	vp = {};
		vp.Width = renderTarget->GetWidth();
		vp.Height = renderTarget->GetHeight();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		dc->RSSetViewports(1, &vp);

		//RenderContext rc;
		//rc.deviceContext = dc;
		//rc.BlurCount 
		// �|�X�g�v���Z�X����

		postprocessingRenderer->SetRenderPosition(renderPosition);
		postprocessingRenderer->Render(dc);
		//postprocessingRenderer->Render(&rc);


	}


	// 3D�f�o�b�O�`��
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

	// 2D�X�v���C�g�`��
	{
		RenderContext rc;
		rc.deviceContext = dc;

		// ��ʑJ�ڗp
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

	// 2D�f�o�b�OGUI�`��
	{
		// �v���C���[�f�o�b�O�`��
		

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


	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);


	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp = {};
	vp.Width = renderTarget->GetWidth();
	vp.Height = renderTarget->GetHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);


	// �`�揈��
	RenderContext rc;
	rc.deviceContext = dc;

	// ���C�g�̏����l�ߍ���
	LightManager::Instance().PushRenderContext(rc);


	// �V���h�E�}�b�v�̐ݒ�
	rc.shadowmapData.shadowMap = shadowmapDepthStencil->GetShaderResourceView().Get();
	rc.shadowmapData.lightViewProjection = lightViewProjection;
	rc.shadowmapData.shadowColor = shadowColor;
	rc.shadowmapData.shadowBias = shadowBias;

	rc.shadowBlurData.textureSize.x = static_cast<float>(shadowmapDepthStencil->GetWidth());
	rc.shadowBlurData.textureSize.y = static_cast<float>(shadowmapDepthStencil->GetHeight());
	rc.shadowBlurData.kernelSize =	  shadowKernelSize;
	rc.shadowBlurData.deviation =     shadowDeviation;


	// �J�����p�����[�^
	Camera& camera = Camera::Instance();
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();



	// 3D���f���`��
	{
		ModelShader* shader = graphics.GetShader(id);

		shader->Begin(rc);

		// �X�e�[�W�`��
		StageManager::Instance().Render(rc,shader);
		EnemyManager::Instance().Render(rc, shader);
		ObjectManager::Instance().Render(rc, shader);
		player->Render(rc, shader);
		shader->End(rc);
	}



	//3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	//3D2DUI
	{
		RenderEnemyGauge(rc, rc.view, rc.projection);

	}


	// �f�o�b�O�v���~�e�B�u�̕\��
	{
		// �O���b�h�`��
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();
		StageManager::Instance().DrawDebugPrimitive();
		//ObjectManager::Instance().DrawDebugPrimitive();



		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);
		// �f�o�b�O�����_���`����s
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

		// ��ʃN���A
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		if (!mainDirectionalLight)	return;

		// �����_�[�^�[�Q�b�g�ݒ�
		dc->OMSetRenderTargets(0, &rtv, dsv);

		// �r���[�|�[�g�̐ݒ�
		D3D11_VIEWPORT vp = {};
		vp.Width = static_cast<float>(shadowmapDepthStencil->GetWidth());
		vp.Height = static_cast<float>(shadowmapDepthStencil->GetHeight());
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		dc->RSSetViewports(1, &vp);

		// �`�揈��
		RenderContext rc;
		rc.deviceContext = dc;

		// �J�����p�����[�^�ݒ�
		{
			// ���s��������J�����ʒu���쐬�A���_�̈ʒu������悤�Ɏ����s��𐶐�
			DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&mainDirectionalLight->GetDirection());
			LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
			DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
				DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			// �V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
			DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowDrawRect, shadowDrawRect, 0.1f, 1000.0f);
			DirectX::XMStoreFloat4x4(&rc.view, V);
			DirectX::XMStoreFloat4x4(&rc.projection, P);
			DirectX::XMStoreFloat4x4(&lightViewProjection, V * P);

		}

		// 3D���f���`��
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
	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;

	rc.deviceContext->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();					// �P�ʍs��
	// ����XMVector3Unproject���g���̂Ȃ�΋t�s��ɂ���K�v�Ƃ�������

	// �S�Ă̓G�̓����HP�Q�[�W��\��
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();


	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		DirectX::XMFLOAT3 overHead = enemy->GetPosition();
		overHead = { overHead.x, overHead.y + enemy->GetHeight() + 0.2f, overHead.z };

		// �G�l�~�[�̓���̃��[���h���W
		DirectX::XMVECTOR worldEnemyPos = DirectX::XMLoadFloat3(&overHead);

		// ���[���h���W����X�N���[�����W�ւ̕ϊ�
		DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
			worldEnemyPos,			// ���[���h���W
			viewport.TopLeftX,		// �r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		// �r���[�|�[�g����Y�ʒu
			viewport.Width,			// �r���[�|�[�g��
			viewport.Height,		// �r���[�|�[�g����
			viewport.MinDepth,		// �[�x�l�͈̔͂�\���ŏ��l(0.0f�ł悢)
			viewport.MaxDepth,		// �[�x�l�͈̔͂�\���ő�l(1.0f�ł悢)
			Projection,				// �v���W�F�N�V�����s��
			View,					// �r���[�s��
			World					// ���[���h�s��(�P�ʍs��ł悢)
		);


		// �X�N���[�����W(XMFLOAT2)
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		// �Q�[�W�̒���
		const float guageWidth = 30.0f;
		const float guageHeight = 5.0f;

		float healthRate = (float)enemy->GetHealth() / enemy->GetMaxHealth();


		Graphics& graphics = Graphics::Instance();
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);

		shader->Begin(rc);

		// �Q�[�W�`��
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

	// �G�l�~�[�z�u����
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		return;

		// �}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetPositionX());
		screenPosition.y = static_cast<float>(mouse.GetPositionY());

		// �����̃R�[�h
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
		//// ���C�̒���
		//float neart;
		//DirectX::XMStoreFloat(&neart, Length);

		HitResult hit;

		DirectX::XMVECTOR Start = DirectX::XMVector3Unproject(
			DirectX::XMVECTOR(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 0.0f))),					// ���[���h���W
			viewport.TopLeftX,		// �r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		// �r���[�|�[�g����Y�ʒu
			viewport.Width,			// �r���[�|�[�g��
			viewport.Height,		// �r���[�|�[�g����
			viewport.MinDepth,		// �[�x�l�͈̔͂�\���ŏ��l(0.0f�ł悢)
			viewport.MaxDepth,		// �[�x�l�͈̔͂�\���ő�l(1.0f�ł悢)
			Projection,				// �v���W�F�N�V�����s��
			View,					// �r���[�s��
			World					// ���[���h�s��(�P�ʍs��ł悢)
		);		DirectX::XMVECTOR Start = DirectX::XMVector3Unproject(
			DirectX::XMVECTOR(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 0.0f))),					// ���[���h���W
			viewport.TopLeftX,		// �r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		// �r���[�|�[�g����Y�ʒu
			viewport.Width,			// �r���[�|�[�g��
			viewport.Height,		// �r���[�|�[�g����
			viewport.MinDepth,		// �[�x�l�͈̔͂�\���ŏ��l(0.0f�ł悢)
			viewport.MaxDepth,		// �[�x�l�͈̔͂�\���ő�l(1.0f�ł悢)
			Projection,				// �v���W�F�N�V�����s��
			View,					// �r���[�s��
			World					// ���[���h�s��(�P�ʍs��ł悢)
		);

		if (stage->RayCast(
			DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 0.0f),
			DirectX::XMFLOAT3(screenPosition.x, screenPosition.y, 1.0f),
			hit))
		{
			DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&hit.position);


			DirectX::XMFLOAT3 enemyPos;
			DirectX::XMStoreFloat3(&enemyPos, WorldPosition);

			EnemyManager& enemyManager = EnemyManager::Instance();	// �����������Ƃ������̂ق����ǂ�
			Enemy* enemy = new EnemySlime();
			enemy->setPosition(enemyPos);
			enemyManager.Register(enemy);

		}
#endif

		// �͔͉�
		DirectX::XMVECTOR ScreenPosition, WorldPosition;
		DirectX::XMFLOAT3 rayStart, rayEnd;

		// ���_�̎Z�o
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		ScreenPosition = DirectX::XMVector3Unproject(
			ScreenPosition,			// ���[���h���W
			viewport.TopLeftX,		// �r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		// �r���[�|�[�g����Y�ʒu
			viewport.Width,			// �r���[�|�[�g��
			viewport.Height,		// �r���[�|�[�g����
			viewport.MinDepth,		// �[�x�l�͈̔͂�\���ŏ��l(0.0f�ł悢)
			viewport.MaxDepth,		// �[�x�l�͈̔͂�\���ő�l(1.0f�ł悢)
			Projection,				// �v���W�F�N�V�����s��
			View,					// �r���[�s��
			World					// ���[���h�s��(�P�ʍs��ł悢)
		);

		DirectX::XMStoreFloat3(&rayStart, ScreenPosition);

		// �I�_�̎Z�o
		screenPosition.z = 1.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		ScreenPosition = DirectX::XMVector3Unproject(
			ScreenPosition,			// ���[���h���W
			viewport.TopLeftX,		// �r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		// �r���[�|�[�g����Y�ʒu
			viewport.Width,			// �r���[�|�[�g��
			viewport.Height,		// �r���[�|�[�g����
			viewport.MinDepth,		// �[�x�l�͈̔͂�\���ŏ��l(0.0f�ł悢)
			viewport.MaxDepth,		// �[�x�l�͈̔͂�\���ő�l(1.0f�ł悢)
			Projection,				// �v���W�F�N�V�����s��
			View,					// �r���[�s��
			World					// ���[���h�s��(�P�ʍs��ł悢)
		);

		DirectX::XMStoreFloat3(&rayEnd, ScreenPosition);

		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			// �ǂ����ɓ���������

			//EnemyManager& enemyManager = EnemyManager::Instance();	// �����������Ƃ������̂ق����ǂ�
			//Enemy* enemy = new EnemySlime();
			//enemy->SetPosition(hit.position);
			//enemyManager.Register(enemy);


		}
	}
#endif
}

