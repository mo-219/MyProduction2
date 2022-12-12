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

	// �G�l�~�[HP�Q�[�W�̕`��
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

	Rect fade;		// ��ʑJ�ڗp
	Player* player = nullptr;
	SkyBox* sky = nullptr;
	Sprite* guage = nullptr;
	State state = State::INITIALIZE;
	int currentStageNum = 0;

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


	
	// �V���h�E�p���
	Light* mainDirectionalLight = nullptr;
	float shadowDrawRect = 80.0f;

	DirectX::XMFLOAT4X4 lightViewProjection;			  // ���C�g�r���[�v���W�F�N�V�����s��
	DirectX::XMFLOAT3 shadowColor = { 0.01f, 0.0f, 1.0f}; // �e�̐F
	float shadowBias = 0.00045f;

	//std::unique_ptr<RenderTarget>	shadowmapRenderTarget;	// �V���h�E�p�[�x�X�e���V��
	std::unique_ptr<Sprite>         shadowSprite;
	std::unique_ptr<RenderTarget>   shadowBlurTarget;
	std::unique_ptr<DepthStencil>	shadowmapDepthStencil;	// �V���h�E�p�[�x�X�e���V��
	int				  shadowKernelSize = 8;		// �J�[�l���T�C�Y
	float			  shadowDeviation = 10.0f;	// �W���΍�



	// �I�t�X�N���[�������_�����O�p�`��o�b�t�@
	std::unique_ptr<RenderTarget> renderTarget;


	// �|�X�g�v���Z�X
	std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;

	ModelShaderId id = ModelShaderId::Cubic;
};
