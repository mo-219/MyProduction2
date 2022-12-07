#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Collision.h"

//�X�e�[�W
enum StageNumber
{
    Error = -1,
    Main = 0,
    Movefloor,
    Door,

    Max,
};

//�X�e�[�W
class Stage
{
public:
    Stage() {};
    virtual ~Stage() {};

    //static Stage& Instance();

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
    virtual void Render(const RenderContext& rc, ModelShader* shader) = 0;

    // �V�F�[�_�[�p�ݒ�֐�
    virtual RenderContext SetRenderContext(const RenderContext& rc) = 0;

    // ���C�L���X�g
    virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

    // �j������
    void Destroy();

    // �X�e�[�W�̔ԍ�
    void setStageNum(int i) { stageNum = i; }
    int GetStageNum() { return stageNum; }

    // �X�e�[�W���̃Q�b�^�[
    DirectX::XMFLOAT3 GetPosition() { return position; }
    float GetHeight() { return height; }
    float GetWidth() { return width; }
    float GetDepth() { return depth; }
    bool GetCollisionFlag() { return collisionFlag; }


    // �X�e�[�W���̃Z�b�^�[
    void setPosition(DirectX::XMFLOAT3 pos) { position = pos; }
    void setPosition(float x, float y, float z) { position.x = x; position.y = y; position.z = z; }
    void setWidth(float wi) { width = wi; }
    void setHeight(float he) { height = he; }
    void setDepth(float de) { depth = de; }
    void setCollisionFlag(bool b) { collisionFlag = b; }

protected:
    Model* model = nullptr;

    DirectX::XMFLOAT3 position = { 0,0,0 };
    float height = 0;
    float width = 0;
    float depth = 0;

    int stageNum = -1;

    bool collisionFlag = true;
    //true: �����蔻�肠��
    //false: �����蔻�薳��

};