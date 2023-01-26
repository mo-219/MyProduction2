#include "LoadObj.h"
#include "GameObjectManager.h"



#include <iostream>
#include <fstream>
#include <string>

#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>




void LoadObj::Load(const char* filename)
{
    std::ifstream ifs(filename, std::ios::in);
    cereal::JSONInputArchive archive(ifs);
    loadData.serialize(archive);
    //loadData.objects.clear();
}

void LoadObj::Create()
{
    // ロードデータに何もはいってないときはなにもせず返す
    if (loadData.objects.size() <= 0)    return;

    ObjectManager& objectManager = ObjectManager::Instance();

    GameObject* obj;
    for (auto& data : loadData.objects)
    {
        obj = new GameObject();
        obj->SetCode(data.code);
        obj->SetScale(data.scale);
        DirectX::XMFLOAT3 rotate;
        rotate.x = DirectX::XMConvertToRadians(data.rotate.x);
        rotate.y = DirectX::XMConvertToRadians(data.rotate.y);
        rotate.z = DirectX::XMConvertToRadians(data.rotate.z);

        obj->SetAngle(rotate);
        obj->SetPosition(data.translate);
        obj->Initialize();

        /*obj->CreateModel(data.model_filename.c_str());*/
        objectManager.Register(obj);

        obj = nullptr;
    }
}

void LoadObj::Create(std::map<std::string, ObjData> Map)
{
    // ロードデータに何もはいってないときはなにもせず返す
    if (loadData.objects.size() <= 0)    return;

    ObjectManager& objectManager = ObjectManager::Instance();

    GameObject* obj;
    for (auto& data : loadData.objects)
    {

        obj = new GameObject();
        obj->SetCode(data.code);
        obj->SetScale(data.scale);

        DirectX::XMFLOAT3 rotate;
        rotate.x = DirectX::XMConvertToRadians(data.rotate.x);
        rotate.y = DirectX::XMConvertToRadians(data.rotate.y);
        rotate.z = DirectX::XMConvertToRadians(data.rotate.z);

        obj->SetAngle(rotate);
        
        obj->Initialize();

        for (auto& map : Map)
        {
            if (obj->code == map.first)
            {
                Param param = { DirectX::XMFLOAT3{}, map.second.radius, map.second.width, map.second.height, map.second.depth};
                obj->SetParam(param);
                obj->CreateModel(map.second.modelName);
                obj->SetBehavior(map.second.behavior);
                obj->SetRayCast(map.second.rayCast);
                
            }
        }
        // 最後にposition代入
        obj->SetPosition(data.translate);

        if (obj->GetBehavior() == nullptr)
        {
            delete obj;
            continue;
        }

        objectManager.Register(obj);

    }
    loadData.objects.clear();


    //objectManager.Initialize();
}

//**************************************************
//
//              CubeBehavior
//
//**************************************************
bool CubeBehavior::collision(Param paramA,  Param paramB, DirectX::XMFLOAT3& outB)
{
    return Collision::InstersectCubeVsCylinder(paramA.position, paramA.width, paramA.height, paramA.depth, paramB.position, paramB.radius, paramB.height, outB);
}
void CubeBehavior::hit()
{
}

//**************************************************
//
//              SphereBehavior
//
//**************************************************
bool SphereBehavior::collision(Param paramA, Param paramB, DirectX::XMFLOAT3& outB)
{
    return Collision::InstersectSphereVsCylinder(paramA.position, paramA.radius,paramB.position, paramB.radius, paramB.height, outB);
}

void SphereBehavior::hit()
{
}

//**************************************************
//
//              CylinderBehavior
//
//**************************************************


bool CylinderBehavior::collision(Param paramA,  Param paramB, DirectX::XMFLOAT3& outB)
{
    return Collision::InstersectCylinderVsCylinder(paramA.position, paramA.radius, paramA.height, paramB.position,paramB.radius, paramB.height, outB);
}

void CylinderBehavior::hit()
{
}


//**************************************************
//
//              RayCastBehavior
//
//**************************************************

bool RayCastBehavior::collision(DirectX::XMFLOAT3& start, DirectX::XMFLOAT3& end, Model* model, HitResult& result)
{
    return Collision::IntersectRayVsModel(start, end, model, result);
}

void RayCastBehavior::hit()
{
}


//bool RayCastBehavior::collision(Param paramA, Param paramB, DirectX::XMFLOAT3& start, DirectX::XMFLOAT3& end, Model* model, HitResult& result)
//{
//    DirectX::XMFLOAT3 dummy;
//    if (Collision::InstersectSphereVsSphere(paramA.position, paramA.radius, paramB.position, paramB.radius * 10, dummy))
//    {
//        return Collision::IntersectRayVsModel(start, end, model, result);
//    }
//    return false;
//}
//
//void RayCastBehavior::hit()
//{
//}

//------------------------------------------------------------------------------------------------------

SphereBehavior sphereBehavior;
CubeBehavior cubeBehavior;
CylinderBehavior cylinderBehavior;
NoneBehavior noneBehavior;

RayCastBehavior rayCastBehavior;
NoneRayCastBehavior noneRayCastBehavior;



#define COLLISION_SPHERE    &sphereBehavior
#define COLLISION_CUBE      &cubeBehavior
#define COLLISION_CYLINDER  &cylinderBehavior
#define COLLISION_NONE      &noneBehavior

#define RayCastON           &rayCastBehavior
#define RayCastOFF          &noneRayCastBehavior
//======================================================================================================

// ObjData{ file_name, collisionBehavior, radius, width, height, depth, scale};
std::map<std::string, ObjData> stage1_mapData = {
    {"Bush_03",     ObjData{"Data/Stage/SimpleNaturePack/Models/Bush_03.mdl",      COLLISION_NONE,      RayCastOFF}},
    {"Flowers_01",  ObjData{"Data/Stage/SimpleNaturePack/Models/Flowers_01.mdl",   COLLISION_NONE,      RayCastOFF}},
    {"Flowers_02",  ObjData{"Data/Stage/SimpleNaturePack/Models/Flowers_02.mdl",   COLLISION_NONE,      RayCastOFF}},
    {"Grass_01",    ObjData{"Data/Stage/SimpleNaturePack/Models/Grass_01.mdl",     COLLISION_NONE,      RayCastOFF}},
    {"Grass_02",    ObjData{"Data/Stage/SimpleNaturePack/Models/Grass_02.mdl",     COLLISION_NONE,      RayCastOFF}},
    {"Ground_01",   ObjData{"Data/Stage/SimpleNaturePack/Models/Ground_01.mdl",    COLLISION_NONE,      RayCastON,      25.0f}},
    {"Ground_02",   ObjData{"Data/Stage/SimpleNaturePack/Models/Ground_02.mdl",    COLLISION_NONE,      RayCastON,      25.0f}},
    {"Ground_03",   ObjData{"Data/Stage/SimpleNaturePack/Models/Ground_03.mdl",    COLLISION_NONE,      RayCastON,      30.0f}},
    {"Mushroom_01", ObjData{"Data/Stage/SimpleNaturePack/Models/Mushroom_01.mdl",  COLLISION_NONE,      RayCastOFF}},
    {"Mushroom_02", ObjData{"Data/Stage/SimpleNaturePack/Models/Mushroom_02.mdl",  COLLISION_NONE,      RayCastOFF}},
    {"Rock_01",     ObjData{"Data/Stage/SimpleNaturePack/Models/Rock_01.mdl",      COLLISION_SPHERE,    RayCastOFF,     3.0f,1.0f,1.0f,1.0f}},
    {"Rock_02",     ObjData{"Data/Stage/SimpleNaturePack/Models/Rock_02.mdl",      COLLISION_SPHERE,    RayCastOFF,     1.2f,1.0f,1.0f,1.0f}},
    {"Rock_03",     ObjData{"Data/Stage/SimpleNaturePack/Models/Rock_03.mdl",      COLLISION_SPHERE,    RayCastOFF,     0.6f,1.0f,1.0f,1.0f}},
    {"Rock_04",     ObjData{"Data/Stage/SimpleNaturePack/Models/Rock_04.mdl",      COLLISION_SPHERE,    RayCastOFF,     0.2f,1.0f,1.0f,1.0f}},
    {"Rock_05",     ObjData{"Data/Stage/SimpleNaturePack/Models/Rock_05.mdl",      COLLISION_SPHERE,    RayCastOFF,     1.0f,1.0f,1.0f,1.0f}},
    {"Stump_01",    ObjData{"Data/Stage/SimpleNaturePack/Models/Stump_01.mdl",     COLLISION_SPHERE,    RayCastOFF,     0.5f,1.0f,1.0f,1.0f}},
    {"Tree_01",     ObjData{"Data/Stage/SimpleNaturePack/Models/Tree_01.mdl",      COLLISION_CYLINDER,  RayCastOFF,     0.5f,1.0f,6.0f,1.0f}},
    {"Tree_02",     ObjData{"Data/Stage/SimpleNaturePack/Models/Tree_02.mdl",      COLLISION_CYLINDER,  RayCastOFF,     0.5f,1.0f,6.0f,1.0f}},
    {"Tree_03",     ObjData{"Data/Stage/SimpleNaturePack/Models/Tree_03.mdl",      COLLISION_CYLINDER,  RayCastOFF,     0.5f,1.0f,6.0f,1.0f}},
    {"Tree_04",     ObjData{"Data/Stage/SimpleNaturePack/Models/Tree_04.mdl",      COLLISION_CYLINDER,  RayCastOFF,     0.5f,1.0f,6.0f,1.0f}},
    {"Tree_05",     ObjData{"Data/Stage/SimpleNaturePack/Models/Tree_05.mdl",      COLLISION_CYLINDER,  RayCastOFF,     0.5f,1.0f,6.0f,1.0f}},
    };