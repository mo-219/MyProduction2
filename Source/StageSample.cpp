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
//// �R���X�g���N�^
//StageSample::StageSample()
//{
//    //�X�e�[�W���f���ǂݍ���
//    LoadGameObj("SimpleNaturePack_Demo.json");
//
//}
//
//// �f�X�g���N�^
//StageSample::~StageSample()
//{
//    objData.clear();
//    // �X�e�[�W���f����j��
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
//// �X�V����
//void StageSample::Update(float elapsedTime)
//{
//    //���͂�邱�ƂȂ�
//
//}
//
//// �`�揈��
//void StageSample::Render(ID3D11DeviceContext* dc, Shader* shader)
//{
//    // �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
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
//// ���C�L���X�g
//bool StageSample::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
//{
//    return Collision::IntersectRayVsModel(start, end, model, hit);
//}