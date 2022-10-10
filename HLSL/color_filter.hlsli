struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer COLOR_FILTER : register(b4)
{
    float hueShift;     // �F������
    float saturation;   // �ʓx����
    float brightness;   // ���x����
    float dummy;
};
