#pragma once

#include "LevelScript.h"
#include <DirectXMath.h>

#include "Effect.h"


class LevelManager
{
private:
    LevelManager();
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
    void LoadScript(LevelScript* p);
    void Update(float elapsedTime);

    void Clear();

    void SetLevelScript(int a) { pLevelScript = ppLevelScript[a]; }

    bool IsEndLevel() { return EndFlag; }

    DirectX::XMFLOAT3 GetRespawnPos(){ return respawnPos; }
    DirectX::XMFLOAT3 GetMaxPos(){ return maxPos; }
    DirectX::XMFLOAT3 GetMinPos(){ return minPos; }

    int GetStageMax() { return stageSize; }
private:
    LevelScript* pLevelScript;          // 1ステージごとのステージ情報を所持するポインター
    LevelScript** ppLevelScript;        // 全ステージ情報を所持したポインター

    float timer = 0.0f;      // エネミー生成用タイマー
    int currentStageNum;
    int stageSize = 1;

    Effekseer::Handle generateEffectHandle = 0;
    Effect* generateEffect = nullptr;

    bool EndFlag = false;

    DirectX::XMFLOAT3 respawnPos = {};
    DirectX::XMFLOAT3 maxPos = {};
    DirectX::XMFLOAT3 minPos = {};

};