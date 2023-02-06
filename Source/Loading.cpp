#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

#include "./Graphics/font.h"

// ������
void SceneLoading::Initialize()
{
    // �X�v���C�g������
    //sprite = new Sprite("Data/Sprite/LoadingIcon.png");

    // �X���b�h�J�n
    thread = new std::thread(LoadingThread, this);

    fade = new RectFade();
    fade->SetAll(0, 0, 1280, 720, 1280, 720, 0, 0, 0, 1);

    standertPos = 650;
    dotPos[0] = { 1070, standertPos };
    dotPos[1] = { 1110, standertPos };
    dotPos[2] = { 1150, standertPos };
}

// �I����
void SceneLoading::Finalize()
{
    // �X���b�h�I����
    if (thread != nullptr)
    {
        thread->join();
        delete thread;
        thread = nullptr;
    }

    if (fade != nullptr)
    {
        delete fade;
        fade = nullptr;
    }
}

// �X�V����
void SceneLoading::Update(float elapsedTime)
{

    switch (state)
    {
    case 0:
        // �t�F�[�h�A�E�g
        if (fade->fadeOut(0.01f))  state++;
        break;

    case 1:
        // �ʏ�
        // ���̃V�[���̏���������������V�[����؂�ւ���
        if (nextScene->IsReady())
        {
            state++;
        }

        
        break;

    case 2:
        // �t�F�[�h�C��
        if (fade->fadeIn(0.01f))
        {
            SceneManager::Instance().ChangeScene(nextScene);
            nextScene = nullptr;
        }
        break;
    }
    DotUpdate(elapsedTime);


}

// �`�揈��
void SceneLoading::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };    // RGBA(0.0 ~ 1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    RenderContext rc;
    rc.deviceContext = dc;

    // 2D�X�v���C�g�`��
    {
        SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);

        font::textOut(3, "Now Loading",
                          550, 650,
                          4, 4,
                          1, 1, 1, 1);
        for (int i = 0; i < 3; i++)
        {
            font::textOut(3, ".",
                dotPos[i].x, dotPos[i].y,
                4, 4,
                1, 1, 1, 1);
        }



        // �t�F�[�h�̕`��
        fade->Render(rc, shader);    
    }

}

void SceneLoading::DotUpdate(float elapsedTime)
{
    switch (dotState)
    {
    case 0:
        // ��ɏオ��
        dotPos[currentNum].y -= 0.8f;
        if (standertPos - 15 >= dotPos[currentNum].y)
        {
             dotPos[currentNum].y = standertPos - 15;
             dotState++;
        }
        break;

    case 1:
        // ���ɉ�����
        dotPos[currentNum].y += 2.0;
        if (standertPos <= dotPos[currentNum].y)
        {
            dotPos[currentNum].y = standertPos;
            dotState = 0;
            currentNum++;
            currentNum = currentNum % 3;
        }
        break;
    }
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
    // COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
    CoInitialize(nullptr);

    // ���̃V�[���̏��������s��
    scene->nextScene->Initialize();

    // �X���b�h���I���O��COM�֘A�̏I����
    CoUninitialize();

    // ���̃V�[���̏��������ݒ�
    scene->nextScene->SetReady();
}