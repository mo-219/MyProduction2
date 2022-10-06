

// �Z�����F�p�\����
struct HexColor
{
    float4 colorTop;
    float4 colorBottom;
    float4 colorRight;
    float4 colorLeft;
    float4 colorBack;
    float4 colorFront;
};


//-----------------------------------------------
//  �����o�[�g�g�U���ˌv�Z�֐�
//-----------------------------------------------
//  N: �@��(���K���ς�)
//  L: ���˃x�N�g��(���K���ς�)
//  C: ���ˌ�(�F�A����)
//  K: ���˗�
//-----------------------------------------------
float3 CalcLambert(float3 N, float3 L, float3 C, float3 K)
{
    float power = saturate(dot(N, -L));
    return C * power * K;
}


//-----------------------------------------------
//  �t�H���̋��ʔ��ˌv�Z�֐�
//-----------------------------------------------
//  N: �@��(���K���ς�)
//  L: ���˃x�N�g��(���K���ς�)
//  E: �����x�N�g��(���K���ς�)
//  C: ���ˌ�(�F�A����)
//  K: ���˗�
//-----------------------------------------------
float3 CalcPhongSpeculer(float3 N, float3 L, float3 E, float3 C, float3 K)
{
    float3 R = reflect(L, N);
    float power = max(dot(-E, R), 0);
    power = pow(power, 128);
    
    return C * power * K.rgb;
}


//-----------------------------------------------
//  �n�[�t�����o�[�g�g�U���ˌv�Z�֐�
//-----------------------------------------------
//  N: �@��(���K���ς�)
//  L: ���˃x�N�g��(���K���ς�)
//  C: ���ˌ�(�F�A����)
//  K: ���˗�
//-----------------------------------------------
float3 CalcHalfLambert(float3 N, float3 L, float3 C, float3 K)
{
    float D = saturate(dot(N, -L) * 0.5f + 0.5f);
    
    return C * D * K;
}


//-----------------------------------------------
//  �������C�g
//-----------------------------------------------
//  N: �@��(���K���ς�)
//  E: ���������x�N�g��(���K���ς�)
//  L: ���˃x�N�g��(���K���ς�)
//  C: ���C�g�F
//�@RimPower: �������C�g�̋���
//-----------------------------------------------
float3 CalcRimLight(float3 N,float3 E,float3 L, float3 C, float3 RimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(N, -E));
    
    return C * pow(rim, RimPower) * saturate(dot(L, -E));
}


//--------------------------------------------
// �����v�V�F�[�f�B���O
//--------------------------------------------
// tex:�����v�V�F�[�f�B���O�p�e�N�X�`��
// samp:�����v�V�F�[�f�B���O�p�T���v���X�e�[�g
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�
//--------------------------------------------
float3 CalcRampShading(Texture2D tex, SamplerState samp, float3 N, float3 L, float3 C, float3 K)
{
    float D = saturate(dot(N, -L) * 0.5f + 0.5f);
    float Ramp = tex.Sample(samp, float2(D, 0.5f)).r;
    return C * Ramp * K.rgb;
}


//--------------------------------------------
// ���̊��}�b�s���O
//--------------------------------------------
// tex:�����v�V�F�[�f�B���O�p�e�N�X�`��
// samp:�����v�V�F�[�f�B���O�p�T���v���X�e�[�g
// color: ���݂̃s�N�Z���F
// N:�@��(���K���ς�)
// E:���ˌ�(�F�E����)
// value:�K����
//--------------------------------------------
float3 CalcSphereEnvironment(Texture2D tex, SamplerState samp, in float3 color,
                                                float3 N, float3 E, float value)
{
    float3 R = reflect(E, N);
    float2 texcoord = R.xy * 0.5f + 0.5f;
    
    return lerp(color.rgb, tex.Sample(samp, texcoord).rgb, value);
    
}


//--------------------------------------------
// �������C�e�B���O
//--------------------------------------------
// normal:�@��(���K���ς�)
// up:������i�Е��j
// sky_color:��(��)�F
// ground_color:�n��(��)�F
// hemisphere_weight:�d��
//--------------------------------------------

float3 CalcHemiSphereLight(float3 normal, float3 up, float3 sky_color, float3 ground_color, float4 hemisphere_weight)
{
    float facter = dot(normal, up) * 0.5f + 0.5f;
    return lerp(ground_color, sky_color, facter) * hemisphere_weight.x;
    
}


//--------------------------------------------
// �t�H�O
//--------------------------------------------
// color : ���݂̃s�N�Z���F
// fog_color : �t�H�O�̐F
// fog_range : �t�H�O�͈̔͏��
// eye_length : ���_����̋���
//--------------------------------------------
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
    float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
    
    return lerp(color, fog_color, fogAlpha);
}

//--------------------------------------------
//  cubic color
//--------------------------------------------
// color : ���݂̃s�N�Z���F
// N : �@��
// 
// colorTop,     : ��̐F
// colorBottom,  : ���̐F
// colorRight,   : �E�̐F
// colorLeft,    : ���̐F
// colorBack,    : ��̐F
// colorFront,   : �O�̐F
// 
//--------------------------------------------

float3 CubicColor(float3 color, float3 N, 
                  float3 colorTop, float3 colorBottom, float3 colorRight,
                  float3 colorLeft, float3 colorBack, float3 colorFront)
{
    // �����x�N�g�����
    float3 vecTop    = {  0,  1,  0 };
    float3 vecBottom = {  0, -1,  0 };
    float3 vecRight  = {  1,  0,  0 };
    float3 vecLeft   = { -1,  0,  0 };
    float3 vecBack   = {  0,  0,  1 };
    float3 vecFront  = {  0,  0, -1 };
    
    // �����x�N�g����N�̌v�Z
    float dirTop    = max(0, dot(N, vecTop));
    float dirBottom = max(0, dot(N, vecBottom));
    float dirRight  = max(0, dot(N, vecRight));
    float dirLeft   = max(0, dot(N, vecLeft));
    float dirBack   = max(0, dot(N, vecBack));
    float dirFront  = max(0, dot(N, vecFront));
    
    float3 newColor;
    float3 white = { 1, 1, 1 };
    
    newColor = lerp(colorTop, white, 1 - dirTop) * lerp(colorBottom, white, 1 - dirBottom)
               * lerp(colorRight, white, 1 - dirRight) * lerp(colorLeft, white, 1 - dirLeft)
               * lerp(colorBack, white, 1 - dirBack) * lerp(colorFront, white, 1 - dirFront);
    
    
    return color + newColor;
};

float4 CubicColor(float4 color, float3 N,
                  float4 colorTop, float4 colorBottom, float4 colorRight,
                  float4 colorLeft, float4 colorBack, float4 colorFront, float colorAlpha)
{
    // �����x�N�g�����
    float3 vecTop = { 0, 1, 0 };
    float3 vecBottom = { 0, -1, 0 };
    float3 vecRight = { 1, 0, 0 };
    float3 vecLeft = { -1, 0, 0 };
    float3 vecBack = { 0, 0, 1 };
    float3 vecFront = { 0, 0, -1 };
    
    // �����x�N�g����N�̌v�Z
    float dirTop = max(0, dot(N, vecTop));
    float dirBottom = max(0, dot(N, vecBottom));
    float dirRight = max(0, dot(N, vecRight));
    float dirLeft = max(0, dot(N, vecLeft));
    float dirBack = max(0, dot(N, vecBack));
    float dirFront = max(0, dot(N, vecFront));
    
    float4 newColor;
    float4 white = { 1, 1, 1, 1 };
    
    newColor = color + (lerp(colorTop, white, 1 - dirTop) * lerp(colorBottom, white, 1 - dirBottom)
               * lerp(colorRight, white, 1 - dirRight) * lerp(colorLeft, white, 1 - dirLeft)
               * lerp(colorBack, white, 1 - dirBack) * lerp(colorFront, white, 1 - dirFront));
    
    return lerp(color, newColor, colorAlpha);
};

float4 CubicColor(float4 color, float3 N,
                  float4 colorTop1, float4 colorBottom1, float4 colorRight1,
                  float4 colorLeft1, float4 colorBack1, float4 colorFront1, 
                  float4 colorTop2, float4 colorBottom2, float4 colorRight2,
                  float4 colorLeft2, float4 colorBack2, float4 colorFront2, 
                  float colorAlpha)
{
    // �����x�N�g�����
    float3 vecTop = { 0, 1, 0 };
    float3 vecBottom = { 0, -1, 0 };
    float3 vecRight = { 1, 0, 0 };
    float3 vecLeft = { -1, 0, 0 };
    float3 vecBack = { 0, 0, 1 };
    float3 vecFront = { 0, 0, -1 };
    
    // �����x�N�g����N�̌v�Z
    float dirTop = max(0, dot(N, vecTop));
    float dirBottom = max(0, dot(N, vecBottom));
    float dirRight = max(0, dot(N, vecRight));
    float dirLeft = max(0, dot(N, vecLeft));
    float dirBack = max(0, dot(N, vecBack));
    float dirFront = max(0, dot(N, vecFront));
    
    
    // 6�����̐F�쐬
    //�����
    float4 colorTop = lerp(colorTop1, colorTop2, 0.5f);
    
    //������
    float4 colorBottom = lerp(colorBottom1, colorBottom2, 0.5f);
    
    //�E����
    float4 colorRight = lerp(colorRight1, colorRight2, 0.5f);
    
    //������
    float4 colorLeft = lerp(colorLeft1, colorLeft2, 0.5f);
    
    //�����
    float4 colorBack = lerp(colorBack1, colorBack2, 0.5f);
    
    //�O����
    float4 colorFront = lerp(colorFront1, colorFront2, 0.5f);
    
    
    
    float4 newColor;
    float4 white = { 1, 1, 1, 1 };
    
    newColor = color + (lerp(colorTop, white, 1 - dirTop) * lerp(colorBottom, white, 1 - dirBottom)
               * lerp(colorRight, white, 1 - dirRight) * lerp(colorLeft, white, 1 - dirLeft)
               * lerp(colorBack, white, 1 - dirBack) * lerp(colorFront, white, 1 - dirFront));
    
    return lerp(color, newColor, colorAlpha);
};

float4 CubicColor(float4 color, float3 N,
                  HexColor color1, HexColor color2,
                  float colorAlpha)
{
    // �����x�N�g�����
    float3 vecTop = { 0, 1, 0 };
    float3 vecBottom = { 0, -1, 0 };
    float3 vecRight = { 1, 0, 0 };
    float3 vecLeft = { -1, 0, 0 };
    float3 vecBack = { 0, 0, 1 };
    float3 vecFront = { 0, 0, -1 };
    
    // �����x�N�g����N�̌v�Z
    float dirTop = max(0, dot(N, vecTop));
    float dirBottom = max(0, dot(N, vecBottom));
    float dirRight = max(0, dot(N, vecRight));
    float dirLeft = max(0, dot(N, vecLeft));
    float dirBack = max(0, dot(N, vecBack));
    float dirFront = max(0, dot(N, vecFront));
    
    
    // 6�����̐F�쐬
    //�����
    float4 colorTop = lerp(color1.colorTop, color2.colorTop, 0.5f);
    
    //������
    float4 colorBottom = lerp(color1.colorBottom, color2.colorBottom, 0.5f);
    
    //�E����
    float4 colorRight = lerp(color1.colorRight, color2.colorRight, 0.5f);
    
    //������
    float4 colorLeft = lerp(color1.colorLeft, color2.colorLeft, 0.5f);
    
    //�����
    float4 colorBack = lerp(color1.colorBack, color2.colorBack, 0.5f);
    
    //�O����
    float4 colorFront = lerp(color1.colorFront, color2.colorFront, 0.5f);
    
    
    
    float4 newColor;
    float4 white = { 1, 1, 1, 1 };
    
    newColor = color + (lerp(colorTop, white, 1 - dirTop) * lerp(colorBottom, white, 1 - dirBottom)
               * lerp(colorRight, white, 1 - dirRight) * lerp(colorLeft, white, 1 - dirLeft)
               * lerp(colorBack, white, 1 - dirBack) * lerp(colorFront, white, 1 - dirFront));
    
    return lerp(color, newColor, colorAlpha);
};