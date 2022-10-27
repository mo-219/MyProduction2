#pragma once

#include <DirectXMath.h>
#include "Graphics/Model.h"

// ヒット結果
struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 };         // 例とポリゴンの交点
    DirectX::XMFLOAT3 normal = { 0,0,0 };           // 衝突したポリゴンの法線ベクトル
    float             distance = 0.0f;              // レイの視点から交点までの距離
    int               materialIndex = -1;           // 衝突したポリゴンのマテリアル番号

    DirectX::XMFLOAT3 rotation = { 0,0,0 };         // 1フレームにおける回転量(差分)

};


// コリジョン
class Collision
{
public:
    // 球と球の交差判定
    static bool InstersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB);

    // 円柱と円柱の交差判定
    static bool InstersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB);

    // 球と円柱の交差判定
    static bool InstersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPositionB,
        float cylinderRadiusB,
        float cylinderHeightB,
        DirectX::XMFLOAT3& outCylinderPositionB);

    // レイとモデルの交差判定
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
