//#include"StageSample.h"
//#include <stdio.h>
//
//#include <fstream>
//#include <string>
//
//#include <cereal/types/map.hpp>
//#include <cereal/types/string.hpp>
//#include <cereal/archives/json.hpp>
//#include <cereal/types/vector.hpp>
//
//#include <cereal/archives/portable_binary.hpp>
//#include <cereal/archives/binary.hpp>
//
//// コンストラクタ
//StageSample::StageSample()
//{
//    //ステージモデル読み込み
//    LoadGameObj("SimpleNaturePack_Demo.json");
//
//}
//
//// デストラクタ
//StageSample::~StageSample()
//{
//    objData.clear();
//    // ステージモデルを破棄
//    delete model;
//}
//
//void StageSample::LoadGameObj(const char* filename)
//{
//    GameObjectData data;
//    std::ifstream ifs(filename, std::ios::binary);
//    if (ifs)
//    {
//        cereal::JSONInputArchive archive(ifs);
//        data.serialize(archive);
//        ifs.close();
//
//        GameObject* obj;
//        for (auto object : data.objects)
//        {
//            obj = new GameObject();
//            obj->name = object.name;
//            obj->code = object.code;
//            obj->model_filename = object.model_filename;
//            obj->texture_filename = object.texture_filename;
//            obj->scale = object.scale;
//            obj->rotate = object.rotate;
//            obj->translate = object.translate;
//
//            objData.emplace_back(obj);
//        }
//    }
//
//}
//
//
//
//// 更新処理
//void StageSample::Update(float elapsedTime)
//{
//    //今はやることない
//
//}
//
//// 描画処理
//void StageSample::Render(ID3D11DeviceContext* dc, Shader* shader)
//{
//    // シェーダーにモデルを描画してもらう
//    shader->Draw(dc, model);
//}
//
//void StageSample::Render(const RenderContext& rc, ModelShader* shader)
//{
//    shader->Draw(rc, model);
//}
//
//RenderContext StageSample::SetRenderContext(const RenderContext& rc)
//{
//    RenderContext myRc = rc;
//    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;
//
//    return myRc;
//}
//
//// レイキャスト
//bool StageSample::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
//{
//    return Collision::IntersectRayVsModel(start, end, model, hit);
//}