#include "LevelManager.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EnemyGolem.h"
#include "EnemyRed.h"

LevelManager::~LevelManager()
{
    pLevelScript = nullptr;
    ppLevelScript = nullptr;
}

void LevelManager::Initialize(LevelScript** pp)
{
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

void LevelManager::load_script(LevelScript* script)
{
    // 一応ぬるぽちぇっく
    if (script == nullptr) return;


    // manager
    EnemyManager& enemyManager = EnemyManager::Instance();

    // new用ポインタ宣言
    Enemy* enemy;

    switch (script->objType)
    {
    case OBJ_TYPE::END:
        break;

    case OBJ_TYPE::RESPAWN_POINT:
        respawnPos = script->pos;
        break;
    
    
    case OBJ_TYPE::ENEMY_SLIME:
        enemy = new EnemySlime();
        enemy->setPosition(script->pos);
        enemy->SetTerritory(script->pos, script->territoryRange);

        enemyManager.Register(enemy);
        break;

    case OBJ_TYPE::ENEMY_GOLEM:
        enemy = new EnemyGolem();
        enemy->setPosition(script->pos);
        enemy->SetTerritory(script->pos, script->territoryRange);

        enemyManager.Register(enemy);
        break;

    case OBJ_TYPE::ENEMY_RED:
        enemy = new EnemyRed();
        enemy->setPosition(script->pos);
        enemy->SetTerritory(script->pos, script->territoryRange);

        enemyManager.Register(enemy);
    }

    // 一応ぬるぽ入れておく
    enemy = nullptr;
}

void LevelManager::update(float elapsedTime)
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
            load_script(pLevelScript);
            pLevelScript++;
        }
    }

}
