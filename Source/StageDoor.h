#pragma once
#include "Stage.h"
#include "Collision.h"

class StageDoor : public Stage
{
public:
    StageDoor() {};
    virtual ~StageDoor() {};

    //static Stage& Instance();

    //更新処理
    void Update(float elapsedTime) override;

    //描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;
    void Render(const RenderContext& rc, ModelShader* shader) override;


    RenderContext SetRenderContext(const RenderContext& rc) override;


    // レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override { return false; };

private:


    bool goalFlag = false;


};
