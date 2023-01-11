#include "SkyBox.h"

SkyBox::SkyBox(ID3D11Device* device)
{
    shader = new SkyShader(device);
    model = new Model("Data/Model/Sphere/sphere1.mdl");
}

SkyBox::~SkyBox()
{
    delete model;
    delete shader;
}

void SkyBox::Render(const RenderContext& rc)
{
    
    shader->Begin(rc);
    shader->Draw(rc, model);
    shader->End(rc);
}

void SkyBox::Render(const RenderContext& rc, ModelShader* shader)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;
    myRc.heightFogFlag = 0;    

    shader->Begin(rc);
    shader->Draw(rc, model);
    shader->End(rc);
}

void SkyBox::UpdateTransform()
{

    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);


    // 回転行列を作成
    //DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(0.0f);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(0.0f);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(0.0f);

    DirectX::XMMATRIX R = Y * X * Z;                    // 数学ベースでは Z * X * Y


    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);


    // ３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;    //左手系




    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);

}

void SkyBox::DebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // ターゲット位置をデバッグ球描画
    //debugRenderer->DrawSphere(position, 3, DirectX::XMFLOAT4(1, 0, 1, 1));
}
