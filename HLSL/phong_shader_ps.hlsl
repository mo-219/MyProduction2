#include "phong_shader.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);

Texture2D normal_map : register(t1);

Texture2D shadow_map : register(t4);
SamplerState shadow_sampler_state : register(s4);

float4 main(VS_OUT pin) : SV_TARGET
{

    float4 diffuse_color = color_map.Sample(color_sampler_state, pin.texcoord);
    
    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);     // 視線のベクトル
    float3 L = normalize(directional_light_direction.xyz);                  // 光のベクトル
    
    //float3 N = normalize(pin.world_normal.xyz);                             // 法線ベクトル
    
    float3x3 mat = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
    float3 N = normal_map.Sample(color_sampler_state, pin.texcoord).rgb;
    
    // ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    
    //
    
    
    float3 ambient = ambient_color.rgb * Ka.rgb;
    ambient += CalcHemiSphereLight(N, float3(0, 1, 0), sky_color.rgb, ground_color.rgb, hemisphere_weight);
    
    float3 directional_diffuse = CalcLambert(N, L, directional_light_color.rgb, Kd.rgb);
    float3 directional_specular = CalcPhongSpeculer(N, L, E, directional_light_color.rgb, Ks.rgb);
    

    
    
    // グラデーション用(color1とcolor2でグラデーション)
    HexColor color1;
    color1.colorTop     = colorTop1;
    color1.colorBottom  = colorBottom1;
    color1.colorRight   = colorRight1;
    color1.colorLeft    = colorLeft1;
    color1.colorBack    = colorBack1;
    color1.colorFront   = colorFront1;
    
    HexColor color2;
    color2.colorTop     = colorTop2;
    color2.colorBottom  = colorBottom2;
    color2.colorRight   = colorRight2;
    color2.colorLeft    = colorLeft2;
    color2.colorBack    = colorBack2;
    color2.colorFront   = colorFront2;
    
    
   
    float4 color = float4(ambient, diffuse_color.a);
    color.rgb += diffuse_color.rgb * directional_diffuse;
    color.rgb += directional_specular;
    color = CubicColor(color, N, color1, color2, colorAlpha.a);
    color = CalcFog(color, fog_color, fog_range.xy, length(pin.world_position.xyz - camera_position.xyz));
    
    // シャドウマップから深度値取得
    float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
    // 深度値を比較して影かどうかを判定する
    if (pin.shadow_texcoord.z - depth > shadow_bias)
    {
        color.rgb *= shadow_color.rgb;
    }
    
    
    return color;

}


#if false
    //float3 directional_diffuse = 0;
    //{
    //    float power = saturate(dot(N, -L));
    //    directional_diffuse = directional_light_color.rgb * power * Kd.rgb;
    //}
    //float3 directional_specular = 0;
    //{
    //    float3 R = reflect(L, N);               // どれだけ反射するか           反射ベクトル
    //    float power = max(dot(E, R),0);         // どれだけスペキュラーが強いか  反射ベクトルと視線のベクトルの内積    
    //    power = pow(power, 128);
    //    directional_specular = directional_light_color.rgb * power * Ks.rbg;
    //}
#endif