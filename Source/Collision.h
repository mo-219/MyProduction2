#pragma once

#include <DirectXMath.h>
#include "Graphics/Model.h"

// �q�b�g����
struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 };         // ��ƃ|���S���̌�_
    DirectX::XMFLOAT3 normal = { 0,0,0 };           // �Փ˂����|���S���̖@���x�N�g��
    float             distance = 0.0f;              // ���C�̎��_�����_�܂ł̋���
    int               materialIndex = -1;           // �Փ˂����|���S���̃}�e���A���ԍ�

    DirectX::XMFLOAT3 rotation = { 0,0,0 };         // 1�t���[���ɂ������]��(����)

};


// �R���W����
class Collision
{
public:
    // ���Ƌ��̌�������
    static bool InstersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB);

    // �~���Ɖ~���̌�������
    static bool InstersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB);

    // ���Ɖ~���̌�������
    static bool InstersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPositionB,
        float cylinderRadiusB,
        float cylinderHeightB,
        DirectX::XMFLOAT3& outCylinderPositionB);

    // ���C�ƃ��f���̌�������
    static bool IntersectRayVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result);

    static bool InstersectCubeVsCylinder(
        const DirectX::XMFLOAT3& cubePosition,
        float width, float height, float depth,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius, float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition);

};
