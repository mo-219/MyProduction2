#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

// �^�C�g���V�[��
class SceneTitle : public Scene
{
public:
    SceneTitle() {}
    ~SceneTitle() override {}


    void Initialize() override;                  // ������
    void Finalize() override;                    // �I����
    void Update(float elapsedTime) override;     // �X�V����
    void Render() override;                      // �`�揈��

private:
    Sprite* sprite = nullptr;


};
