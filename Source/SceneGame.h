#pragma once

//--<�C���N���[�h�t�@�C��>---------------------------
#include "Scene.h"

// �O���t�B�b�N�X
#include "Graphics/Light.h"
#include "Graphics/PostprocessingRenderer.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"

// �J�����A���͗p
#include "CameraController.h"

#include "Stage.h"
#include "Player.h"
#include "LevelScript.h"
#include "Rect.h"
#include "MetaAI.h"

// ���p
#include "Audio/Audio.h"

//--------------------------------------------------




// �Q�[���V�[��
class SceneGame : public  Scene
{
public:
	~SceneGame() override {}

	// ������
	void Initialize()override;

	// �I����
	void Finalize()override;

	// �X�V����
	void Update(float elapsedTime)override;

	void UiUpdate(float elapsedTime);

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

	CameraController*	cameraController = nullptr;	// �J�����R���g���[���[
	Player*				player = nullptr;			// �v���C���[�ϐ�
	Meta*				meta = nullptr;				// ���^AI�ۑ��p


//----<UI�p�ϐ�>------------------------------------

	RectFade*	fade;			// ��ʑJ�ڗp
	Rect*		result;			// ���U���g�`��p
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

//----<Update�p�ϐ�>---------------------------------------------------

	State	state = State::INITIALIZE;	// Update�p
	int		currentStageNum = 0;		// ���݂̃X�e�[�W
	bool	sceneChange = false;		// �V�[���`�F���W�p�t���O


//--------------------------------------------------------------------


//----<�O���t�B�b�N�X�p�ϐ�>------------------------------------

	// ���s�����f�[�^
	DirectX::XMFLOAT4			ambientLightColor;
	std::unique_ptr<Light>		directional_light;


//------------------------------------------------------------

	
//----<�V���h�E�p�ϐ�>--------------------------------------------------------------------------
	Light* mainDirectionalLight = nullptr;
	float shadowDrawRect = 80.0f;
	DirectX::XMFLOAT4X4 lightViewProjection;			  // ���C�g�r���[�v���W�F�N�V�����s��
	DirectX::XMFLOAT3 shadowColor = { 0.01f, 0.0f, 1.0f}; // �e�̐F
	float shadowBias = 0.00015f;
	std::unique_ptr<DepthStencil>	shadowmapDepthStencil;	// �V���h�E�p�[�x�X�e���V��
//---------------------------------------------------------------------------------------------


	// �I�t�X�N���[�������_�����O�p�`��o�b�t�@
	std::unique_ptr<RenderTarget> renderTarget;
	std::unique_ptr<DepthStencil> depthStencil;	// �V���h�E�p�[�x�X�e���V��


	// �|�X�g�v���Z�X
	std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;
	
	ModelShaderId id = ModelShaderId::Cubic;
	DirectX::XMFLOAT2 renderPosition = { 0,0 };

	WaveUI waveUI = {};


	std::unique_ptr<AudioSource> bgm;
	std::unique_ptr<AudioSource> se;

};



