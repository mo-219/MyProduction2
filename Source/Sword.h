#pragma once
#include "Param.h"

#include "Graphics/shader.h"
#include "Graphics/Model.h"

class Sword 
{
public:
    Sword(const char* filename);
    ~Sword();

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Render(const RenderContext& rc, ModelShader* shader);

    // デバッグ用GUI描画
    void DrawDebugGUI();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    void UpdateTransform(DirectX::XMFLOAT4X4 mat);

    void SetTipPos(DirectX::XMFLOAT3 pos) { tipPos = pos; }

    void SetCollisionValue();

    void collisionWeaponVSEnemies();

    // 位置設定
    void setPosition(const DirectX::XMFLOAT3& position) { this->param.position = position; }

    // 回転取得
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }

    // 回転設定
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    // スケール取得
    const DirectX::XMFLOAT3& SetScale() const { return scale; }

    // スケール取得
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // 半径取得
    float GetRadius() const { return param.radius; }

    // 高さ取得
    float GetHeight() const { return param.height; }
    float GetWidth() const { return param.width; }
    float GetDepth() const { return param.depth; }


private:
    Model* model = nullptr;
    DirectX::XMFLOAT3 tipPos = {};

    // 当たり判定
    static const int collisionNum = 3;
    DirectX::XMFLOAT3 collisionPos[collisionNum];

    float collisionRange = 0.15f;

    Param param = { DirectX::XMFLOAT3(0,0,0), 0.5f, 0.0f,2.0f,0.0f };

    DirectX::XMFLOAT3   angle = { 0,0,0 };
    DirectX::XMFLOAT3   scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 transform = {
        1,0,0,0,    // X軸ベクトル(右方向ベクトルのx,y,z(右はどっちかがわかる))
        0,1,0,0,    // Y軸ベクトル(上方向のベクトル)
        0,0,1,0,    // Z軸ベクトル(前方向のベクトル)
        0,0,0,1     // 位置
    };


    // シェーダー用
// キュービックカラー
    DirectX::XMFLOAT4 colorTop1 = { 1.0f, 1.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorBottom1 = { 1.0f, 0.8f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorRight1 = { 1.0f, 0.0f, 0.8f, 1.0f };
    DirectX::XMFLOAT4 colorLeft1 = { 0.0f, 1.0f, 0.3f, 1.0f };
    DirectX::XMFLOAT4 colorBack1 = { 0.2f, 0.1f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 colorFront1 = { 1.0f, 0.0f, 0.0f, 1.0f };

    DirectX::XMFLOAT4 colorTop2 = { 1.0f, 1.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorBottom2 = { 1.0f, 0.8f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 colorRight2 = { 1.0f, 0.0f, 0.8f, 1.0f };
    DirectX::XMFLOAT4 colorLeft2 = { 0.0f, 1.0f, 0.3f, 1.0f };
    DirectX::XMFLOAT4 colorBack2 = { 0.2f, 0.1f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 colorFront2 = { 1.0f, 0.0f, 0.0f, 1.0f };

    DirectX::XMFLOAT4 colorAlpha = { 1,1,1,0.5f };

};
