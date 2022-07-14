#include "Camera.h"

// �w�����������
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus,
    const DirectX::XMFLOAT3& up)
{
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);		// LH:LeftHand�̗��́@����n�̂��Ƃ��w��
	// ���[���Ȃ񂽂�͕��s���e
	DirectX::XMStoreFloat4x4(&view, View);

	// �r���[���t�s�񉻂��A���[���h�s��ɒ���
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;

	DirectX::XMStoreFloat4x4(&world, World);

	// �J�����̕��������o��
	this->right.x = view._11;
	this->right.y = view._12;
	this->right.z = view._13;

	this->up.x = view._21;
	this->up.y = view._22;
	this->up.z = view._23;

	this->front.x = view._31;
	this->front.y = view._32;
	this->front.z = view._33;
	
//	this->eye = DirectX::XMStoreFloat3(&(this->eye), Eye);
	DirectX::XMStoreFloat3(&(this->eye), Eye);
	DirectX::XMStoreFloat3(&(this->focus), Focus);

	
}

// �p�[�X�y�N�e�B�u�ݒ�
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	// ��p�A��ʔ䗦�A�N���b�v��������v���W�F�N�V�����s����쐬
	DirectX::XMMATRIX Projection =  DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}