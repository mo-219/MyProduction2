#pragma once

#include <DirectXMath.h>
#define SET_OBJ(x,y,z, frm, range, type)        {{(x),(y),(z)}, (frm), (range), (type), false}

#define SET_RESPAWN(x,y,z, frm, range, type)        {{(x),(y),(z)}, (-1.0f), (-1.0f), (OBJ_TYPE::RESPAWN_POINT)}
#define SET_SLIME(x,y,z, frm, range, type)        {{(x),(y),(z)}, (frm), (10.0f), (OBJ_TYPE::ENEMY_SLIME)}
#define SET_GOLEM(x,y,z, frm, range, type)        {{(x),(y),(z)}, (frm), (10.0f), (OBJ_TYPE::ENEMY_GOLEM)}
#define SET_RED(x,y,z, frm, range, type)        {{(x),(y),(z)}, (frm), (10.0f), (OBJ_TYPE::ENEMY_RED)}
#define SET_BLUE(x,y,z, frm, range, type)        {{(x),(y),(z)}, (frm), (10.0f), (OBJ_TYPE::ENEMY_BLUE)}



#define SET_END      {{(0),(0),(0)}, -1, -1, (OBJ_TYPE::END)}


enum OBJ_TYPE
{
    END = -1,
    RESPAWN_POINT = 0,
    ENEMY_SLIME,
    ENEMY_GOLEM,
    ENEMY_RED,
    ENEMY_BLUE
};

class LevelScript
{
public:
    DirectX::XMFLOAT3 pos = {}; // 生成位置
    float frame;
    float territoryRange;

    OBJ_TYPE objType;           // 生成キャラクタータイプ
};

//void load_script(LevelScript* script, float frame);

extern LevelScript level1_script[];
extern LevelScript* all_script[];       // 全てのスクリプトのポインターを持ってる配列

