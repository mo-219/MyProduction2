#pragma once

#include "Graphics/Shader.h"
#include "Character.h"


// �G�l�~�[
class Enemy : public Character
{
public:
    Enemy() {}
    ~Enemy()override {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;
    virtual void UpdateOnlyTransform(float elapsedTime) = 0;
    
    // �`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
    virtual void Render(const RenderContext& rc, ModelShader* shader) = 0;

    // �V�F�[�_�[�p�ݒ�֐�
    virtual RenderContext SetRenderContext(const RenderContext& rc) = 0;

    // �꒣��ݒ�
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);



    // �j������
    void Destroy();


    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive() {};

    virtual void DrawDebugImGui() {};

protected:

    DirectX::XMFLOAT3   targetPosition = { 0,0,0 };
    DirectX::XMFLOAT3   territoryOrigin = { 0,0,0 };
    float               territoryRange = 10.0f;
};