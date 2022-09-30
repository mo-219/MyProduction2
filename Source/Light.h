#pragma once
#include <DirectXMath.h>

// �����^�C�v
enum class LightType
{
	Directional,	// ���s����
};

// �����N���X
class Light
{
public:
	Light(LightType lightType = LightType::Directional);

	// �f�o�b�O���̕\��
	void DrawDebugGUI();

	// �f�o�b�O�}�`�̕\��
	void DrawDebugPrimitive();

	// ���C�g�̌���
	DirectX::XMFLOAT3 GetDirection() { return direction; }
	void SetDirection(DirectX::XMFLOAT3 direction) { this->direction = direction; }

	// �F
	DirectX::XMFLOAT4 GetColor() { return color; }
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

private:
	LightType			lightType = LightType::Directional;		// ���C�g�^�C�v
	DirectX::XMFLOAT3	direction = DirectX::XMFLOAT3(0, -1, -1);	// ���C�g�̌���
	DirectX::XMFLOAT4	color = DirectX::XMFLOAT4(1, 1, 1, 1);	// ���C�g�̐F
};
