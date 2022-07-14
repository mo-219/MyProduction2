#include "SceneManager.h"

// �X�V����
void SceneManager::Update(float elapsedTime)
{
    if (nextScene != nullptr)
    {
        // �Â��V�[�����I������
        Clear();

        // �V�����V�[����ݒ�
        currentScene = nextScene;
        nextScene = nullptr;

        // �V�[������������
        if (!currentScene->IsReady())
        {
            // ���[�f�B���O�̂���K�v�����邩�ǂ��������ʂ��Ă���
            // �����ŃC�j�V�����C�Y���Ăق����Ȃ��̂̓X���b�h�ł��łɃC�j�V�����C�Y���Ă�����́B
            currentScene->Initialize();
        }

    }
    if (currentScene != nullptr)
    {
        currentScene->Update(elapsedTime);
    }
}

// �`�揈��
void SceneManager::Render()
{
    if (currentScene != nullptr)
    {
        currentScene->Render();
    }
}

// �V�[���N���A
void SceneManager::Clear()
{
    if (currentScene != nullptr)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

// �V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
    // �V�����V�[����ݒ�
    nextScene = scene;
}