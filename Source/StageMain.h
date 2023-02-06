#pragma once
#include "Stage.h"
#include "Graphics/Model.h"


// メインステージ
class StageMain : public Stage
{
public:
    StageMain();
    ~StageMain() override;


    //更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(const RenderContext& rc, ModelShader* shader)override;

    // シェーダー用設定関数
    RenderContext SetRenderContext(const RenderContext& rc)override;


    // レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override;



private:
    Model* model = nullptr;

};