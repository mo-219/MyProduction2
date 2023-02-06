#pragma once
#include "Stage.h"
#include "Collision.h"
#include "Audio/Audio.h"

class StageDoor : public Stage
{
public:
    StageDoor();
    virtual ~StageDoor();

    //static Stage& Instance();

    //更新処理
    void Update(float elapsedTime) override;

    //描画処理
    void Render(const RenderContext& rc, ModelShader* shader) override;


    RenderContext SetRenderContext(const RenderContext& rc) override;


    // レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override { return false; };

private:
    void UpdateTransform();

    void TraditionOpenDoor();

private:
    enum Animation
    {
        Anim_Close,
        Anim_Open,
        Anim_Max
    };


    DirectX::XMFLOAT3   angle = { 0, 0, 0 };
    DirectX::XMFLOAT3   scale = { 1, 1, 1 };
    DirectX::XMFLOAT4X4 transform = { 1,0,0,0,
                                      0,1,0,0,
                                      0,0,1,0,
                                      0,0,0,1 };



    bool goalFlag = false;

    std::unique_ptr<AudioSource> se;
};
