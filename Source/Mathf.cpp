#include "Mathf.h"

#include <stdlib.h>

float Mathf::Lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

// 指定範囲のランダム値を計算する
float Mathf::RandomRange(float min, float max)
{
    return ((float)rand() / (float)RAND_MAX * (max - min)) + min;
}

float Mathf::clamp(float min, float max, float num)
{
    float value = num;
    if (num < min)   value = min;
    if (num > max)   value = max;

    return value;
}

