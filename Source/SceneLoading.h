#pragma once
#include <thread>

#include "Graphics/Sprite.h"
#include "Scene.h"

// ���[�f�B���O�V�[��
class SceneLoading : public Scene
{
//�@�����o�֐�
public:
    SceneLoading(Scene* next) : nextScene(next) {}
    ~SceneLoading() override {}

    void Initialize() override;
    void Finalize() override;
    void Update(float elapsedTime) override;
    void Render() override;

private:
    static void LoadingThread(SceneLoading* scene);     // ���[�f�B���O�X���b�h

// �����o�ϐ�
private:
    Sprite* sprite = nullptr;
    float angle = 0.0f;

    Scene* nextScene = nullptr;
    std::thread* thread = nullptr;
};
