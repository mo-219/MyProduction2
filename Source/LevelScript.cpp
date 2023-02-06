#include "LevelScript.h"

// マネージャーインクルード
#include "EnemyManager.h"

// 読み込みたいモデルインクルード
#include "EnemySlime.h"



LevelScript level1_script[] = {
    SET_MAX_POS(70,70,70),
    SET_MIN_POS(-70,-70,-70),
    SET_RESPAWN(3,0,3),


    //position 生成フレーム
    SET_RED(0,0,15, 2),
    SET_GOLEM(0,0,1,5),
    SET_SLIME(10,0,20, 10),

    SET_END
};

LevelScript level2_script[] = {
    SET_MAX_POS(70,70,70),
    SET_MIN_POS(-70,-70,-70),
    SET_RESPAWN(3,0,3),

    SET_RED     (0,0,15,    2),
    SET_GOLEM   (0,0,1,     5),
    SET_GOLEM   (10,0,1,     7),
    SET_GOLEM   (5,0,11,     8),
    SET_SLIME   (10,0,20,   10),


    ////position 生成フレーム
    //SET_SLIME(0,0,15, 2),
    //SET_SLIME(10,0,20, 10),

    SET_END
};

LevelScript* all_script[] = {
    level1_script,
    level2_script,
    nullptr,
};