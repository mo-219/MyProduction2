#pragma once

#include "LevelScript.h"
#include <DirectXMath.h>


class LevelManager
{
private:
    LevelManager() {}
    ~LevelManager();

public:
    // �B��̃C���X�^���X�擾
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
    LevelScript* pLevelScript;          // 1�X�e�[�W���Ƃ̃X�e�[�W������������|�C���^�[
    LevelScript** ppLevelScript;        // �S�X�e�[�W�������������|�C���^�[

    float timer = 0.0f;      // �G�l�~�[�����p�^�C�}�[
    int currentStageNum;
    int stageSize = 1;

    bool EndFlag = false;

    DirectX::XMFLOAT3 respawnPos = {};
};