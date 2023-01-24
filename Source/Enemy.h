#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "Telegram.h"


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

    // �U���t���O�擾
    virtual bool	GetAttackFlg() { return attackFlg; }

    // �U���t���O�Z�b�g
    virtual void	SetAttackFlg(bool flg) { attackFlg = flg; };
    virtual void	SetId(int id) { this->id = id; }
    virtual int		GetId() { return id; }

    // �U���͈͎擾
    virtual float GetAttackRange() { return attackRange; }

    // ���b�Z�[�W��M�����Ƃ��̏���
    virtual bool OnMessage(const Telegram& msg);


    // �j������
    void Destroy();


    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive() {};

    virtual void DrawDebugImGui() {};

protected:

    DirectX::XMFLOAT3   targetPosition = { 0,0,0 };
    DirectX::XMFLOAT3   territoryOrigin = { 0,0,0 };
    float               territoryRange = 10.0f;

    int                 DropHeelItem = 0;


    bool attackFlg = false;
    int id = 0;

    float searchRange = 0.0f;
    float attackRange = 0.0f;
};