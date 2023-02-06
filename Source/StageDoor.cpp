#include "StageDoor.h"
#include "EnemyManager.h"
#include "LevelManager.h"

StageDoor::StageDoor()
{
    model = new Model("Data/Model/Gate/gate.mdl");
    scale = { 1,1,1 };

    se = Audio::Instance().LoadAudioSource("Data/Audio/SE/Gate.wav");
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
        TraditionOpenDoor();
        goalFlag = true;
        collisionFlag = false;
    }

    UpdateTransform();
    model->UpdateAnimation(elapsedTime);
    model->UpdateTransform(transform);
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

void StageDoor::TraditionOpenDoor()
{
    if (!collisionFlag)   return;
    model->PlayAnimation(Anim_Open, false, 1.0f);
    se->Play(false);
}





