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



// �Q�[���V�[��
class SceneGame : public  Scene
{
public:
	//SceneGame() {}
	~SceneGame() override {}

	// ������
	void Initialize()override;

	// �I����
	void Finalize()override;

	// �X�V����
	void Update(float elapsedTime)override;

	// �`�揈��
	void Render()override;

	// 3D��Ԃ̕`��
	void Render3DScene();

	// �V���h�E�p���
	void RenderShadowmap();

private:
	Stage* stage = nullptr;
	Player* player = nullptr;

	CameraController* cameraController = nullptr;

	// ���s�����f�[�^
	DirectX::XMFLOAT4			ambientLightColor;
	std::unique_ptr<Light>		directional_light;


	// 2D�f�[�^�p�ϐ�
	// UV�X�N���[���f�[�^
	UVScrollData			uvScrollData;

	// �}�X�N�摜
	std::unique_ptr<Texture>	maskTexture;
	float						dissolveThreshold;
	float						edgeThreshold; 	// ����臒l
	DirectX::XMFLOAT4			edgeColor;		// ���̐F

	std::unique_ptr<Sprite>		sprite;
	std::unique_ptr<Texture>	texture;

	//// �K�E�X�t�B���^�[�f�[�^
	//GaussianFilterData			gaussianFilterData;
	//std::unique_ptr<Texture>	gaussianBlurTexture;
	//std::unique_ptr<Sprite>		gaussianBlurSprite;

	//// �u���[���f�[�^
	//LuminanceExtractionData luminanceExtractionData;

	
	// �V���h�E�p���
	Light* mainDirectionalLight = nullptr;
	float shadowDrawRect = 500.0f;

	DirectX::XMFLOAT4X4 lightViewProjection;			  // ���C�g�r���[�v���W�F�N�V�����s��
	DirectX::XMFLOAT3 shadowColor = { 0.1f, 0.1f, 0.1f}; // �e�̐F
	float shadowBias = 0.0003f;

	std::unique_ptr<DepthStencil>	shadowmapDepthStencil;	// �V���h�E�p�[�x�X�e���V��

	// �I�t�X�N���[�������_�����O�p�`��o�b�t�@
	std::unique_ptr<RenderTarget> renderTarget;


	// �|�X�g�v���Z�X
	std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;

	ModelShaderId id = ModelShaderId::Cubic;
};
