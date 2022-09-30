#pragma once

#include "Graphics/shader.h"
#include "Graphics/Model.h"
#include "Character.h"

#include "ProjectileManager.h"

#include "Effect.h"

//�v���C���[
class Player : public Character
{
public:
    Player();
    ~Player();

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Render(const RenderContext& rc, ModelShader* shader);

    // �f�o�b�O�pGUI�`��
    void DrawDebugGUI();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �W�����v���͏���
    void ImputJump();

protected:
    // �ڒn�����Ƃ��ɌĂ΂��
    void OnLanding() override;

private:
    // �X�e�B�b�N���͒l����ړ��x�N�g�����擾
    DirectX::XMFLOAT3 GetMoveVec() const;


    // �ړ����͏���
    void InputMove(float elapsedTime);


    // �v���C���[�ƃG�l�~�[�̏Փˏ���
    void CollisionPlayerVsEnemies();

    // �e�ۂƓG�̏Փˏ���
    void CollisionProjectilesVsEnemies();

    // �e�ۓ��͏���
    void InputProjectile();

private:
    Model* model            = nullptr;
    float  moveSpeed        = 5.0f;

    float  turnSpeed        = DirectX::XMConvertToRadians(720);     // 720��]�܂ł�k

    // �W�����v�ɕK�v�ȕϐ�
    float  jumpSpeed        = 20.0f;
    int    jumpCount        = 0;
    int    jumpLimit        = 2;

    //int    jumpRest         = 0;
    
    ProjectileManager projectileManager;

    Effect* hitEffect = nullptr;

};