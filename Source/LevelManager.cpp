#include "LevelManager.h"
#include "StageManager.h"
#include "EnemyManager.h"

#include "EnemySlime.h"
#include "EnemyGolem.h"
#include "EnemyRed.h"
#include "EnemyBoss.h"

#include "StageDoor.h"

LevelManager::LevelManager()
{
    generateEffect = new Effect("Data/Effect/Generate2.efk");
}

LevelManager::~LevelManager()
{
    delete generateEffect;
    Clear();
}

void LevelManager::Initialize(LevelScript** pp)
{
    if (ppLevelScript != nullptr)
    {
        delete ppLevelScript;
        ppLevelScript = nullptr;
    }
    ppLevelScript = pp;
    currentStageNum = 0;
    pLevelScript = ppLevelScript[currentStageNum];
    timer = 0;
    EndFlag = false;

}

void LevelManager::Initialize(int stageNum)
{
    currentStageNum = stageNum;
    pLevelScript = ppLevelScript[currentStageNum];
    timer = 0;
    EndFlag = false;
}

void LevelManager::Clear()
{
    if (pLevelScript != nullptr)
    {
        pLevelScript = nullptr;
        delete pLevelScript;
    }
    if (ppLevelScript != nullptr)
    {
        ppLevelScript = nullptr;
        delete ppLevelScript;
    }
}

void LevelManager::LoadScript(LevelScript* script)
{
    // 一応ぬるぽちぇっく
    if (script == nullptr) return;


    // manager
    EnemyManager& enemyManager = EnemyManager::Instance();
    StageManager& stageManager = StageManager::Instance();

    // new用ポインタ宣言
    Enemy* enemy = {};

    DirectX::XMFLOAT3 pos = script->pos;

    switch (script->objType)
    {
    case OBJ_TYPE::END:
        
        break;

    case OBJ_TYPE::RESPAWN_POINT:
        respawnPos = pos;
        break;
    
    
    case OBJ_TYPE::ENEMY_SLIME:
        enemy = new EnemySlime();
        enemy->SetPosition(pos);
        enemy->SetTerritory(pos, script->territoryRange);
        enemy->SetMaxPos(maxPos);
        enemy->SetMinPos(minPos);

        enemyManager.Register(enemy);

        // 生成エフェクト再生
        pos.y += 0.5f;
        generateEffectHandle = generateEffect->Play(pos);
        generateEffect->SetScale(generateEffectHandle, DirectX::XMFLOAT3(1.5f, 2, 1.5f));
        break;

    case OBJ_TYPE::ENEMY_GOLEM:
        enemy = new EnemyGolem();
        enemy->SetPosition(pos);
        enemy->SetTerritory(pos, script->territoryRange);
        enemy->SetMaxPos(maxPos);
        enemy->SetMinPos(minPos);

        enemyManager.Register(enemy);

        // 生成エフェクト再生
        pos.y += 0.5f;
        generateEffectHandle = generateEffect->Play(pos);
        generateEffect->SetScale(generateEffectHandle, DirectX::XMFLOAT3(3, 3, 3));
        break;

    case OBJ_TYPE::ENEMY_RED:
        enemy = new EnemyRed();
        enemy->SetPosition(pos);
        enemy->SetTerritory(pos, script->territoryRange);
        enemy->SetMaxPos(maxPos);
        enemy->SetMinPos(minPos);

        enemyManager.Register(enemy);

        // 生成エフェクト再生
        pos.y += 0.5f;
        generateEffectHandle = generateEffect->Play(pos);
        generateEffect->SetScale(generateEffectHandle, DirectX::XMFLOAT3(1.5f, 2, 1.5f));
        break;

    case OBJ_TYPE::ENEMY_BOSS:
        enemy = new EnemyBoss();
        enemy->SetPosition(pos);
        enemy->SetTerritory(pos, script->territoryRange);
        enemy->SetMaxPos(maxPos);
        enemy->SetMinPos(minPos);

        enemyManager.Register(enemy);


        // 生成エフェクト再生
        pos.y += 0.5f;
        generateEffectHandle = generateEffect->Play(pos);
        generateEffect->SetScale(generateEffectHandle, DirectX::XMFLOAT3(2, 2, 2));
        break;
    
    case OBJ_TYPE::DOOR:
        //enemy = new EnemyRed();
        //enemy->SetPosition(script->pos);
        //enemy->SetTerritory(script->pos, script->territoryRange);
        //enemy->SetMaxPos(maxPos);
        //enemy->SetMinPos(minPos);

        //enemyManager.Register(enemy);
        break;


    case OBJ_TYPE::MAX_POS:
        maxPos = pos;
        break;

    case OBJ_TYPE::MIN_POS:
        minPos = pos;
        break;
    }

    // 一応ぬるぽ入れておく
    enemy = nullptr;
}

void LevelManager::Update(float elapsedTime)
{
    timer += 1 * elapsedTime;
     
    // pLevelScriptヌルはエンドコード
    if (pLevelScript->objType == OBJ_TYPE::END)
    {
        EndFlag = true;
        return;
    }

    if (timer >= pLevelScript->frame)
    {
        for (; timer >= pLevelScript->frame;)
        {
            if (pLevelScript->objType == OBJ_TYPE::END) return;
            LoadScript(pLevelScript);
            pLevelScript++;
        }
    }

}
