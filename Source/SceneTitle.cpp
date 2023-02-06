#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneLoading.h"
#include "SceneGame.h"

#include "Input/Input.h"

#include "Graphics/font.h"
#include "Audio/Audio.h"
#include "Audio/AudioSource.h"
//#include "Music.h"
//#include "Sound.h"


// ������
void SceneTitle::Initialize()
{
    texture = std::make_unique<Texture>("Data/Sprite/Title.png");
    sprite = std::make_unique<Sprite>();

    pressTexture = std::make_unique<Texture>("Data/Sprite/PressAnyKey.png");
    pressSprite = std::make_unique<Sprite>();

    sprite->SetShaderResourceView(texture->GetShaderResourceView(), texture->GetWidth(), texture->GetHeight());
    pressSprite->SetShaderResourceView(pressTexture->GetShaderResourceView(), pressTexture->GetWidth(), pressTexture->GetHeight());

    fade = new RectFade();
    fade->SetAll(0, 0, 1280, 720, 1280, 720, 0, 0, 0, 1);

    bgm = Audio::Instance().LoadAudioSource("Data/Audio/BGM/Title.wav");
    bgm->Play(true);

    se = Audio::Instance().LoadAudioSource("Data/Audio/SE/Select.wav");
}
// �I����
void SceneTitle::Finalize()
{
    bgm->Stop();
    bgm = nullptr;
    if (fade != nullptr)
    {
        delete fade;
        fade = nullptr;
    }
}

// �X�V����
void SceneTitle::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    const GamePadButton anyButton = GamePad::BTN_UP
        | GamePad::BTN_RIGHT
        | GamePad::BTN_DOWN
        | GamePad::BTN_LEFT
        | GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_X
        | GamePad::BTN_Y
        | GamePad::BTN_START
        | GamePad::BTN_BACK
        | GamePad::BTN_LEFT_THUMB
        | GamePad::BTN_RIGHT_THUMB
        | GamePad::BTN_LEFT_SHOULDER
        | GamePad::BTN_RIGHT_SHOULDER
        | GamePad::BTN_LEFT_TRIGGER
        | GamePad::BTN_RIGHT_TRIGGER;


    switch (state)
    {
    case 0:
        // �t�F�[�h�A�E�g
        if (fade->fadeOut(0.01f))  state++;
        break;
    
    case 1:
        // �ʏ�
        // �����{�^������������Q�[���V�[���֐؂�ւ�
        if (gamePad.GetButtonDown() & anyButton)
        {
            //SceneManager::Instance().ChangeScene(new SceneGame);
            changeScene = true;
            state++;
            se->Play(false);
        }
        break;

    case 2:
        if (fade->fadeIn(0.01f))
        {
            SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
        }
        break;
    }


    // �摜�A�b�v�f�[�g

    if (alphaFlag)
    {
        alpha -= 0.02f;
        if (alpha <= 0.2f)
        {
            alphaFlag = false;
        }
    }
    else
    {
        alpha += 0.02f;
        if (alpha >= 1.0f)
        {
            alphaFlag = true;
        }
    }



    sprite->Update(0.0f, 0.0f,
        Graphics::Instance().GetScreenWidth(), Graphics::Instance().GetScreenHeight(),
        0.0f, 0.0f,
        static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight()),
        0.0f,
        1.0f, 1.0f, 1.0f, 1.0f);

    pressSprite->Update(Graphics::Instance().GetScreenWidth()/2-297, 580,
        593, 65,
        0.0f, 0.0f,
        static_cast<float>(pressTexture->GetWidth()), static_cast<float>(pressTexture->GetHeight()),
        0.0f,
        1.0f, 1.0f, 1.0f, alpha);

}

// �`�揈��
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };    // RGBA(0.0 ~ 1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    RenderContext rc;
    rc.deviceContext = dc;

    // 2D�X�v���C�g�`��
    {
        SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);
        GaussianFilterData          gaussianFilterData;

        rc.gaussianFilterData.deviation = gaussianFilterData.deviation;
        rc.gaussianFilterData.kernelSize = gaussianFilterData.kernelSize;
        rc.gaussianFilterData.textureSize.x = texture->GetWidth();
        rc.gaussianFilterData.textureSize.y = texture->GetHeight();

        // �`��J�n
        shader->Begin(rc);

        // �X�v���C�g�̕`��
        shader->Draw(rc, sprite.get());
        shader->Draw(rc, pressSprite.get());

        // �`��I��
        shader->End(rc);

        // �t�F�[�h�̕`��
        fade->Render(rc, shader);

    }

}
