#include "GameObjectManager.h"
#include "Graphics/Graphics.h"

#include "Param.h"

#include <algorithm>

ObjectManager::~ObjectManager()
{
    Clear();
}


// 更新処理
void ObjectManager::Update(float elapsedTime)
{
    for (GameObject* obj : objects)
    {
        obj->Update(elapsedTime);
    }

    // 破棄処理
    for (GameObject* obj : removes)
    {
        std::vector<GameObject*>::iterator it =
            std::find(objects.begin(), objects.end(), obj);

        if (it != objects.end())
        {
            objects.erase(it);
        }
        // 弾丸の破棄処理
        delete obj;
    }
    // 破棄リストをクリア
    removes.clear();

}

// RayCast以外の当たり判定
void ObjectManager::Collision(Param paramB, DirectX::XMFLOAT3& out)
{
    for (GameObject* obj : objects)
    {
        Param scaleFix = obj->GetParam();
        scaleFix.radius *= (obj->GetScale().x + obj->GetScale().y + obj->GetScale().z) / 3;
        obj->collision->collision(scaleFix, paramB, out);
    }
    
}

bool ObjectManager::RayCast(Param paramB, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, HitResult& hit)
{

    bool result = false;

    hit.distance = FLT_MAX;
    //for (GameObject* obj : objects)
    for(int i = 0;i < GetObjectCount();i++)
    {
        GameObject* obj = GetObj(i);

        if (obj->GetRayCast() == &noneRayCastBehavior) continue;
        if (paramB.rayCastRadius <= 0) continue;
        DirectX::XMFLOAT3 dummy{};

        //if (Collision::InstersectSphereVsSphere(obj->GetPosition(), obj->GetRadius(),
        //    paramB.position, paramB.rayCastRadius, dummy))
        //{
        //    obj->CollisionFlag = true;
        //}
        //else
        //{
        //    obj->CollisionFlag = false;
        //}

        if (Collision::InstersectSphereVsSphere(obj->GetPosition(), obj->GetRadius(),
            paramB.position, paramB.rayCastRadius, dummy))
        {
            HitResult tmp;
            if (obj->GetRayCast()->collision(start,end,obj->GetModel(), tmp))
            {
                if (hit.distance > tmp.distance)
                {
                    hit = tmp;
                    result = true;
                }
            }
        }
    }

    return result;
}

// デバッグ用当たり判定(上と大体一緒)
bool ObjectManager::RayCast(Param paramB, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, HitResult& hit, int name)
{
    bool result = false;

    hit.distance = FLT_MAX;
    //for (GameObject* obj : objects)
    for (int i = 0; i < GetObjectCount(); i++)
    {
        GameObject* obj = GetObj(i);

        if (obj->GetRayCast() == &noneRayCastBehavior) continue;
        if (paramB.rayCastRadius <= 0) continue;
        DirectX::XMFLOAT3 dummy{};

        //if (name == 0)
        //{
        //    if (Collision::InstersectSphereVsSphere(obj->GetPosition(), obj->GetRadius(),
        //        paramB.position, paramB.rayCastRadius, dummy))
        //    {
        //        obj->CollisionFlag = true;
        //    }
        //    else
        //    {
        //        obj->CollisionFlag = false;
        //    }
        //}
        if (Collision::InstersectSphereVsSphere(obj->GetPosition(), obj->GetRadius(),
            paramB.position, paramB.rayCastRadius, dummy))

        {
            HitResult tmp;
            if (obj->GetRayCast()->collision(start, end, obj->GetModel(), tmp))
            {
                if (hit.distance > tmp.distance)
                {
                    hit = tmp;
                    result = true;
                }
            }
        }
    }

    return result;
}

int ObjectManager::findNear(DirectX::XMFLOAT3 p)
{
    float min = FLT_MAX;
    int num = 0;
    for (int i=0; i < GetObjectCount();i++)
    {
        GameObject* obj = GetObj(i);
        if (obj->GetRayCast() == &noneRayCastBehavior) continue;
        DirectX::XMFLOAT3 objPos = obj->GetPosition();
        float length = (p.x - objPos.x) * (p.x - objPos.x) + (p.y - objPos.y) * (p.y - objPos.y) + (p.z - objPos.z) * (p.z - objPos.z);

        if (min > length)
        {
            min = length;
            num = i;
        }
    }

    return num;
}

void ObjectManager::UpdateOnlyTransform(float elapsedTime)
{
    for (GameObject* obj : objects)
    {
        obj->UpdateOnlyTransform(elapsedTime);
    }
}

void ObjectManager::Initialize()
{
    for (GameObject* obj : objects)
    {
        DirectX::XMFLOAT3 scale = obj->GetScale();
        float maxValue = (std::max)(scale.x, (std::max)(scale.y, scale.z));

        obj->SetRadius(obj->GetRadius() * maxValue);
    }
}

// 描画処理
void ObjectManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    for (GameObject* obj : objects)
    {
        obj->Render(context, shader);
    }
    //GameObject* obj = GetObj(count);
    //obj->Render(context, shader);
}
void ObjectManager::Render(const RenderContext& rc, ModelShader* shader)
{
    for (GameObject* obj : objects)
    {
        //GameObject* obj = GetObj(count);


        RenderContext myRc = obj->SetRenderContext(rc);
        obj->Render(myRc, shader);

    }
}

// オブジェクト登録
void ObjectManager::Register(GameObject* obj)
{
    objects.emplace_back(obj);
}

// オブジェクト全削除
void ObjectManager::Clear()
{
    for (GameObject* obj : objects)
    {
        if (obj != nullptr)
        {
            delete obj;
            obj = nullptr;
        }
    }
    objects.clear();
}

void ObjectManager::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();



    DirectX::XMFLOAT4 color = { 1,0,0,1 };

    // 衝突判定用のデバッグ円柱を描画
    for (GameObject* obj : objects)
    {
        if (obj->CollisionFlag)color = { 0,1,1,1 };
        if (obj->collision == &cubeBehavior)
        {
            debugRenderer->DrawCube(obj->GetPosition(), obj->GetWidth(), obj->GetHeight(), obj->GetDepth(), color);

        }
        if (obj->collision == &sphereBehavior)
        {
            debugRenderer->DrawSphere(obj->GetPosition(), obj->GetRadius(), color);
        }

        if (obj->collision == &cylinderBehavior)
        {
            debugRenderer->DrawCylinder(obj->GetPosition(), obj->GetRadius(), obj->GetHeight(), color);
        }
        //if (obj->GetCode() == "Ground_01")
        //{
        //    debugRenderer->DrawSphere(obj->GetPosition(), obj->GetRadius(), color);
        //}        
        //if (obj->GetCode() == "Ground_02")
        //{
        //    debugRenderer->DrawSphere(obj->GetPosition(), obj->GetRadius(), color);
        //}        
        //if (obj->GetCode() == "Ground_03")
        //{
        //    debugRenderer->DrawSphere(obj->GetPosition(), obj->GetRadius(),color);
        //}

    ////        for (GameObject* obj : objects)
    ////    {
    ////        Param scaleFix = obj->GetParam();
    ////        scaleFix.radius *= (obj->GetScale().x + obj->GetScale().y + obj->GetScale().z) / 3;
    ////        scaleFix.width *= obj->GetScale().x;
    ////        scaleFix.height *= obj->GetScale().y;
    ////        scaleFix.depth *= obj->GetScale().z;


    ////        if (obj->behavior == &cubeBehavior)
    ////        {
    ////            debugRenderer->DrawCube(scaleFix.position, scaleFix.width, scaleFix.height, scaleFix.depth, DirectX::XMFLOAT4(1, 0, 0, 1));

    ////        }
    ////        if (obj->behavior == &sphereBehavior)
    ////        {
    ////            debugRenderer->DrawSphere(scaleFix.position, scaleFix.radius, DirectX::XMFLOAT4(1, 0, 0, 1));
    ////        }

    ////        if (obj->behavior == &cylinderBehavior)
    ////        {
    ////            debugRenderer->DrawCylinder(scaleFix.position, scaleFix.radius, scaleFix.height, DirectX::XMFLOAT4(1, 0, 0, 1));
    ////        }
    ////        if (obj->GetCode() == "Ground_01")
    ////        {
    ////            debugRenderer->DrawSphere(scaleFix.position, scaleFix.radius, DirectX::XMFLOAT4(1, 0, 1, 1));
    ////        }

        obj->DrawDebugPrimitive();
    }

}

void ObjectManager::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Object", nullptr, ImGuiWindowFlags_None))
    {
        //for (GameObject* obj : objects)
        //{
        //    if(
        //    obj->DrawDebugImGui();
        //}
        GetObj(count)->DrawDebugImGui();
        int a = GetObjectCount();
        ImGui::InputInt("EnemyCount", &a);
        ImGui::InputInt("count", &count);
    }
    ImGui::End();

}

//void ObjectManager::CollisionEnemyVsEnemies()
//{
//    ObjectManager& enemyManager = ObjectManager::Instance();
//
//    // 全ての敵と総当たりで衝突処理
//    int enemyCount = enemyManager.GetObjectCount();
//    for (int j = 0; j < enemyCount; ++j)
//    {
//        GameObject* enemy1 = enemyManager.GetEnemy(j);
//
//        for (int i = j + 1; i < enemyCount; ++i)
//        {
//            if (i == j)  continue;
//
//            DirectX::XMFLOAT3 outPosition{};
//            GameObject* enemy2 = enemyManager.GetEnemy(i);
//
//
//            if (Collision::InstersectCylinderVsCylinder(enemy1->GetPosition(), enemy1->GetRadius(), enemy1->GetHeight(),
//                enemy2->GetPosition(), enemy2->GetRadius(), enemy2->GetHeight(),
//                outPosition))
//            {
//                // 押し出し後の位置設定
//                enemyManager.GetEnemy(i)->setPosition({ outPosition.x,
//                                                        outPosition.y,
//                                                        outPosition.z });
//
//            }
//        }
//    }
//}

void ObjectManager::Remove(GameObject* enemy)
{
    // 破棄リストに追加
    removes.insert(enemy);
}

