
//--<�C���N���[�h�t�@�C��>---------------------------
#include "SceneGame.h"

// Graphics�t�@�C��
#include "Graphics/Graphics.h"
#include "Graphics/Light.h"
#include "Graphics/font.h"

// �J�����A���͌n
#include "Camera.h"
#include "Input/Input.h"

// �f�[�^���[�f�B���O�p
#include "DataLoading.h"

// �}�l�[�W���[�N���X
#include "EnemyManager.h"
#include "EffectManager.h"
#include "StageManager.h"
#include "GameObjectManager.h" 
#include "LevelManager.h"
#include "LightManager.h"

#include "StageDoor.h"


// �V�[���J�ڗp
#include "SceneManager.h"
#include "SceneTitle.h"

//----------------------------------------------


// �V���h�E�}�b�v�̃T�C�Y
static const UINT SHADOWMAP_SIZE = 2048;

// ������
void SceneGame::Initialize()
{

	timer = 0;

	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();

	// �v���C���[������
	player = new Player();

	// �Q�[�W�X�v���C�g
	guage = new Sprite();


	// ��ʑJ�ڗp
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


	// ���x���}�l�[�W���[������
	LevelManager::Instance().Initialize(all_script);


	// �J���������ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(DirectX::XMFLOAT3(0, 10, -10), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	camera.SetPerspectiveFov(DirectX::XMConvertToRadians(45),
							 graphics.GetScreenWidth() / graphics.GetScreenHeight(),
							 0.1f, 1000.0f);

	{

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

	// UI�擾
	waveUI.wave.text = "Wave ";
	waveUI.wave.position = { graphics.GetScreenWidth() / 2, graphics.GetScreenHeight() / 2 };
	waveUI.wave.scale = { 4,4 };
	waveUI.wave.color = { 1.0f, 0.1f, 0.4f, 0.0f};

	waveUI.enemy.text = "EnemyCount: ";
	waveUI.enemy.position = { waveUI.wave.position.x, waveUI.wave.position.y + 60 };
	waveUI.enemy.scale = { 2.5f, 2.5f };
	waveUI.enemy.color = { 0.0f, 0.0f, 0.0f, 0.0f };

	// UI�p�X�v���C�g������
	manualTexture = std::make_unique<Texture>("Data/Sprite/Manual_tenzi.png");
	manualSprite = std::make_unique<Sprite>();
	manualSprite->SetShaderResourceView(manualTexture->GetShaderResourceView(), manualTexture->GetWidth(), manualTexture->GetHeight());

	HPIconTexture = std::make_unique<Texture>("Data/Sprite/HP.png");
	HPIconSprite = std::make_unique<Sprite>();
	HPIconSprite->SetShaderResourceView(HPIconTexture->GetShaderResourceView(), HPIconTexture->GetWidth(), HPIconTexture->GetHeight());

	APIconTexture = std::make_unique<Texture>("Data/Sprite/AP.png");
	APIconSprite = std::make_unique<Sprite>();
	APIconSprite->SetShaderResourceView(APIconTexture->GetShaderResourceView(), APIconTexture->GetWidth(), APIconTexture->GetHeight());



	// �J�����R���g���[���[������
	cameraController = new CameraController();



	// �X�e�[�W�f�[�^�ǂݍ���
	LoadObj load;
	load.Load("Data/Stage/SimpleNaturePack/GameObjectData.json");
	load.Create(stage1_mapData);


	// ���^AI�擾
	EnemyManager& enemyManager = EnemyManager::Instance();
	meta = new Meta(player, &enemyManager);


	// Audio ���[�f�B���O
	Audio& audio = Audio::Instance();
	bgm = audio.LoadAudioSource("Data/Audio/BGM/Game.wav");
	se = audio.LoadAudioSource("Data/Audio/SE/Ashioto.wav");

	bgm->Play(true);
}

// �I����
void SceneGame::Finalize()
{
	// ���^AI�I����
	if (meta != nullptr)
	{
		delete meta;
		meta = nullptr;
	}

	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();

	// �X�e�[�W�I�u�W�F�N�g�I����
	ObjectManager::Instance().Clear();

	// ���x���}�l�[�W���[�I����
	LevelManager::Instance().Clear();

	// �J�����R���g���[���[�I����
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
		// ������

		// �X�e�[�W�̏�����
		levelManager.Initialize(currentStageNum);
		levelManager.Update(elapsedTime);	// ���X�|���ʒu�擾


		// UI�������ʒu�ɖ߂�
		waveUI.wave.position = { Graphics::Instance().GetScreenWidth() / 2, Graphics::Instance().GetScreenHeight() / 2 };
		waveUI.wave.color.w = 0;
		waveUI.waveCount++;

		waveUI.enemy.position = { waveUI.wave.position.x, waveUI.wave.position.y + 60 };
		waveUI.enemy.color.w = 0;
		waveUI.enemyCounts = levelManager.GetEnemyCounts();

		waveUI.state = 0;


		// �v���C���[�̏�����
		player->Initialize(levelManager.GetRespawnPos());
		player->SetMaxPos(levelManager.GetMaxPos());
		player->SetMinPos(levelManager.GetMinPos());
		player->SetIsInputMove(false);

		timer = 0;


		// �|�����G�̐��̏�����
		enemyManager.SetCurrentEnemyCount(0);


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
			player->SetIsInputMove(true);
		}

		stageManager.Update(elapsedTime);

		// �J�����R���g���[���[�X�V����
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->UpdateOnlyTransform(elapsedTime);

		
		objectManager.Update(elapsedTime);
		

		enemyManager.UpdateOnlyTransform(elapsedTime);

		// �G�t�F�N�g�X�V����
		effctManager.Update(elapsedTime);
		break;

	case State::UPDATE:
		UiUpdate(elapsedTime);

		// �X�V����
		if (player->GetStageClearFlag())
		{
			// �N���A������
			state = State::END;
			stageManager.StageNumDelete(StageNumber::Door);
			player->SetIsInputMove(false);
			se->Play(false);
			break;
		}
		if (!player->IsLive())
		{
			// �v���C���[�����񂾂�
			state = State::RESULT;
			break;
		}
		stageManager.Update(elapsedTime);
		levelManager.Update(elapsedTime);


		// �J�����R���g���[���[�X�V����
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);
		cameraController->Update(elapsedTime);


		// ���̑��X�V����
		enemyManager.Update(elapsedTime);
		waveUI.currentEnemyCount = enemyManager.GetCurrentEnemyCount();
		objectManager.Update(elapsedTime);

		effctManager.Update(elapsedTime);

		break;
	case State::FADEIN:
		// �t�F�[�h�C��
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

	case State::RESULT:
		// ���ʕ\��

		result->color.w += 0.01f;
		if (result->color.w >= 0.8f)	result->color.w = 0.8f;
		waveUI.enemy.color.w -= 0.03f;
		waveUI.wave.position.y += 2.0f;
		if (waveUI.wave.position.y >= 300)
		{
			// ��������{�^���������ꂽ��
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

// UI�X�V����
void SceneGame::UiUpdate(float elapsedTime)
{
	switch (waveUI.state)
	{
	case 0:
		// �t�F�[�h
		waveUI.wave.color.w += 0.1f;
		waveUI.enemy.color.w += 0.1f;
		if (waveUI.wave.color.w >= 1)
		{
			waveUI.state++;
		}
		break;

	case 1:
		// ��~
		waveUI.timer++;
		if (waveUI.timer >= 100)
		{
			waveUI.state++;
		}
		break;

	case 2:
		//��ɏオ���Ă���
		waveUI.wave.position.y -= 2.0f;
		waveUI.enemy.position.y -= 2.0f;
		if (waveUI.wave.position.y <= 60)
		{
			waveUI.wave.position.y = 60;
			waveUI.state++;
		}
		break;

	case 3:
		// �ʏ펞�������Ȃ�

		break;

	}

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

		// �|�X�g�v���Z�X����
		postprocessingRenderer->SetRenderPosition(renderPosition);
		postprocessingRenderer->Render(dc);
	}



	// 3D�f�o�b�O�`��
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
			// �v���C���[�f�o�b�O�`��
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

	// 2D�X�v���C�g�`��
	{
		RenderContext rc;
		rc.deviceContext = dc;

		// ��ʑJ�ڗp
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

		// ���ʔ��\�`��
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

		// �t�F�[�h�`��p
		fade->Render(rc, shader);

	}


}

// 3D���f����renderTarget�ɕ`��
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
		//player->DrawDebugPrimitive();
		//player->sword->DrawDebugPrimitive();
		//EnemyManager::Instance().DrawDebugPrimitive();
		//StageManager::Instance().DrawDebugPrimitive();
		//ObjectManager::Instance().DrawDebugPrimitive();

		//// ���C�������_���`����s
		//graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);
		//// �f�o�b�O�����_���`����s
		//graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);

	}

}

// �e�f�[�^�擾
void SceneGame::RenderShadowmap()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11DepthStencilView* dsv = shadowmapDepthStencil->GetDepthStencilView().Get();

	// ��ʃN���A
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (!mainDirectionalLight)
		return;

	// �����_�[�^�[�Q�b�g�ݒ�
	dc->OMSetRenderTargets(0, &rtv, dsv);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT	vp = {};
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
		// ���s��������J�����ʒu���쐬���A�������猴�_�̈ʒu������悤�Ɏ����s��𐶐�
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
		ObjectManager::Instance().Render(rc, shader);

		shader->End(rc);
	}
}

// �G�l�~�[�̗̓Q�[�W�`��
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

}

