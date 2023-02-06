#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

#include "./Graphics/font.h"

// 初期化
void SceneLoading::Initialize()
{
    // スプライト初期化
    //sprite = new Sprite("Data/Sprite/LoadingIcon.png");

    // スレッド開始
    thread = new std::thread(LoadingThread, this);

    fade = new RectFade();
    fade->SetAll(0, 0, 1280, 720, 1280, 720, 0, 0, 0, 1);

    standertPos = 650;
    dotPos[0] = { 1070, standertPos };
    dotPos[1] = { 1110, standertPos };
    dotPos[2] = { 1150, standertPos };
}

// 終了化
void SceneLoading::Finalize()
{
    // スレッド終了化
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

// 更新処理
void SceneLoading::Update(float elapsedTime)
{

    switch (state)
    {
    case 0:
        // フェードアウト
        if (fade->fadeOut(0.01f))  state++;
        break;

    case 1:
        // 通常
        // 次のシーンの準備が完了したらシーンを切り替える
        if (nextScene->IsReady())
        {
            state++;
        }

        
        break;

    case 2:
        // フェードイン
        if (fade->fadeIn(0.01f))
        {
            SceneManager::Instance().ChangeScene(nextScene);
            nextScene = nullptr;
        }
        break;
    }
    DotUpdate(elapsedTime);


}

// 描画処理
void SceneLoading::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };    // RGBA(0.0 ~ 1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    RenderContext rc;
    rc.deviceContext = dc;

    // 2Dスプライト描画
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



        // フェードの描画
        fade->Render(rc, shader);    
    }

}

void SceneLoading::DotUpdate(float elapsedTime)
{
    switch (dotState)
    {
    case 0:
        // 上に上がる
        dotPos[currentNum].y -= 0.8f;
        if (standertPos - 15 >= dotPos[currentNum].y)
        {
             dotPos[currentNum].y = standertPos - 15;
             dotState++;
        }
        break;

    case 1:
        // 下に下がる
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
    // COM関連の初期化でスレッド毎に呼ぶ必要がある
    CoInitialize(nullptr);

    // 次のシーンの初期化を行う
    scene->nextScene->Initialize();

    // スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    // 次のシーンの準備完了設定
    scene->nextScene->SetReady();
}