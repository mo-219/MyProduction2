#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
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
    std::unique_ptr<Texture>	texture;
    std::unique_ptr<Sprite>		sprite;

    DirectX::XMFLOAT2 renderPosition = {};
};

#endif /* SCENE_TITLE_H */