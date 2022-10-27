#include "Mathf.h"

#include <stdlib.h>

float Mathf::Lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

// �w��͈͂̃����_���l���v�Z����
float Mathf::RandomRange(float min, float max)
{
    return ((float)rand() / (float)RAND_MAX * (max - min)) + min;
}
