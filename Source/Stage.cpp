#include"Stage.h"

static Stage* instance = nullptr;

// インスタンス取得
Stage& Stage::Instance()
{
    return *instance;
}


// コンストラクタ
Stage::Stage()
{
    instance = this;
    //ステージモデル読み込み
    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
}

// デストラクタ
Stage::~Stage()
{
    // ステージモデルを破棄
    delete model;
}

// 更新処理
void Stage::Update(float elapsedTime)
{
    //今はやることない

}

// 描画処理
void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // シェーダーにモデルを描画してもらう
    shader->Draw(dc, model);
}

void Stage::Render(const RenderContext& rc, ModelShader* shader)
{
    RenderContext prc = rc;
    prc.cubicColorData.shaderFlag = CUBIC_DEFAULT;
    shader->Draw(rc, model);
}

// レイキャスト
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}