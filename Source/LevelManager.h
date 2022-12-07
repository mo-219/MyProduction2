#pragma once

#include "LevelScript.h"
#include <DirectXMath.h>


class LevelManager
{
private:
    LevelManager() {}
    ~LevelManager();

public:
    // 唯一のインスタンス取得
    static LevelManager& Instance()
    {
        static LevelManager instance;
        return instance;
    }
    void Initialize(LevelScript** pp);
    void Initialize(int stageNum);
    void load_script(LevelScript* p);
    void update(float elapsedTime);

    void SetLevelScript(int a) { pLevelScript = ppLevelScript[a]; }

    bool IsEndLevel() { return EndFlag; }

    DirectX::XMFLOAT3 GetRespawnPos(){ return respawnPos; }

    int GetStageMax() { return stageSize; }
private:
    LevelScript* pLevelScript;          // 1ステージごとのステージ情報を所持するポインター
    LevelScript** ppLevelScript;        // 全ステージ情報を所持したポインター

    float timer = 0.0f;      // エネミー生成用タイマー
    int currentStageNum;
    int stageSize = 1;

    bool EndFlag = false;

    DirectX::XMFLOAT3 respawnPos = {};
};