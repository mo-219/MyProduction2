#pragma once
#include "Stage.h"
#include "Collision.h"

class StageDoor : public Stage
{
public:
    StageDoor() {};
    virtual ~StageDoor() {};

    //static Stage& Instance();

    //�X�V����
    void Update(float elapsedTime) override;

    //�`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;
    void Render(const RenderContext& rc, ModelShader* shader) override;


    RenderContext SetRenderContext(const RenderContext& rc) override;


    // ���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override { return false; };

private:


    bool goalFlag = false;


};
