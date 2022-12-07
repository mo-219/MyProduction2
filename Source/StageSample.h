#pragma once
//#include <memory>
//
//#include "Stage.h"
//#include "Graphics/Model.h"
//#include "GameObjectData.h"
//#include "GameObject.h"
//
//
//
//// メインステージ
//class StageSample : public Stage
//{
//public:
//    StageSample();
//    ~StageSample() override;
//
//    void LoadGameObj(const char* filename);
//
//    //更新処理
//    void Update(float elapsedTime);
//
//    //描画処理
//    void Render(ID3D11DeviceContext* dc, Shader* shader)override;
//    void Render(const RenderContext& rc, ModelShader* shader)override;
//
//    // シェーダー用設定関数
//    RenderContext SetRenderContext(const RenderContext& rc)override;
//
//
//    // レイキャスト
//    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override;
//
//
//
//private:
//    Model* model = nullptr;
//    std::vector<GameObject*> objData;
//
//};