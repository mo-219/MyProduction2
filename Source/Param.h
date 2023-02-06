#pragma once

#include <DirectXMath.h>

struct Param
{
    DirectX::XMFLOAT3 position = {};
    float radius = 0;

    float width = 0;    // X•
    float height = 0;   // Y•
    float depth = 0;    // Z•

    float rayCastRadius = 5.0f;
};
