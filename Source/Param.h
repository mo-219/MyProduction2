#pragma once

#include <DirectXMath.h>

struct Param
{
    DirectX::XMFLOAT3 position;
    float radius;

    float width;    // X•
    float height;   // Y•
    float depth;    // Z•

    float rayCastRadius = 5.0f;
};
