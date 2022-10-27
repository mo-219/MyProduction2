#include"StageMain.h"

//static Stage* instance = nullptr;
//
//// インスタンス取得
//Stage& Stage::Instance()
//{
//    return *instance;
//}


// コンストラクタ
StageMain::StageMain()
{
    //ステージモデル読み込み
    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
}

// デストラクタ
StageMain::~StageMain()
{
    // ステージモデルを破棄
    delete model;
}

// 更新処理
void StageMain::Update(float elapsedTime)
{
    //今はやることない

}

// 描画処理
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // シェーダーにモデルを描画してもらう
    shader->Draw(dc, model);
}

void StageMain::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}

RenderContext StageMain::SetRenderContext(const RenderContext& rc)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;

    return myRc;
}

// レイキャスト
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}