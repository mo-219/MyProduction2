#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Scene.h"

#include "Rect.h" 

#include "Audio/AudioSource.h"
//#include "DXTKAudio.h"

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
    RectFade*                   fade = nullptr;         // ��ʂ̃t�F�[�h�p 
    int                         state = 0;              // Update�J�ڗp
    bool                        changeScene = false;    // ��ʑJ�ڃt���O

    std::unique_ptr<Texture>	texture;
    std::unique_ptr<Sprite>		sprite;

    std::unique_ptr<Texture>	pressTexture;
    std::unique_ptr<Sprite>		pressSprite;

    DirectX::XMFLOAT2 renderPosition = {};

    bool                        alphaFlag = true;
    float                       alpha = 1.0f;

    std::unique_ptr<AudioSource> bgm;
    std::unique_ptr<AudioSource> se;

};

#endif /* SCENE_TITLE_H */