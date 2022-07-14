#pragma once
#include "Scene.h"

// �V�[���}�l�[�W���[
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}

public:
    // �B��̃C���X�^���X�擾
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    void Update(float elapsedTime);        // �X�V����
    void Render();                         // �`�揈��
    void Clear();                          // �V�[���N���A
    void ChangeScene(Scene* scene);        // �V�[���؂�ւ�

private:
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
};
