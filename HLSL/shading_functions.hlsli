

// 六方向色用構造体
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
//  ランバート拡散反射計算関数
//-----------------------------------------------
//  N: 法線(正規化済み)
//  L: 入射ベクトル(正規化済み)
//  C: 入射光(色、強さ)
//  K: 反射率
//-----------------------------------------------
float3 CalcLambert(float3 N, float3 L, float3 C, float3 K)
{
    float power = saturate(dot(N, -L));
    return C * power * K;
}


//-----------------------------------------------
//  フォンの鏡面反射計算関数
//-----------------------------------------------
//  N: 法線(正規化済み)
//  L: 入射ベクトル(正規化済み)
//  E: 視線ベクトル(正規化済み)
//  C: 入射光(色、強さ)
//  K: 反射率
//-----------------------------------------------
float3 CalcPhongSpeculer(float3 N, float3 L, float3 E, float3 C, float3 K)
{
    float3 R = reflect(L, N);
    float power = max(dot(-E, R), 0);
    power = pow(power, 128);
    
    return C * power * K.rgb;
}


//-----------------------------------------------
//  ハーフランバート拡散反射計算関数
//-----------------------------------------------
//  N: 法線(正規化済み)
//  L: 入射ベクトル(正規化済み)
//  C: 入射光(色、強さ)
//  K: 反射率
//-----------------------------------------------
float3 CalcHalfLambert(float3 N, float3 L, float3 C, float3 K)
{
    float D = saturate(dot(N, -L) * 0.5f + 0.5f);
    
    return C * D * K;
}


//-----------------------------------------------
//  リムライト
//-----------------------------------------------
//  N: 法線(正規化済み)
//  E: 視線方向ベクトル(正規化済み)
//  L: 入射ベクトル(正規化済み)
//  C: ライト色
//　RimPower: リムライトの強さ
//-----------------------------------------------
float3 CalcRimLight(float3 N,float3 E,float3 L, float3 C, float3 RimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(N, -E));
    
    return C * pow(rim, RimPower) * saturate(dot(L, -E));
}


//--------------------------------------------
// ランプシェーディング
//--------------------------------------------
// tex:ランプシェーディング用テクスチャ
// samp:ランプシェーディング用サンプラステート
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
//--------------------------------------------
float3 CalcRampShading(Texture2D tex, SamplerState samp, float3 N, float3 L, float3 C, float3 K)
{
    float D = saturate(dot(N, -L) * 0.5f + 0.5f);
    float Ramp = tex.Sample(samp, float2(D, 0.5f)).r;
    return C * Ramp * K.rgb;
}


//--------------------------------------------
// 球体環境マッピング
//--------------------------------------------
// tex:ランプシェーディング用テクスチャ
// samp:ランプシェーディング用サンプラステート
// color: 現在のピクセル色
// N:法線(正規化済み)
// E:入射光(色・強さ)
// value:適応率
//--------------------------------------------
float3 CalcSphereEnvironment(Texture2D tex, SamplerState samp, in float3 color,
                                                float3 N, float3 E, float value)
{
    float3 R = reflect(E, N);
    float2 texcoord = R.xy * 0.5f + 0.5f;
    
    return lerp(color.rgb, tex.Sample(samp, texcoord).rgb, value);
    
}


//--------------------------------------------
// 半球ライティング
//--------------------------------------------
// normal:法線(正規化済み)
// up:上方向（片方）
// sky_color:空(上)色
// ground_color:地面(下)色
// hemisphere_weight:重み
//--------------------------------------------

float3 CalcHemiSphereLight(float3 normal, float3 up, float3 sky_color, float3 ground_color, float4 hemisphere_weight)
{
    float facter = dot(normal, up) * 0.5f + 0.5f;
    return lerp(ground_color, sky_color, facter) * hemisphere_weight.x;
    
}


//--------------------------------------------
// フォグ
//--------------------------------------------
// color : 現在のピクセル色
// fog_color : フォグの色
// fog_range : フォグの範囲情報
// eye_length : 視点からの距離
//--------------------------------------------
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
    float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
    
    return lerp(color, fog_color, fogAlpha);
}

//--------------------------------------------
//  cubic color
//--------------------------------------------
// color : 現在のピクセル色
// N : 法線
// 
// colorTop,     : 上の色
// colorBottom,  : 下の色
// colorRight,   : 右の色
// colorLeft,    : 左の色
// colorBack,    : 後の色
// colorFront,   : 前の色
// 
//--------------------------------------------

float3 CubicColor(float3 color, float3 N, 
                  float3 colorTop, float3 colorBottom, float3 colorRight,
                  float3 colorLeft, float3 colorBack, float3 colorFront)
{
    // 方向ベクトル作る
    float3 vecTop    = {  0,  1,  0 };
    float3 vecBottom = {  0, -1,  0 };
    float3 vecRight  = {  1,  0,  0 };
    float3 vecLeft   = { -1,  0,  0 };
    float3 vecBack   = {  0,  0,  1 };
    float3 vecFront  = {  0,  0, -1 };
    
    // 方向ベクトルとNの計算
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
    // 方向ベクトル作る
    float3 vecTop = { 0, 1, 0 };
    float3 vecBottom = { 0, -1, 0 };
    float3 vecRight = { 1, 0, 0 };
    float3 vecLeft = { -1, 0, 0 };
    float3 vecBack = { 0, 0, 1 };
    float3 vecFront = { 0, 0, -1 };
    
    // 方向ベクトルとNの計算
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
    // 方向ベクトル作る
    float3 vecTop = { 0, 1, 0 };
    float3 vecBottom = { 0, -1, 0 };
    float3 vecRight = { 1, 0, 0 };
    float3 vecLeft = { -1, 0, 0 };
    float3 vecBack = { 0, 0, 1 };
    float3 vecFront = { 0, 0, -1 };
    
    // 方向ベクトルとNの計算
    float dirTop = max(0, dot(N, vecTop));
    float dirBottom = max(0, dot(N, vecBottom));
    float dirRight = max(0, dot(N, vecRight));
    float dirLeft = max(0, dot(N, vecLeft));
    float dirBack = max(0, dot(N, vecBack));
    float dirFront = max(0, dot(N, vecFront));
    
    
    // 6方向の色作成
    //上方向
    float4 colorTop = lerp(colorTop1, colorTop2, 0.5f);
    
    //下方向
    float4 colorBottom = lerp(colorBottom1, colorBottom2, 0.5f);
    
    //右方向
    float4 colorRight = lerp(colorRight1, colorRight2, 0.5f);
    
    //左方向
    float4 colorLeft = lerp(colorLeft1, colorLeft2, 0.5f);
    
    //後方向
    float4 colorBack = lerp(colorBack1, colorBack2, 0.5f);
    
    //前方向
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
    // 方向ベクトル作る
    float3 vecTop = { 0, 1, 0 };
    float3 vecBottom = { 0, -1, 0 };
    float3 vecRight = { 1, 0, 0 };
    float3 vecLeft = { -1, 0, 0 };
    float3 vecBack = { 0, 0, 1 };
    float3 vecFront = { 0, 0, -1 };
    
    // 方向ベクトルとNの計算
    float dirTop = max(0, dot(N, vecTop));
    float dirBottom = max(0, dot(N, vecBottom));
    float dirRight = max(0, dot(N, vecRight));
    float dirLeft = max(0, dot(N, vecLeft));
    float dirBack = max(0, dot(N, vecBack));
    float dirFront = max(0, dot(N, vecFront));
    
    
    // 6方向の色作成
    //上方向
    float4 colorTop = lerp(color1.colorTop, color2.colorTop, 0.5f);
    
    //下方向
    float4 colorBottom = lerp(color1.colorBottom, color2.colorBottom, 0.5f);
    
    //右方向
    float4 colorRight = lerp(color1.colorRight, color2.colorRight, 0.5f);
    
    //左方向
    float4 colorLeft = lerp(color1.colorLeft, color2.colorLeft, 0.5f);
    
    //後方向
    float4 colorBack = lerp(color1.colorBack, color2.colorBack, 0.5f);
    
    //前方向
    float4 colorFront = lerp(color1.colorFront, color2.colorFront, 0.5f);
    
    
    
    float4 newColor;
    float4 white = { 1, 1, 1, 1 };
    
    newColor = color + (lerp(colorTop, white, 1 - dirTop) * lerp(colorBottom, white, 1 - dirBottom)
               * lerp(colorRight, white, 1 - dirRight) * lerp(colorLeft, white, 1 - dirLeft)
               * lerp(colorBack, white, 1 - dirBack) * lerp(colorFront, white, 1 - dirFront));
    
    return lerp(color, newColor, colorAlpha);
};