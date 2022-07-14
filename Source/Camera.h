#pragma once

#include <DirectXMath.h>

// カメラ
class Camera
{
private:
    Camera() {}
    ~Camera() {}

public:


    // 唯一のインスタンス取得
    static Camera& Instance()
    {
        static Camera camera;
        return camera;
    }

    // 指定方向を向く
    void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus,
                   const DirectX::XMFLOAT3& up);

    // パースペクティブ設定
    void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

    // ビュー行列取得
    const DirectX::XMFLOAT4X4& GetView() const { return view; }

    // プロジェクション行列取得
    const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }


    // 視点習得
    const DirectX::XMFLOAT3& GetEye() const { return eye; }

    // 注視点習得
    const DirectX::XMFLOAT3& GetFocus() const { return focus; }

    // 上方向習得
    const DirectX::XMFLOAT3& GetUp() const { return up; }

    // 前方向習得
    const DirectX::XMFLOAT3& GetFront() const { return front; }

    // 右方向習得
    const DirectX::XMFLOAT3& GetRight() const { return right; }

private:

    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;

    DirectX::XMFLOAT3 eye;
    DirectX::XMFLOAT3 focus;

    DirectX::XMFLOAT3 up;
    DirectX::XMFLOAT3 front;
    DirectX::XMFLOAT3 right;
};

// Eye   カメラの位置
// Focus どこを見ているか（遠近感的なそれではない）
// Up    カメラの上(仮)方向（仮の上を定義しないと姿勢が決まらない）
// 仮の理由→本当の上は向いているベクトルの直角にならなければならない
// 上→鉛直上向きが多い（違うものは違うので注意）マリオギャラクシーを作るときは重要
// 視野角　
// インチは対角線の長さ
// FPSのスナイパースコープの原理 → ①カメラを物理的に近づける ②視野角を狭める
// 
