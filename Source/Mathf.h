#pragma once
// ���������Z�p
class Mathf
{
public:
    // ���`�ۊ�
    static float Lerp(float a, float b, float t);

    // �w��͈͂̃����_���l���v�Z����
    static float RandomRange(float min, float max);

    // �w��͈͂̃����_���l���v�Z����
    static int RandomRange(int min, int max);

    // �w��͈͂𒴂����璼��
    static float clamp(float min, float max, float num);


};