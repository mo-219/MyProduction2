#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"

#include "EnemyManager.h"
#include "EffectManager.h"
#include "EnemySlime.h"

// ������
void SceneGame::Initialize()
{
	// �X�e�[�W������
	stage = new Stage();

	// �v���C���[������
	player = new Player();

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
			renderTarget = std::make_unique<RenderTarget>(static_cast<UINT>(graphics.GetScreenWidth()),
														  static_cast<UINT>(graphics.GetScreenHeight()),
														  DXGI_FORMAT_R8G8B8A8_UNORM);

			//// sprite�ŕ`�悷����̂��V�[���̕`�挋�ʂɕς���
			//sprite = std::make_unique<Sprite>();
			//sprite->SetShaderResourceView(renderTarget->GetShaderResourceView(),
			//							  renderTarget->GetWidth(), 
			//							  renderTarget->GetHeight());
		}


		//// �K�E�X�u���[�p�X�v���C�g����
		//{
		//	// �e�N�X�`����ǂݍ���
		//	gaussianBlurTexture = std::make_unique<Texture>("Data/Texture/1920px-Equirectangular-projection.jpg");
		//	gaussianBlurSprite = std::make_unique<Sprite>();
		//	gaussianBlurSprite->SetShaderResourceView(gaussianBlurTexture->GetShaderResourceView(), gaussianBlurTexture->GetWidth(),
		//		gaussianBlurTexture->GetHeight());
		//}
		

		// ���s������ǉ�
		directional_light = std::make_unique<Light>(LightType::Directional);
		ambientLightColor = { 0.2f, 0.2f, 0.2f, 0.2f };
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

	// �G�l�~�[������
	//EnemyManager::Instance().Register(new EnemySlime());
	EnemyManager& enemyManager = EnemyManager::Instance();	// �����������Ƃ������̂ق����ǂ�
	Enemy* enemy = new EnemySlime();
	enemy->setPosition(DirectX::XMFLOAT3{ 5, 0, 5 });
	enemyManager.Register(enemy);

	enemy = new EnemySlime();
	enemy->setPosition(DirectX::XMFLOAT3{ 7, 0, 5 });
	enemyManager.Register(enemy);


}

// �I����
void SceneGame::Finalize()
{
	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();

	// �J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}


	// �v���C���[�I����
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// �X�e�[�W�I����
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	stage->Update(elapsedTime);

	player->Update(elapsedTime);

	// �J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	EnemyManager::Instance().Update(elapsedTime);

	// �G�t�F�N�g�X�V����
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
// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.ambientLightColor = ambientLightColor;
	rc.directionalLightData.direction.x = directional_light->GetDirection().x;
	rc.directionalLightData.direction.y = directional_light->GetDirection().y;
	rc.directionalLightData.direction.z = directional_light->GetDirection().z;
	rc.directionalLightData.direction.w = 0;
	rc.directionalLightData.color = directional_light->GetColor();

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	//rc.view = camera.GetView();
	//rc.projection = camera.GetProjection();

	// 01�܂ł̏���													
#if 0
	// �r���[�s��
	{
		DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// �J�����̎��_�i�ʒu�j
		DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// �J�����̒����_�i�^�[�Q�b�g�j
		DirectX::XMFLOAT3 up = { 0, 1, 0 };		// �J�����̏����

		DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
		DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
		DirectX::XMStoreFloat4x4(&rc.view, View);
	}
	// �v���W�F�N�V�����s��
	{
		float fovY = DirectX::XMConvertToRadians(45);	// ����p
		float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// ��ʏc���䗦
		float nearZ = 0.1f;	// �J�������f���o���̍ŋߋ���
		float farZ = 1000.0f;	// �J�������f���o���̍ŉ�����
		DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
		DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	}	 
#endif	

	// 3D���f���`��
	{
		ModelShader* shader = graphics.GetShader(ModelShaderId::Phong);
		shader->Begin(rc);

		// �X�e�[�W�`��
		stage->Render(rc, shader);
		player->Render(rc,shader);
		EnemyManager::Instance().Render(rc, shader);

		shader->End(rc);
	}

	//3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		// �v���C���[�f�o�b�O�v���~�e�B�u�`��
		//graphics.GetDebugRenderer()->DrawSphere(player->GetPosition(), player->GetRadius(), { 1,0,0,1 });
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
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

	// 2D�X�v���C�g�`��
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

	// 2D�f�o�b�OGUI�`��
	{
		// �v���C���[�f�o�b�O�`��
		player->DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}
#endif
// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// 3D��Ԃ̕`���ʃo�b�t�@�ɑ΂��čs��
	Render3DScene();


	// �`�����ݐ���o�b�N�o�b�t�@�ɕς��ăI�t�X�N���[�������_�����O�̌��ʂ�`�悷��
	{
#if 0
		ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
		ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

		// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0�`1.0)
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

		// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0�`1.0)
		dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, dsv);

		// �|�X�g�v���Z�X����
		postprocessingRenderer->Render(dc);
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


	}

	// 2D�X�v���C�g�`��
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

	// 2D�f�o�b�OGUI�`��
	{
		// �v���C���[�f�o�b�O�`��
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


	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);


	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp = {};
	vp.Width = graphics.GetScreenWidth();
	vp.Height = graphics.GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);


	// �`�揈��
	RenderContext rc;
	rc.deviceContext = dc;

	// ���C�g�̏����l�ߍ���
	//LightManager::Instance().PushRenderContext(rc);
	rc.ambientLightColor = ambientLightColor;
	rc.directionalLightData.direction.x = directional_light->GetDirection().x;
	rc.directionalLightData.direction.y = directional_light->GetDirection().y;
	rc.directionalLightData.direction.z = directional_light->GetDirection().z;
	rc.directionalLightData.direction.w = 0;
	rc.directionalLightData.color = directional_light->GetColor();

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
		ModelShader* shader = graphics.GetShader(ModelShaderId::Phong);

		shader->Begin(rc);

		// �X�e�[�W�`��
		stage->Render(rc, shader);
		player->Render(rc, shader);
		EnemyManager::Instance().Render(rc, shader);

		shader->End(rc);
	}


	//3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}


	// �f�o�b�O�v���~�e�B�u�̕\��
	{
		// �O���b�h�`��
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);
		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

}
