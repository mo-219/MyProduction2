#pragma once
//#include <memory>
//
//#include "Stage.h"
//#include "Graphics/Model.h"
//#include "GameObjectData.h"
//#include "GameObject.h"
//
//
//
//// ���C���X�e�[�W
//class StageSample : public Stage
//{
//public:
//    StageSample();
//    ~StageSample() override;
//
//    void LoadGameObj(const char* filename);
//
//    //�X�V����
//    void Update(float elapsedTime);
//
//    //�`�揈��
//    void Render(ID3D11DeviceContext* dc, Shader* shader)override;
//    void Render(const RenderContext& rc, ModelShader* shader)override;
//
//    // �V�F�[�_�[�p�ݒ�֐�
//    RenderContext SetRenderContext(const RenderContext& rc)override;
//
//
//    // ���C�L���X�g
//    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override;
//
//
//
//private:
//    Model* model = nullptr;
//    std::vector<GameObject*> objData;
//
//};