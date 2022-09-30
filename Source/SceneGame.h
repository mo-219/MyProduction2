#pragma once

#include "Scene.h"
#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Light.h"

#include "Graphics/Texture.h"


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

	std::unique_ptr<Sprite>	sprite;
	std::unique_ptr<Texture> texture;

};
