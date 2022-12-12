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

    //�s��X�V����
    void UpdateTransform();
    void DebugPrimitive();
private:
    Model* model;
    SkyShader* shader;

    DirectX::XMFLOAT3 position = {0,0,0};
    DirectX::XMFLOAT3 scale = {0.01f, 0.01f, 0.01f };


    DirectX::XMFLOAT4X4 transform = {
    1,0,0,0,    // X���x�N�g��(�E�����x�N�g����x,y,z(�E�͂ǂ��������킩��))
    0,1,0,0,    // Y���x�N�g��(������̃x�N�g��)
    0,0,1,0,    // Z���x�N�g��(�O�����̃x�N�g��)
    0,0,0,1     // �ʒu
    };

};