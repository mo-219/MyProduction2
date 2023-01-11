#include <imgui.h>
#include "Sword.h"
#include "Collision.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"



Sword::Sword(const char* filename)
{
    model = new Model(filename);
    scale.x = 100.0f;
    scale.y = 100.0f;
    scale.z = 100.0f;
}

Sword::~Sword()
{
    delete model;

}

void Sword::Update(float elapsedTime)
{
    SetCollisionValue();

    // モデル行列更新
    model->UpdateTransform(transform);
    

}

void Sword::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void Sword::Render(const RenderContext& rc, ModelShader* shader)
{
    // 送るデータ挿入
    RenderContext prc = rc;
    prc.cubicColorData.shaderFlag = CUBIC_CUSTOMIZE;
    prc.cubicColorData.rightVec = DirectX::XMFLOAT4{ transform._11, transform._12, transform._13, transform._14 };
    prc.cubicColorData.topVec = DirectX::XMFLOAT4{ transform._21, transform._22, transform._23, transform._24 };
    prc.cubicColorData.frontVec = DirectX::XMFLOAT4{ transform._31, transform._32, transform._33, transform._34 };

    prc.cubicColorData.colorTop1 = colorTop1;
    prc.cubicColorData.colorBottom1 = colorBottom1;
    prc.cubicColorData.colorRight1 = colorRight1;
    prc.cubicColorData.colorLeft1 = colorLeft1;
    prc.cubicColorData.colorBack1 = colorBack1;
    prc.cubicColorData.colorFront1 = colorFront1;

    prc.cubicColorData.colorTop2 = colorTop2;
    prc.cubicColorData.colorBottom2 = colorBottom2;
    prc.cubicColorData.colorRight2 = colorRight2;
    prc.cubicColorData.colorLeft2 = colorLeft2;
    prc.cubicColorData.colorBack2 = colorBack2;
    prc.cubicColorData.colorFront2 = colorFront2;

    prc.cubicColorData.colorAlpha = colorAlpha;

    shader->Draw(prc, model);

}

void Sword::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Sword", nullptr, ImGuiWindowFlags_None))
    {
        // トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 位置
            ImGui::InputFloat3("Position", &param.position.x);
            DirectX::XMFLOAT4X4 nodeTransform = model->FindNode("Sword")->worldTransform;
#if 0
            ImGui::InputFloat("Transform11", &nodeTransform._11); 
            ImGui::InputFloat("Transform12", &nodeTransform._12); 
            ImGui::InputFloat("Transform13", &nodeTransform._13); 
            ImGui::InputFloat("Transform14", &nodeTransform._14); 
            ImGui::InputFloat("Transform21", &nodeTransform._21); 
            ImGui::InputFloat("Transform22", &nodeTransform._22); 
            ImGui::InputFloat("Transform23", &nodeTransform._23); 
            ImGui::InputFloat("Transform24", &nodeTransform._24); 
            ImGui::InputFloat("Transform31", &nodeTransform._31); 
            ImGui::InputFloat("Transform32", &nodeTransform._32); 
            ImGui::InputFloat("Transform33", &nodeTransform._33); 
            ImGui::InputFloat("Transform34", &nodeTransform._34); 
            ImGui::InputFloat("Transform41", &nodeTransform._41); 
            ImGui::InputFloat("Transform42", &nodeTransform._42); 
            ImGui::InputFloat("Transform43", &nodeTransform._43); 
            ImGui::InputFloat("Transform44", &nodeTransform._44); 
#endif
            // 回転
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);

            // スケール
            ImGui::InputFloat3("Scale", &scale.x);
            ImGui::Separator();
            ImGui::Text("CubicColor");





        }
    }
    ImGui::End();

}

void Sword::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    for (int i = 0; i < collisionNum; i++)
    {
        debugRenderer->DrawSphere(collisionPos[i], collisionRange, DirectX::XMFLOAT4(1, 0, 0, 1));
    }


}

void Sword::UpdateTransform(DirectX::XMFLOAT4X4 handMat)
{

    DirectX::XMMATRIX ParentWorld = DirectX::XMLoadFloat4x4(&handMat);

    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);


    DirectX::XMMATRIX Local = S * R ;
    DirectX::XMMATRIX World = DirectX::XMMatrixMultiply(Local, ParentWorld);

    DirectX::XMStoreFloat4x4(&transform, World);

    param.position.x = transform._41;
    param.position.y = transform._42;
    param.position.z = transform._43;
}

void Sword::SetCollisionValue()
{
    DirectX::XMFLOAT3 vec = DirectX::XMFLOAT3(tipPos.x - param.position.x, tipPos.y - param.position.y, tipPos.z - param.position.z);
    float len = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

    vec.x /= len;
    vec.y /= len;
    vec.z /= len;
          
    vec.x *= collisionRange * 2;
    vec.y *= collisionRange * 2;
    vec.z *= collisionRange * 2;


    collisionPos[0].x = param.position.x + vec.x;
    collisionPos[0].y = param.position.y + vec.y;
    collisionPos[0].z = param.position.z + vec.z;

    for (int i = 1; i < collisionNum; i++)
    {
        collisionPos[i].x = collisionPos[i-1].x + vec.x;
        collisionPos[i].y = collisionPos[i-1].y + vec.y;
        collisionPos[i].z = collisionPos[i-1].z + vec.z;
    }

}

void Sword::collisionWeaponVSEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての弾丸とすべての敵を総当たりで衝突処理
    int enemyCount = enemyManager.GetEnemyCount();
    for(int i = 0; i < collisionNum;i++)
    {
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // 衝突処理
            DirectX::XMFLOAT3 outPosition;
            if (Collision::InstersectSphereVsCylinder(
                collisionPos[i],
                collisionRange,
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPosition))
            {
                // ダメージを与える
                if (enemy->ApplyDamage(1, 0.5f))
                {
                    // 吹き飛ばす
                    {
                        DirectX::XMFLOAT3 impulse;
                        const float power = 10.0f;

                        const DirectX::XMFLOAT3& e = enemy->GetPosition();
                        const DirectX::XMFLOAT3& p = collisionPos[i];

                        float vx = e.x - p.x;
                        float vz = e.z - p.z;

                        float length = sqrtf(vx * vx + vz * vz);
                        vx /= length;
                        vz /= length;

                        impulse.x = vx * power;
                        impulse.y = 0.5f * power;
                        impulse.z = vz * power;


                        enemy->AddImpulse(impulse);
                    }

                    // ヒットエフェクト再生
                    {
                        DirectX::XMFLOAT3 e = enemy->GetPosition();
                        e.y += enemy->GetHeight() * 0.5f;

                    }

                }
            }
        }
    }
}


