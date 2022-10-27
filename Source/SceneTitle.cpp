#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneLoading.h"
#include "SceneGame.h"

#include "Input/Input.h"

// ������
void SceneTitle::Initialize()
{
    texture = std::make_unique<Texture>("Data/Texture/Title.png");
    sprite = std::make_unique<Sprite>();

    sprite->SetShaderResourceView(texture->GetShaderResourceView(), texture->GetWidth(), texture->GetHeight());
}

// �I����
void SceneTitle::Finalize()
{
    //// �X�v���C�g�I����
    //if (sprite != nullptr)
    //{
    //    delete sprite;
    //    sprite = nullptr;
    //}
}

// �X�V����
void SceneTitle::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    sprite->Update(0.0f, 0.0f,
        Graphics::Instance().GetScreenWidth(), Graphics::Instance().GetScreenHeight(),
        0.0f, 0.0f,
        static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight()),
        0.0f,
        1.0f, 1.0f, 1.0f, 1.0f);


    // �����{�^������������Q�[���V�[���֐؂�ւ�
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

    if (gamePad.GetButtonDown() & anyButton)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
    }
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
        SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);
        GaussianFilterData          gaussianFilterData;

        rc.gaussianFilterData.deviation = gaussianFilterData.deviation;
        rc.gaussianFilterData.kernelSize = gaussianFilterData.kernelSize;
        rc.gaussianFilterData.textureSize.x = texture->GetWidth();
        rc.gaussianFilterData.textureSize.y = texture->GetHeight();

        // �`��J�n
        shader->Begin(rc);

        // �`��
        shader->Draw(rc, sprite.get());

        // �`��I��
        shader->End(rc);
    }

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(" ", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::Image(texture->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 },
            { 1, 1, 1, 1 });

    }
    ImGui::End();

}
