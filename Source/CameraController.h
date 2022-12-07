#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
    CameraController() {}
    ~CameraController() {}

    // 更新処理
    void Update(float elapsedTime);
    void UpdateOnlyTransform(float elapsedTime);

    // ターゲット位置設定
    void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }
    
    // デバッグ用GUI描画
    void DrawDebugGUI();

private:
    DirectX::XMFLOAT3 target = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { DirectX::XMConvertToRadians(50),0,0 };

    float rollSpeed = DirectX::XMConvertToRadians(90);
    float range = 15.0f;

    float maxAngleX = DirectX::XMConvertToRadians(50);
    float minAngleX = DirectX::XMConvertToRadians(-45);

};
