#include "StageDoor.h"
#include "EnemyManager.h"
#include "LevelManager.h"

StageDoor::StageDoor()
{
    model = new Model("Data/Model/Gate/gate.mdl");
    scale = { 1,1,1 };

    //width = ;
    //height;
    //depth;
}

StageDoor::~StageDoor()
{
    delete model;
}

void StageDoor::Update(float elapsedTime)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    LevelManager& levelManager = LevelManager::Instance();


    if (enemyManager.GetEnemyCount() <= 0 && levelManager.IsEndLevel())
    {
        goalFlag = true;
        collisionFlag = false;
    }

    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageDoor::Render(ID3D11DeviceContext* dc, Shader* shader)
{

}

void StageDoor::Render(const RenderContext& rc, ModelShader* shader)
{
    SetRenderContext(rc);
    shader->Draw(rc, model);
}



RenderContext StageDoor::SetRenderContext(const RenderContext& rc)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;

    return myRc;
}

void StageDoor::UpdateTransform()
{
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.z, angle.y, angle.z);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y+scale.y/2, position.z);
    DirectX::XMMATRIX W = S * R * T;

    DirectX::XMStoreFloat4x4(&transform, W);

}





