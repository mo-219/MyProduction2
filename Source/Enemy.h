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
    
    // �`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

    // �j������
    void Destroy();

    // �f�o�b�O�v���~�e�B�u�`��
 

    // �p����ł̎������Y���h�~���邽�߂ɏ������z�֐��ɂ��Ă���
    // �p����p����Ƃ��͂�������������g��ł�������

protected:
};