#pragma once
// 浮動小数算術
class Mathf
{
public:
    // 線形保管
    static float Lerp(float a, float b, float t);

    // 指定範囲のランダム値を計算する
    static float RandomRange(float min, float max);

    // 指定範囲のランダム値を計算する
    static int RandomRange(int min, int max);

    // 指定範囲を超えたら直す
    static float clamp(float min, float max, float num);


};