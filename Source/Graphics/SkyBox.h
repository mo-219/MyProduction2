#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Shader.h"
#include "Mathf.h"
#include "SkyShader.h"

class SkyBox
{
public:
    SkyBox(ID3D11Device* device);
    ~SkyBox();

    void Render(const RenderContext& rc);

    void Render(const RenderContext& rc, ModelShader* shader);

    void SetPosition(DirectX::XMFLOAT3 pos) { position = pos; }

    //行列更新処理
    void UpdateTransform();
    void DebugPrimitive();
private:
    Model* model;
    SkyShader* shader;

    DirectX::XMFLOAT3 position = {0,0,0};
    DirectX::XMFLOAT3 scale = {0.01f, 0.01f, 0.01f };


    DirectX::XMFLOAT4X4 transform = {
    1,0,0,0,    // X軸ベクトル(右方向ベクトルのx,y,z(右はどっちかがわかる))
    0,1,0,0,    // Y軸ベクトル(上方向のベクトル)
    0,0,1,0,    // Z軸ベクトル(前方向のベクトル)
    0,0,0,1     // 位置
    };

};