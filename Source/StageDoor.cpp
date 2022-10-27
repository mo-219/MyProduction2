#include "StageDoor.h"
#include "EnemyManager.h"

void StageDoor::Update(float elapsedTime)
{
    EnemyManager& enemyManager = EnemyManager::Instance();


    if (enemyManager.GetEnemyCount() > 0)   return;

    goalFlag = true;
    collisionFlag = false;
}

void StageDoor::Render(ID3D11DeviceContext* dc, Shader* shader)
{

}

void StageDoor::Render(const RenderContext& rc, ModelShader* shader)
{
}

RenderContext StageDoor::SetRenderContext(const RenderContext& rc)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;

    return myRc;
}





