#pragma once
#include "Stage.h"
#include "Graphics/Model.h"


// ���C���X�e�[�W
class StageMain : public Stage
{
public:
    StageMain();
    ~StageMain() override;


    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(const RenderContext& rc, ModelShader* shader)override;

    // �V�F�[�_�[�p�ݒ�֐�
    RenderContext SetRenderContext(const RenderContext& rc)override;


    // ���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override;



private:
    Model* model = nullptr;

};