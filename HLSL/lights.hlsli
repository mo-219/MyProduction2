// �_����
struct point_lights
{
    float4 position;
    float4 color;
    float range;
    float3 dummy;
};

// �X�|�b�g���C�g
struct spot_lights
{
    float4 position;
    float4 direction;
    float4 color;
    float  range;
    float  innerCorn;
    float  outerCorn;
    float  dummy;
};